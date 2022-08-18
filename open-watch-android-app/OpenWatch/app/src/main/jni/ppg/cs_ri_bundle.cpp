#ifdef CS_COMPLEX
#undef CS_COMPLEX
#endif

/* cs_add.c */

#include "cs.h"
/* C = alpha*A + beta*B */
cs* cs_add(const cs* A, const cs* B, CS_ENTRY alpha, CS_ENTRY beta) {
    CS_INT p, j, nz = 0, anz, *Cp, *Ci, *Bp, m, n, bnz, *w, values;
    CS_ENTRY *x, *Bx, *Cx;
    cs* C;
    if (!CS_CSC(A) || !CS_CSC(B)) {
        return (NULL);
    } /* check inputs */
    if (A->m != B->m || A->n != B->n) {
        return (NULL);
    }
    m = A->m;
    anz = A->p[A->n];
    n = B->n;
    Bp = B->p;
    Bx = B->x;
    bnz = Bp[n];
    w = (CS_INT*)cs_calloc(m, sizeof(CS_INT)); /* get workspace */
    values = (A->x != NULL) && (Bx != NULL);
    x = values ? (CS_ENTRY*)cs_malloc(m, sizeof(CS_ENTRY)) : NULL; /* get workspace */
    C = cs_spalloc(m, n, anz + bnz, values, 0);         /* allocate result*/
    if (!C || !w || (values && !x)) {
        return (cs_done(C, w, x, 0));
    }
    Cp = C->p;
    Ci = C->i;
    Cx = C->x;
    for (j = 0; j < n; j++) {
        Cp[j] = nz;                                       /* column j of C starts here */
        nz = cs_scatter(A, j, alpha, w, x, j + 1, C, nz); /* alpha*A(:,j)*/
        nz = cs_scatter(B, j, beta, w, x, j + 1, C, nz);  /* beta*B(:,j) */
        if (values)
            for (p = Cp[j]; p < nz; p++) {
                Cx[p] = x[Ci[p]];
            }
    }
    Cp[n] = nz;                   /* finalize the last column of C */
    cs_sprealloc(C, 0);           /* remove extra space from C */
    return (cs_done(C, w, x, 1)); /* success; free workspace, return C */
}

/* cs_amd.c */

#include "cs.h"
/* clear w */
static CS_INT cs_wclear(CS_INT mark, CS_INT lemax, CS_INT* w, CS_INT n) {
    CS_INT k;
    if (mark < 2 || (mark + lemax < 0)) {
        for (k = 0; k < n; k++)
            if (w[k] != 0) {
                w[k] = 1;
            }
        mark = 2;
    }
    return (mark); /* at this point, w [0..n-1] < mark holds */
}

/* keep off-diagonal entries; drop diagonal entries */
static CS_INT cs_diag(CS_INT i, CS_INT j, CS_ENTRY aij, void* other) {
    (void) aij;
    (void) other;
    return (i != j);
}

/* p = amd(A+A') if symmetric is true, or amd(A'A) otherwise */
CS_INT* cs_amd(CS_INT order, const cs* A) { /* order 0:natural, 1:Chol, 2:LU, 3:QR */
    cs *C, *A2, *AT;
    CS_INT *Cp, *Ci, *last, *W, *len, *nv, *next, *P, *head, *elen, *degree, *w, *hhead, *ATp, *ATi,
        d, dk, dext, lemax = 0, e, elenk, eln, i, j, k, k1, k2, k3, jlast, ln, dense, nzmax,
                     mindeg = 0, nvi, nvj, nvk, mark, wnvi, ok, cnz, nel = 0, p, p1, p2, p3, p4, pj,
                     pk, pk1, pk2, pn, q, n, m, t;
    CS_INT h;
    /* --- Construct matrix C ----------------------------------------------- */
    if (!CS_CSC(A) || order <= 0 || order > 3) {
        return (NULL);
    }                        /* check */
    AT = cs_transpose(A, 0); /* compute A' */
    if (!AT) {
        return (NULL);
    }
    m = A->m;
    n = A->n;
    dense = (CS_INT)CS_MAX(16, 10 * sqrt((double)n)); /* find dense threshold */
    dense = CS_MIN(n - 2, dense);
    if (order == 1 && n == m) {
        C = cs_add(A, AT, cs_createFromDouble(0), cs_createFromDouble(0)); /* C = A+A' */
    } else if (order == 2) {
        ATp = AT->p; /* drop dense columns from AT */
        ATi = AT->i;
        for (p2 = 0, j = 0; j < m; j++) {
            p = ATp[j];  /* column j of AT starts here */
            ATp[j] = p2; /* new column j starts here */
            if (ATp[j + 1] - p > dense) {
                continue;
            } /* skip dense col j */
            for (; p < ATp[j + 1]; p++) {
                ATi[p2++] = ATi[p];
            }
        }
        ATp[m] = p2;                         /* finalize AT */
        A2 = cs_transpose(AT, 0);            /* A2 = AT' */
        C = A2 ? cs_multiply(AT, A2) : NULL; /* C=A'*A with no dense rows */
        cs_spfree(A2);
    } else {
        C = cs_multiply(AT, A); /* C=A'*A */
    }
    cs_spfree(AT);
    if (!C) {
        return (NULL);
    }
    cs_fkeep(C, &cs_diag, NULL); /* drop diagonal entries */
    Cp = C->p;
    cnz = Cp[n];
    P = (CS_INT*)cs_malloc(n + 1, sizeof(CS_INT));       /* allocate result */
    W = (CS_INT*)cs_malloc(8 * (n + 1), sizeof(CS_INT)); /* get workspace */
    t = cnz + cnz / 5 + 2 * n;                  /* add elbow room to C */
    if (!P || !W || !cs_sprealloc(C, t)) {
        return (cs_idone(P, C, W, 0));
    }
    len = W;
    nv = W + (n + 1);
    next = W + 2 * (n + 1);
    head = W + 3 * (n + 1);
    elen = W + 4 * (n + 1);
    degree = W + 5 * (n + 1);
    w = W + 6 * (n + 1);
    hhead = W + 7 * (n + 1);
    last = P; /* use P as workspace for last */
    /* --- Initialize quotient graph ---------------------------------------- */
    for (k = 0; k < n; k++) {
        len[k] = Cp[k + 1] - Cp[k];
    }
    len[n] = 0;
    nzmax = C->nzmax;
    Ci = C->i;
    for (i = 0; i <= n; i++) {
        head[i] = -1; /* degree list i is empty */
        last[i] = -1;
        next[i] = -1;
        hhead[i] = -1;      /* hash list i is empty */
        nv[i] = 1;          /* node i is just one node */
        w[i] = 1;           /* node i is alive */
        elen[i] = 0;        /* Ek of node i is empty */
        degree[i] = len[i]; /* degree of node i */
    }
    mark = cs_wclear(0, 0, w, n); /* clear w */
    elen[n] = -2;                 /* n is a dead element */
    Cp[n] = -1;                   /* n is a root of assembly tree */
    w[n] = 0;                     /* n is a dead element */
    /* --- Initialize degree lists ------------------------------------------ */
    for (i = 0; i < n; i++) {
        d = degree[i];
        if (d == 0) {     /* node i is empty */
            elen[i] = -2; /* element i is dead */
            nel++;
            Cp[i] = -1; /* i is a root of assembly tree */
            w[i] = 0;
        } else if (d > dense) { /* node i is dense */
            nv[i] = 0;          /* absorb i into element n */
            elen[i] = -1;       /* node i is dead */
            nel++;
            Cp[i] = CS_FLIP(n);
            nv[n]++;
        } else {
            if (head[d] != -1) {
                last[head[d]] = i;
            }
            next[i] = head[d]; /* put node i in degree list d */
            head[d] = i;
        }
    }
    while (nel < n) { /* while (selecting pivots) do */
        /* --- Select node of minimum approximate degree -------------------- */
        for (k = -1; mindeg < n && (k = head[mindeg]) == -1; mindeg++)
            ;
        if (next[k] != -1) {
            last[next[k]] = -1;
        }
        head[mindeg] = next[k]; /* remove k from degree list */
        elenk = elen[k];        /* elenk = |Ek| */
        nvk = nv[k];            /* # of nodes k represents */
        nel += nvk;             /* nv[k] nodes of A eliminated */
        /* --- Garbage collection ------------------------------------------- */
        if (elenk > 0 && cnz + mindeg >= nzmax) {
            for (j = 0; j < n; j++) {
                if ((p = Cp[j]) >= 0) { /* j is a live node or element */
                    Cp[j] = Ci[p];      /* save first entry of object */
                    Ci[p] = CS_FLIP(j); /* first entry is now CS_FLIP(j) */
                }
            }
            for (q = 0, p = 0; p < cnz;) {         /* scan all of memory */
                if ((j = CS_FLIP(Ci[p++])) >= 0) { /* found object j */
                    Ci[q] = Cp[j];                 /* restore first entry of object */
                    Cp[j] = q++;                   /* new pointer to object j */
                    for (k3 = 0; k3 < len[j] - 1; k3++) {
                        Ci[q++] = Ci[p++];
                    }
                }
            }
            cnz = q; /* Ci [cnz...nzmax-1] now free */
        }
        /* --- Construct new element ---------------------------------------- */
        dk = 0;
        nv[k] = -nvk; /* flag k as in Lk */
        p = Cp[k];
        pk1 = (elenk == 0) ? p : cnz; /* do in place if elen[k] == 0 */
        pk2 = pk1;
        for (k1 = 1; k1 <= elenk + 1; k1++) {
            if (k1 > elenk) {
                e = k;               /* search the nodes in k */
                pj = p;              /* list of nodes starts at Ci[pj]*/
                ln = len[k] - elenk; /* length of list of nodes in k */
            } else {
                e = Ci[p++]; /* search the nodes in e */
                pj = Cp[e];
                ln = len[e]; /* length of list of nodes in e */
            }
            for (k2 = 1; k2 <= ln; k2++) {
                i = Ci[pj++];
                if ((nvi = nv[i]) <= 0) {
                    continue;
                }              /* node i dead, or seen */
                dk += nvi;     /* degree[Lk] += size of node i */
                nv[i] = -nvi;  /* negate nv[i] to denote i in Lk*/
                Ci[pk2++] = i; /* place i in Lk */
                if (next[i] != -1) {
                    last[next[i]] = last[i];
                }
                if (last[i] != -1) { /* remove i from degree list */
                    next[last[i]] = next[i];
                } else {
                    head[degree[i]] = next[i];
                }
            }
            if (e != k) {
                Cp[e] = CS_FLIP(k); /* absorb e into k */
                w[e] = 0;           /* e is now a dead element */
            }
        }
        if (elenk != 0) {
            cnz = pk2;
        }               /* Ci [cnz...nzmax] is free */
        degree[k] = dk; /* external degree of k - |Lk\i| */
        Cp[k] = pk1;    /* element k is in Ci[pk1..pk2-1] */
        len[k] = pk2 - pk1;
        elen[k] = -2; /* k is now an element */
        /* --- Find set differences ----------------------------------------- */
        mark = cs_wclear(mark, lemax, w, n); /* clear w if necessary */
        for (pk = pk1; pk < pk2; pk++) {     /* scan 1: find |Le\Lk| */
            i = Ci[pk];
            if ((eln = elen[i]) <= 0) {
                continue;
            }             /* skip if elen[i] empty */
            nvi = -nv[i]; /* nv [i] was negated */
            wnvi = mark - nvi;
            for (p = Cp[i]; p <= Cp[i] + eln - 1; p++) { /* scan Ei */
                e = Ci[p];
                if (w[e] >= mark) {
                    w[e] -= nvi;             /* decrement |Le\Lk| */
                } else if (w[e] != 0) {      /* ensure e is a live element */
                    w[e] = degree[e] + wnvi; /* 1st time e seen in scan 1 */
                }
            }
        }
        /* --- Degree update ------------------------------------------------ */
        for (pk = pk1; pk < pk2; pk++) { /* scan2: degree update */
            i = Ci[pk];                  /* consider node i in Lk */
            p1 = Cp[i];
            p2 = p1 + elen[i] - 1;
            pn = p1;
            for (h = 0, d = 0, p = p1; p <= p2; p++) { /* scan Ei */
                e = Ci[p];
                if (w[e] != 0) {        /* e is an unabsorbed element */
                    dext = w[e] - mark; /* dext = |Le\Lk| */
                    if (dext > 0) {
                        d += dext;    /* sum up the set differences */
                        Ci[pn++] = e; /* keep e in Ei */
                        h += e;       /* compute the hash of node i */
                    } else {
                        Cp[e] = CS_FLIP(k); /* aggressive absorb. e->k */
                        w[e] = 0;           /* e is a dead element */
                    }
                }
            }
            elen[i] = pn - p1 + 1; /* elen[i] = |Ei| */
            p3 = pn;
            p4 = p1 + len[i];
            for (p = p2 + 1; p < p4; p++) { /* prune edges in Ai */
                j = Ci[p];
                if ((nvj = nv[j]) <= 0) {
                    continue;
                }             /* node j dead or in Lk */
                d += nvj;     /* degree(i) += |j| */
                Ci[pn++] = j; /* place j in node list of i */
                h += j;       /* compute hash for node i */
            }
            if (d == 0) {           /* check for mass elimination */
                Cp[i] = CS_FLIP(k); /* absorb i into k */
                nvi = -nv[i];
                dk -= nvi;  /* |Lk| -= |i| */
                nvk += nvi; /* |k| += nv[i] */
                nel += nvi;
                nv[i] = 0;
                elen[i] = -1; /* node i is dead */
            } else {
                degree[i] = CS_MIN(degree[i], d); /* update degree(i) */
                Ci[pn] = Ci[p3];                  /* move first node to end */
                Ci[p3] = Ci[p1];                  /* move 1st el. to end of Ei */
                Ci[p1] = k;                       /* add k as 1st element in of Ei */
                len[i] = pn - p1 + 1;             /* new len of adj. list of node i */
                h = ((h < 0) ? (-h) : h) % n;     /* finalize hash of i */
                next[i] = hhead[h];               /* place i in hash bucket */
                hhead[h] = i;
                last[i] = h; /* save hash of i in last[i] */
            }
        }               /* scan2 is done */
        degree[k] = dk; /* finalize |Lk| */
        lemax = CS_MAX(lemax, dk);
        mark = cs_wclear(mark + lemax, lemax, w, n); /* clear w */
        /* --- Supernode detection ------------------------------------------ */
        for (pk = pk1; pk < pk2; pk++) {
            i = Ci[pk];
            if (nv[i] >= 0) {
                continue;
            }            /* skip if i is dead */
            h = last[i]; /* scan hash bucket of node i */
            i = hhead[h];
            hhead[h] = -1; /* hash bucket will be empty */
            for (; i != -1 && next[i] != -1; i = next[i], mark++) {
                ln = len[i];
                eln = elen[i];
                for (p = Cp[i] + 1; p <= Cp[i] + ln - 1; p++) {
                    w[Ci[p]] = mark;
                }
                jlast = i;
                for (j = next[i]; j != -1;) { /* compare i with all j */
                    ok = (len[j] == ln) && (elen[j] == eln);
                    for (p = Cp[j] + 1; ok && p <= Cp[j] + ln - 1; p++) {
                        if (w[Ci[p]] != mark) {
                            ok = 0;
                        } /* compare i and j*/
                    }
                    if (ok) {               /* i and j are identical */
                        Cp[j] = CS_FLIP(i); /* absorb j into i */
                        nv[i] += nv[j];
                        nv[j] = 0;
                        elen[j] = -1; /* node j is dead */
                        j = next[j];  /* delete j from hash bucket */
                        next[jlast] = j;
                    } else {
                        jlast = j; /* j and i are different */
                        j = next[j];
                    }
                }
            }
        }
        /* --- Finalize new element------------------------------------------ */
        for (p = pk1, pk = pk1; pk < pk2; pk++) { /* finalize Lk */
            i = Ci[pk];
            if ((nvi = -nv[i]) <= 0) {
                continue;
            }                         /* skip if i is dead */
            nv[i] = nvi;              /* restore nv[i] */
            d = degree[i] + dk - nvi; /* compute external degree(i) */
            d = CS_MIN(d, n - nel - nvi);
            if (head[d] != -1) {
                last[head[d]] = i;
            }
            next[i] = head[d]; /* put i back in degree list */
            last[i] = -1;
            head[d] = i;
            mindeg = CS_MIN(mindeg, d); /* find new minimum degree */
            degree[i] = d;
            Ci[p++] = i; /* place i in Lk */
        }
        nv[k] = nvk;                   /* # nodes absorbed into k */
        if ((len[k] = p - pk1) == 0) { /* length of adj list of element k*/
            Cp[k] = -1;                /* k is a root of the tree */
            w[k] = 0;                  /* k is now a dead element */
        }
        if (elenk != 0) {
            cnz = p;
        } /* free unused space in Lk */
    }
    /* --- Postordering ----------------------------------------------------- */
    for (i = 0; i < n; i++) {
        Cp[i] = CS_FLIP(Cp[i]);
    } /* fix assembly tree */
    for (j = 0; j <= n; j++) {
        head[j] = -1;
    }
    for (j = n; j >= 0; j--) { /* place unordered nodes in lists */
        if (nv[j] > 0) {
            continue;
        }                      /* skip if j is an element */
        next[j] = head[Cp[j]]; /* place j in list of its parent */
        head[Cp[j]] = j;
    }
    for (e = n; e >= 0; e--) { /* place elements in lists */
        if (nv[e] <= 0) {
            continue;
        } /* skip unless e is an element */
        if (Cp[e] != -1) {
            next[e] = head[Cp[e]]; /* place e in list of its parent */
            head[Cp[e]] = e;
        }
    }
    for (k = 0, i = 0; i <= n; i++) { /* postorder the assembly tree */
        if (Cp[i] == -1) {
            k = cs_tdfs(i, k, head, next, P, w);
        }
    }
    return (cs_idone(P, C, W, 1));
}

/* cs_chol.c */

#include "cs.h"
/* L = chol (A, [pinv parent cp]), pinv is optional */
csn* cs_chol(const cs* A, const css* S) {
    CS_ENTRY d, lki, *Lx, *x, *Cx;
    CS_INT top, i, p, k, n, *Li, *Lp, *cp, *pinv, *s, *c, *parent, *Cp, *Ci;
    cs *L, *C, *E;
    csn* N;
    if (!CS_CSC(A) || !S || !S->cp || !S->parent) {
        return (NULL);
    }
    n = A->n;
    N = (csn*)cs_calloc(1, sizeof(csn));        /* allocate result */
    c = (CS_INT*)cs_malloc(2 * n, sizeof(CS_INT)); /* get CS_INT workspace */
    x = (CS_ENTRY*)cs_malloc(n, sizeof(CS_ENTRY));   /* get CS_ENTRY workspace */
    cp = S->cp;
    pinv = S->pinv;
    parent = S->parent;
    C = pinv ? cs_symperm(A, pinv, 1) : ((cs*)A);
    E = pinv ? C : NULL; /* E is alias for A, or a copy E=A(p,p) */
    if (!N || !c || !x || !C) {
        return (cs_ndone(N, E, c, x, 0));
    }
    s = c + n;
    Cp = C->p;
    Ci = C->i;
    Cx = C->x;
    N->L = L = cs_spalloc(n, n, cp[n], 1, 0); /* allocate result */
    if (!L) {
        return (cs_ndone(N, E, c, x, 0));
    }
    Lp = L->p;
    Li = L->i;
    Lx = L->x;
    for (k = 0; k < n; k++) {
        Lp[k] = c[k] = cp[k];
    }
    for (k = 0; k < n; k++) { /* compute L(k,:) for L*L' = C */
        /* --- Nonzero pattern of L(k,:) ------------------------------------ */
        top = cs_ereach(C, k, parent, s, c);  /* find pattern of L(k,:) */
        x[k] = cs_createFromDouble(0);             /* x (0:k) is now zero */
        for (p = Cp[k]; p < Cp[k + 1]; p++) { /* x = full(triu(C(:,k))) */
            if (Ci[p] <= k) {
                x[Ci[p]] = Cx[p];
            }
        }
        d = x[k];                 /* d = C(k,k) */
        x[k] = cs_createFromDouble(0); /* x (0:k) is now zero */
        /* --- Triangular solve --------------------------------------------- */
        for (; top < n; top++) {             /* solve L(0:k-1,0:k-1) * x = C(:,k) */
            i = s[top];                      /* s [top..n-1] is pattern of L(k,:) */
            lki = cs_divcc(x[i], Lx[Lp[i]]); /* L(k,i) = x (i) / L(i,i) */
            x[i] = cs_createFromDouble(0);        /* clear x for k+1st iteration */

            for (p = Lp[i] + 1; p < c[i]; p++) {
                x[Li[p]] = cs_subcc(x[Li[p]], cs_mulcc(Lx[p], lki));
            }
            d = cs_subcc(d, cs_mulcc(lki, CS_CONJ(lki))); /* d = d - L(k,i)*L(k,i) */
            p = c[i]++;
            Li[p] = k; /* store L(k,i) in column i */
            Lx[p] = CS_CONJ(lki);
        }
        /* --- Compute L(k,k) ----------------------------------------------- */
        if (CS_REAL(d) <= 0 || CS_IMAG(d) != 0) {
            return (cs_ndone(N, E, c, x, 0)); /* not pos def */
        }
        p = c[k]++;
        Li[p] = k; /* store L(k,k) = sqrt (d) in column k */
        Lx[p] = cs_sqrt(d);
    }
    Lp[n] = cp[n];                    /* finalize L */
    return (cs_ndone(N, E, c, x, 1)); /* success: free E,s,x; return N */
}

/* cs_cholsol.c */

#include "cs.h"
/* x=A\b where A is symmetric positive definite; b overwritten with solution */
CS_INT cs_cholsol(CS_INT order, const cs* A, CS_ENTRY* b) {
    CS_ENTRY* x;
    css* S;
    csn* N;
    CS_INT n, ok;
    if (!CS_CSC(A) || !b) {
        return (0);
    } /* check inputs */
    n = A->n;
    S = cs_schol(order, A);             /* ordering and symbolic analysis */
    N = cs_chol(A, S);                  /* numeric Cholesky factorization */
    x = (CS_ENTRY*)cs_malloc(n, sizeof(CS_ENTRY)); /* get workspace */
    ok = (S && N && x);
    if (ok) {
        cs_ipvec(S->pinv, b, x, n); /* x = P*b */
        cs_lsolve(N->L, x);         /* x = L\x */
        cs_ltsolve(N->L, x);        /* x = L'\x */
        cs_pvec(S->pinv, x, b, n);  /* b = P'*x */
    }
    cs_free(x);
    cs_sfree(S);
    cs_nfree(N);
    return (ok);
}

/* cs_counts.c */

#include "cs.h"
/* column counts of LL'=A or LL'=A'A, given parent & post ordering */
#define HEAD(k, j) (ata ? head[k] : j)
#define NEXT(J) (ata ? next[J] : -1)
static void init_ata(cs* AT, const CS_INT* post, CS_INT* w, CS_INT** head, CS_INT** next) {
    CS_INT i, k, p, m = AT->n, n = AT->m, *ATp = AT->p, *ATi = AT->i;
    *head = w + 4 * n, *next = w + 5 * n + 1;
    for (k = 0; k < n; k++) {
        w[post[k]] = k;
    } /* invert post */
    for (i = 0; i < m; i++) {
        for (k = n, p = ATp[i]; p < ATp[i + 1]; p++) {
            k = CS_MIN(k, w[ATi[p]]);
        }
        (*next)[i] = (*head)[k]; /* place row i in linked list k */
        (*head)[k] = i;
    }
}
CS_INT* cs_counts(const cs* A, const CS_INT* parent, const CS_INT* post, CS_INT ata) {
    CS_INT i, j, k, n, m, J, s, p, q, jleaf, *ATp, *ATi, *maxfirst, *prevleaf, *ancestor,
        *head = NULL, *next = NULL, *colcount, *w, *first, *delta;
    cs* AT;
    if (!CS_CSC(A) || !parent || !post) {
        return (NULL);
    } /* check inputs */
    m = A->m;
    n = A->n;
    s = 4 * n + (ata ? (n + m + 1) : 0);
    delta = colcount = (CS_INT*)cs_malloc(n, sizeof(CS_INT)); /* allocate result */
    w = (CS_INT*)cs_malloc(s, sizeof(CS_INT));                /* get workspace */
    AT = cs_transpose(A, 0);                         /* AT = A' */
    if (!AT || !colcount || !w) {
        return (cs_idone(colcount, AT, w, 0));
    }
    ancestor = w;
    maxfirst = w + n;
    prevleaf = w + 2 * n;
    first = w + 3 * n;
    for (k = 0; k < s; k++) {
        w[k] = -1;
    }                         /* clear workspace w [0..s-1] */
    for (k = 0; k < n; k++) { /* find first [j] */
        j = post[k];
        delta[j] = (first[j] == -1) ? 1 : 0; /* delta[j]=1 if j is a leaf */
        for (; j != -1 && first[j] == -1; j = parent[j]) {
            first[j] = k;
        }
    }
    ATp = AT->p;
    ATi = AT->i;
    if (ata) {
        init_ata(AT, post, w, &head, &next);
    }
    for (i = 0; i < n; i++) {
        ancestor[i] = i;
    } /* each node in its own set */
    for (k = 0; k < n; k++) {
        j = post[k]; /* j is the kth node in postordered etree */
        if (parent[j] != -1) {
            delta[parent[j]]--;
        }                                            /* j is not a root */
        for (J = HEAD(k, j); J != -1; J = NEXT(J)) { /* J=j for LL'=A case */
            for (p = ATp[J]; p < ATp[J + 1]; p++) {
                i = ATi[p];
                q = cs_leaf(i, j, first, maxfirst, prevleaf, ancestor, &jleaf);
                if (jleaf >= 1) {
                    delta[j]++;
                } /* A(i,j) is in skeleton */
                if (jleaf == 2) {
                    delta[q]--;
                } /* account for overlap in q */
            }
        }
        if (parent[j] != -1) {
            ancestor[j] = parent[j];
        }
    }
    for (j = 0; j < n; j++) { /* sum up delta's of each child */
        if (parent[j] != -1) {
            colcount[parent[j]] += colcount[j];
        }
    }
    return (cs_idone(colcount, AT, w, 1)); /* success: free workspace */
}

/* cs_cumsum.c */

#include "cs.h"
/* p [0..n] = cumulative sum of c [0..n-1], and then copy p [0..n-1] into c */
double cs_cumsum(CS_INT* p, CS_INT* c, CS_INT n) {
    CS_INT i, nz = 0;
    double nz2 = 0;
    if (!p || !c) {
        return (-1);
    } /* check inputs */
    for (i = 0; i < n; i++) {
        p[i] = nz;
        nz += c[i];
        nz2 += c[i]; /* also in double to avoid CS_INT overflow */
        c[i] = p[i]; /* also copy p[0..n-1] back into c[0..n-1]*/
    }
    p[n] = nz;
    return (nz2); /* return sum (c [0..n-1]) */
}

/* cs_dfs.c */

#include "cs.h"
/* depth-first-search of the graph of a matrix, starting at node j */
CS_INT cs_dfs(CS_INT j, cs* G, CS_INT top, CS_INT* xi, CS_INT* pstack, const CS_INT* pinv) {
    CS_INT i, p, p2, done, jnew, head = 0, *Gp, *Gi;
    if (!CS_CSC(G) || !xi || !pstack) {
        return (-1);
    } /* check inputs */
    Gp = G->p;
    Gi = G->i;
    xi[0] = j; /* initialize the recursion stack */
    while (head >= 0) {
        j = xi[head]; /* get j from the top of the recursion stack */
        jnew = pinv ? (pinv[j]) : j;
        if (!CS_MARKED(Gp, j)) {
            CS_MARK(Gp, j); /* mark node j as visited */
            pstack[head] = (jnew < 0) ? 0 : CS_UNFLIP(Gp[jnew]);
        }
        done = 1; /* node j done if no unvisited neighbors */
        p2 = (jnew < 0) ? 0 : CS_UNFLIP(Gp[jnew + 1]);
        for (p = pstack[head]; p < p2; p++) { /* examine all neighbors of j */
            i = Gi[p];                        /* consider neighbor node i */
            if (CS_MARKED(Gp, i)) {
                continue;
            }                 /* skip visited node i */
            pstack[head] = p; /* pause depth-first search of node j */
            xi[++head] = i;   /* start dfs at node i */
            done = 0;         /* node j is not done */
            break;            /* break, to start dfs (i) */
        }
        if (done) {        /* depth-first search at node j is done */
            head--;        /* remove j from the recursion stack */
            xi[--top] = j; /* and place in the output stack */
        }
    }
    return (top);
}

/* cs_dmperm.c */

#include "cs.h"
/* breadth-first search for coarse decomposition (C0,C1,R1 or R0,R3,C3) */
static CS_INT cs_bfs(const cs* A,
                     CS_INT n,
                     CS_INT* wi,
                     CS_INT* wj,
                     CS_INT* queue,
                     const CS_INT* imatch,
                     const CS_INT* jmatch,
                     CS_INT mark) {
    CS_INT *Ap, *Ai, head = 0, tail = 0, j, i, p, j2;
    cs* C;
    for (j = 0; j < n; j++) { /* place all unmatched nodes in queue */
        if (imatch[j] >= 0) {
            continue;
        }                  /* skip j if matched */
        wj[j] = 0;         /* j in set C0 (R0 if transpose) */
        queue[tail++] = j; /* place unmatched col j in queue */
    }
    if (tail == 0) {
        return (1);
    } /* quick return if no unmatched nodes */
    C = (mark == 1) ? ((cs*)A) : cs_transpose(A, 0);
    if (!C) {
        return (0);
    } /* bfs of C=A' to find R3,C3 from R0 */
    Ap = C->p;
    Ai = C->i;
    while (head < tail) {  /* while queue is not empty */
        j = queue[head++]; /* get the head of the queue */
        for (p = Ap[j]; p < Ap[j + 1]; p++) {
            i = Ai[p];
            if (wi[i] >= 0) {
                continue;
            }               /* skip if i is marked */
            wi[i] = mark;   /* i in set R1 (C3 if transpose) */
            j2 = jmatch[i]; /* traverse alternating path to j2 */
            if (wj[j2] >= 0) {
                continue;
            }                   /* skip j2 if it is marked */
            wj[j2] = mark;      /* j2 in set C1 (R3 if transpose) */
            queue[tail++] = j2; /* add j2 to queue */
        }
    }
    if (mark != 1) {
        cs_spfree(C);
    } /* free A' if it was created */
    return (1);
}

/* collect matched rows and columns into p and q */
static void cs_matched(CS_INT n,
                       const CS_INT* wj,
                       const CS_INT* imatch,
                       CS_INT* p,
                       CS_INT* q,
                       CS_INT* cc,
                       CS_INT* rr,
                       CS_INT set,
                       CS_INT mark) {
    CS_INT kc = cc[set], j;
    CS_INT kr = rr[set - 1];
    for (j = 0; j < n; j++) {
        if (wj[j] != mark) {
            continue;
        } /* skip if j is not in C set */
        p[kr++] = imatch[j];
        q[kc++] = j;
    }
    cc[set + 1] = kc;
    rr[set] = kr;
}

/* collect unmatched rows into the permutation vector p */
static void cs_unmatched(CS_INT m, const CS_INT* wi, CS_INT* p, CS_INT* rr, CS_INT set) {
    CS_INT i, kr = rr[set];
    for (i = 0; i < m; i++)
        if (wi[i] == 0) {
            p[kr++] = i;
        }
    rr[set + 1] = kr;
}

/* return 1 if row i is in R2 */
static CS_INT cs_rprune(CS_INT i, CS_INT j, CS_ENTRY aij, void* other) {
    (void) aij;
    (void) j;
    CS_INT* rr = (CS_INT*)other;
    return (i >= rr[1] && i < rr[2]);
}

/* Given A, compute coarse and then fine dmperm */
csd* cs_dmperm(const cs* A, CS_INT seed) {
    CS_INT m, n, i, j, k, cnz, nc, *jmatch, *imatch, *wi, *wj, *pinv, *Cp, *Ci, *ps, *rs, nb1, nb2,
        *p, *q, *cc, *rr, *r, *s, ok;
    cs* C;
    csd *D, *scc;
    /* --- Maximum matching ------------------------------------------------- */
    if (!CS_CSC(A)) {
        return (NULL);
    } /* check inputs */
    m = A->m;
    n = A->n;
    D = cs_dalloc(m, n); /* allocate result */
    if (!D) {
        return (NULL);
    }
    p = D->p;
    q = D->q;
    r = D->r;
    s = D->s;
    cc = D->cc;
    rr = D->rr;
    jmatch = cs_maxtrans(A, seed); /* max transversal */
    imatch = jmatch + m;           /* imatch = inverse of jmatch */
    if (!jmatch) {
        return (cs_ddone(D, NULL, jmatch, 0));
    }
    /* --- Coarse decomposition --------------------------------------------- */
    wi = r;
    wj = s; /* use r and s as workspace */
    for (j = 0; j < n; j++) {
        wj[j] = -1;
    } /* unmark all cols for bfs */
    for (i = 0; i < m; i++) {
        wi[i] = -1;
    }                                                /* unmark all rows for bfs */
    cs_bfs(A, n, wi, wj, q, imatch, jmatch, 1);      /* find C1, R1 from C0*/
    ok = cs_bfs(A, m, wj, wi, p, jmatch, imatch, 3); /* find R3, C3 from R0*/
    if (!ok) {
        return (cs_ddone(D, NULL, jmatch, 0));
    }
    cs_unmatched(n, wj, q, cc, 0);                  /* unmatched set C0 */
    cs_matched(n, wj, imatch, p, q, cc, rr, 1, 1);  /* set R1 and C1 */
    cs_matched(n, wj, imatch, p, q, cc, rr, 2, -1); /* set R2 and C2 */
    cs_matched(n, wj, imatch, p, q, cc, rr, 3, 3);  /* set R3 and C3 */
    cs_unmatched(m, wi, p, rr, 3);                  /* unmatched set R0 */
    cs_free(jmatch);
    /* --- Fine decomposition ----------------------------------------------- */
    pinv = cs_pinv(p, m); /* pinv=p' */
    if (!pinv) {
        return (cs_ddone(D, NULL, NULL, 0));
    }
    C = cs_permute(A, pinv, q, 0); /* C=A(p,q) (it will hold A(R2,C2)) */
    cs_free(pinv);
    if (!C) {
        return (cs_ddone(D, NULL, NULL, 0));
    }
    Cp = C->p;
    nc = cc[3] - cc[2]; /* delete cols C0, C1, and C3 from C */
    if (cc[2] > 0)
        for (j = cc[2]; j <= cc[3]; j++) {
            Cp[j - cc[2]] = Cp[j];
        }
    C->n = nc;
    if (rr[2] - rr[1] < m) { /* delete rows R0, R1, and R3 from C */
        cs_fkeep(C, cs_rprune, rr);
        cnz = Cp[nc];
        Ci = C->i;
        if (rr[1] > 0)
            for (k = 0; k < cnz; k++) {
                Ci[k] -= rr[1];
            }
    }
    C->m = nc;
    scc = cs_scc(C); /* find strongly connected components of C*/
    if (!scc) {
        return (cs_ddone(D, C, NULL, 0));
    }
    /* --- Combine coarse and fine decompositions --------------------------- */
    ps = scc->p;   /* C(ps,ps) is the permuted matrix */
    rs = scc->r;   /* kth block is rs[k]..rs[k+1]-1 */
    nb1 = scc->nb; /* # of blocks of A(R2,C2) */
    for (k = 0; k < nc; k++) {
        wj[k] = q[ps[k] + cc[2]];
    }
    for (k = 0; k < nc; k++) {
        q[k + cc[2]] = wj[k];
    }
    for (k = 0; k < nc; k++) {
        wi[k] = p[ps[k] + rr[1]];
    }
    for (k = 0; k < nc; k++) {
        p[k + rr[1]] = wi[k];
    }
    nb2 = 0; /* create the fine block partitions */
    r[0] = s[0] = 0;
    if (cc[2] > 0) {
        nb2++;
    }                           /* leading coarse block A (R1, [C0 C1]) */
    for (k = 0; k < nb1; k++) { /* coarse block A (R2,C2) */
        r[nb2] = rs[k] + rr[1]; /* A (R2,C2) splits into nb1 fine blocks */
        s[nb2] = rs[k] + cc[2];
        nb2++;
    }
    if (rr[2] < m) {
        r[nb2] = rr[2]; /* trailing coarse block A ([R3 R0], C3) */
        s[nb2] = cc[3];
        nb2++;
    }
    r[nb2] = m;
    s[nb2] = n;
    D->nb = nb2;
    cs_dfree(scc);
    return (cs_ddone(D, C, NULL, 1));
}

/* cs_droptol.c */

#include "cs.h"
static CS_INT cs_tol(CS_INT i, CS_INT j, CS_ENTRY aij, void* tol) {
    (void) i;
    (void) j;
    return (CS_ABS(aij) > *((double*)tol));
}
CS_INT cs_droptol(cs* A, double tol) {
    return (cs_fkeep(A, &cs_tol, &tol)); /* keep all large entries */
}

/* cs_dropzeros.c */

#include "cs.h"
static CS_INT cs_nonzero(CS_INT i, CS_INT j, CS_ENTRY aij, void* other) {
    (void)i;
    (void)j;
    (void)other;
    return cs_equalcc(aij, cs_createFromDouble(0)) ? 0 : 1;
}
CS_INT cs_dropzeros(cs* A) {
    return (cs_fkeep(A, &cs_nonzero, NULL)); /* keep all nonzero entries */
}

/* cs_dupl.c */

#include "cs.h"
/* remove duplicate entries from A */
CS_INT cs_dupl(cs* A) {
    CS_INT i, j, p, q, nz = 0, n, m, *Ap, *Ai, *w;
    CS_ENTRY* Ax;
    if (!CS_CSC(A)) {
        return (0);
    } /* check inputs */
    m = A->m;
    n = A->n;
    Ap = A->p;
    Ai = A->i;
    Ax = A->x;
    w = (CS_INT*)cs_malloc(m, sizeof(CS_INT)); /* get workspace */
    if (!w) {
        return (0);
    } /* out of memory */
    for (i = 0; i < m; i++) {
        w[i] = -1;
    } /* row i not yet seen */
    for (j = 0; j < n; j++) {
        q = nz; /* column j will start at q */
        for (p = Ap[j]; p < Ap[j + 1]; p++) {
            i = Ai[p]; /* A(i,j) is nonzero */
            if (w[i] >= q) {
                Ax[w[i]] = cs_pluscc(Ax[w[i]], Ax[p]); /* A(i,j) is a duplicate */
            } else {
                w[i] = nz;  /* record where row i occurs */
                Ai[nz] = i; /* keep A(i,j) */
                Ax[nz++] = Ax[p];
            }
        }
        Ap[j] = q; /* record start of column j */
    }
    Ap[n] = nz;                  /* finalize A */
    cs_free(w);                  /* free workspace */
    return (cs_sprealloc(A, 0)); /* remove extra space from A */
}

/* cs_entry.c */

#include "cs.h"
/* add an entry to a triplet matrix; return 1 if ok, 0 otherwise */
CS_INT cs_entry(cs* T, CS_INT i, CS_INT j, CS_ENTRY x) {
    if (!CS_TRIPLET(T) || i < 0 || j < 0) {
        return (0);
    } /* check inputs */
    if (T->nz >= T->nzmax && !cs_sprealloc(T, 2 * (T->nzmax))) {
        return (0);
    }
    if (T->x) {
        T->x[T->nz] = x;
    }
    T->i[T->nz] = i;
    T->p[T->nz++] = j;
    T->m = CS_MAX(T->m, i + 1);
    T->n = CS_MAX(T->n, j + 1);
    return (1);
}

/* cs_etree.c */

#include "cs.h"
/* compute the etree of A (using triu(A), or A'A without forming A'A */
CS_INT* cs_etree(const cs* A, CS_INT ata) {
    CS_INT i, k, p, m, n, inext, *Ap, *Ai, *w, *parent, *ancestor, *prev;
    if (!CS_CSC(A)) {
        return (NULL);
    } /* check inputs */
    m = A->m;
    n = A->n;
    Ap = A->p;
    Ai = A->i;
    parent = (CS_INT*)cs_malloc(n, sizeof(CS_INT));            /* allocate result */
    w = (CS_INT*)cs_malloc(n + (ata ? m : 0), sizeof(CS_INT)); /* get workspace */
    if (!w || !parent) {
        return (cs_idone(parent, NULL, w, 0));
    }
    ancestor = w;
    prev = w + n;
    if (ata)
        for (i = 0; i < m; i++) {
            prev[i] = -1;
        }
    for (k = 0; k < n; k++) {
        parent[k] = -1;   /* node k has no parent yet */
        ancestor[k] = -1; /* nor does k have an ancestor */
        for (p = Ap[k]; p < Ap[k + 1]; p++) {
            i = ata ? (prev[Ai[p]]) : (Ai[p]);
            for (; i != -1 && i < k; i = inext) { /* traverse from i to k */
                inext = ancestor[i];              /* inext = ancestor of i */
                ancestor[i] = k;                  /* path compression */
                if (inext == -1) {
                    parent[i] = k;
                } /* no anc., parent is k */
            }
            if (ata) {
                prev[Ai[p]] = k;
            }
        }
    }
    return (cs_idone(parent, NULL, w, 1));
}

/* cs_fkeep.c */

#include "cs.h"
/* drop entries for which fkeep(A(i,j)) is false; return nz if OK, else -1 */
CS_INT cs_fkeep(cs* A, CS_INT (*fkeep)(CS_INT, CS_INT, CS_ENTRY, void*), void* other) {
    CS_INT j, p, nz = 0, n, *Ap, *Ai;
    CS_ENTRY* Ax;
    if (!CS_CSC(A) || !fkeep) {
        return (-1);
    } /* check inputs */
    n = A->n;
    Ap = A->p;
    Ai = A->i;
    Ax = A->x;
    for (j = 0; j < n; j++) {
        p = Ap[j];  /* get current location of col j */
        Ap[j] = nz; /* record new location of col j */
        for (; p < Ap[j + 1]; p++) {
            if (fkeep(Ai[p], j, Ax ? Ax[p] : cs_createFromDouble(1), other)) {
                if (Ax) {
                    Ax[nz] = Ax[p];
                } /* keep A(i,j) */
                Ai[nz++] = Ai[p];
            }
        }
    }
    Ap[n] = nz;         /* finalize A */
    cs_sprealloc(A, 0); /* remove extra space from A */
    return (nz);
}

/* cs_gaxpy.c */

#include "cs.h"
/* y = A*x+y */
CS_INT cs_gaxpy(const cs* A, const CS_ENTRY* x, CS_ENTRY* y) {
    CS_INT p, j, n, *Ap, *Ai;
    CS_ENTRY* Ax;
    if (!CS_CSC(A) || !x || !y) {
        return (0);
    } /* check inputs */
    n = A->n;
    Ap = A->p;
    Ai = A->i;
    Ax = A->x;
    for (j = 0; j < n; j++) {
        for (p = Ap[j]; p < Ap[j + 1]; p++) {
            y[Ai[p]] = cs_pluscc(y[Ai[p]], cs_mulcc(Ax[p], x[j]));
        }
    }
    return (1);
}

/* cs_happly.c */

#include "cs.h"
/* apply the ith Householder vector to x */
CS_INT cs_happly(const cs* V, CS_INT i, double beta, CS_ENTRY* x) {
    CS_INT p, *Vp, *Vi;
    CS_ENTRY *Vx, tau = cs_createFromDouble(0);
    if (!CS_CSC(V) || !x) {
        return (0);
    } /* check inputs */
    Vp = V->p;
    Vi = V->i;
    Vx = V->x;
    for (p = Vp[i]; p < Vp[i + 1]; p++) { /* tau = v'*x */
        tau = cs_pluscc(tau, cs_mulcc(CS_CONJ(Vx[p]), x[Vi[p]]));
    }
    tau = cs_mulcc(tau, cs_createFromDouble(beta));                          /* tau = beta*(v'*x) */
    for (p = Vp[i]; p < Vp[i + 1]; p++) { /* x = x - v*tau */
        x[Vi[p]] = cs_subcc(x[Vi[p]], cs_mulcc(Vx[p], tau));
    }
    return (1);
}

/* cs_house.c */

#include "cs.h"
/* create a Householder reflection [v,beta,s]=house(x), overwrite x with v,
 * where (I-beta*v*v')*x = s*e1 and e1 = [1 0 ... 0]'.
 * Note that this CXSparse version is different than CSparse.  See Higham,
 * Accuracy & Stability of Num Algorithms, 2nd ed, 2002, page 357. */
CS_ENTRY cs_house(CS_ENTRY* x, double* beta, CS_INT n) {
    CS_ENTRY s = cs_createFromDouble(0);
    CS_INT i;
    if (!x || !beta) {
        return cs_createFromDouble(-1);
    } /* check inputs */
    /* s = norm(x) */
    for (i = 0; i < n; i++) {
        s = cs_pluscc(s, cs_mulcc(x[i], CS_CONJ(x[i])));
    }
    s = cs_sqrt(s);
    if (cs_equalcc(s, cs_createFromDouble(0))) {
        (*beta) = 0;
        x[0] = cs_createFromDouble(1);
    } else {
        /* s = sign(x[0]) * norm (x) ; */
        if (cs_equalcc(x[0], cs_createFromDouble(0)) == 0) {
            s = cs_mulcc(s, cs_divcc(x[0], cs_createFromDouble(CS_ABS(x[0]))));
        }
        x[0] = cs_pluscc(x[0], s);
        (*beta) = 1. / CS_REAL(cs_mulcc(CS_CONJ(s), x[0]));
    }
    return cs_flipSign(s);
}

/* cs_ipvec.c */

#include "cs.h"
/* x(p) = b, for dense vectors x and b; p=NULL denotes identity */
CS_INT cs_ipvec(const CS_INT* p, const CS_ENTRY* b, CS_ENTRY* x, CS_INT n) {
    CS_INT k;
    if (!x || !b) {
        return (0);
    } /* check inputs */
    for (k = 0; k < n; k++) {
        x[p ? p[k] : k] = b[k];
    }
    return (1);
}

/* cs_load.c */

#include "cs.h"
/* load a triplet matrix from a file */
cs* cs_load(FILE* f) {
    double i, j; /* use double for integers to avoid csi conflicts */
    double x;
#ifdef CS_COMPLEX
    double xi;
#endif
    cs* T;
    if (!f) {
        return (NULL);
    }                              /* check inputs */
    T = cs_spalloc(0, 0, 1, 1, 1); /* allocate result */
#ifdef CS_COMPLEX
    while (fscanf(f, "%lg %lg %lg %lg\n", &i, &j, &x, &xi) == 4)
#else
    while (fscanf(f, "%lg %lg %lg\n", &i, &j, &x) == 3)
#endif
    {
#ifdef CS_COMPLEX
#if  defined(__cplusplus) || (defined(_MSC_VER) && !defined(__ICL))
        if (!cs_entry(T, (CS_INT)i, (CS_INT)j, CS_ENTRY(x,  xi))) {
#else
        if (!cs_entry(T, (CS_INT)i, (CS_INT)j, x + xi * I)) {
#endif
#else
        if (!cs_entry(T, (CS_INT)i, (CS_INT)j, x)) {
#endif
            return (cs_spfree(T));
        }
    }
    return (T);
}

/* cs_lsolve.c */

#include "cs.h"
/* solve Lx=b where x and b are dense.  x=b on input, solution on output. */
CS_INT cs_lsolve(const cs* L, CS_ENTRY* x) {
    CS_INT p, j, n, *Lp, *Li;
    CS_ENTRY* Lx;
    if (!CS_CSC(L) || !x) {
        return (0);
    } /* check inputs */
    n = L->n;
    Lp = L->p;
    Li = L->i;
    Lx = L->x;
    for (j = 0; j < n; j++) {
        x[j] = cs_divcc(x[j], Lx[Lp[j]]);
        for (p = Lp[j] + 1; p < Lp[j + 1]; p++) {
            x[Li[p]] = cs_subcc(x[Li[p]], cs_mulcc(Lx[p], x[j]));
        }
    }
    return (1);
}

/* cs_ltsolve.c */

#include "cs.h"
/* solve L'x=b where x and b are dense.  x=b on input, solution on output. */
CS_INT cs_ltsolve(const cs* L, CS_ENTRY* x) {
    CS_INT p, j, n, *Lp, *Li;
    CS_ENTRY* Lx;
    if (!CS_CSC(L) || !x) {
        return (0);
    } /* check inputs */
    n = L->n;
    Lp = L->p;
    Li = L->i;
    Lx = L->x;
    for (j = n - 1; j >= 0; j--) {
        for (p = Lp[j] + 1; p < Lp[j + 1]; p++) {
            x[j] = cs_subcc(x[j], cs_mulcc(CS_CONJ(Lx[p]), x[Li[p]]));
        }
        x[j] = cs_divcc(x[j], CS_CONJ(Lx[Lp[j]]));
    }
    return (1);
}

/* cs_lu.c */

#include "cs.h"
/* [L,U,pinv]=lu(A, [q lnz unz]). lnz and unz can be guess */
csn* cs_lu(const cs* A, const css* S, double tol) {
    cs *L, *U;
    csn* N;
    CS_ENTRY pivot, *Lx, *Ux, *x;
    double a, t;
    CS_INT *Lp, *Li, *Up, *Ui, *pinv, *xi, *q, n, ipiv, k, top, p, i, col, lnz, unz;
    if (!CS_CSC(A) || !S) {
        return (NULL);
    } /* check inputs */
    n = A->n;
    q = S->q;
    lnz = (CS_INT)S->lnz;
    unz = (CS_INT)S->unz;
    x = (CS_ENTRY*)cs_malloc(n, sizeof(CS_ENTRY));    /* get CS_ENTRY workspace */
    xi = (CS_INT*)cs_malloc(2 * n, sizeof(CS_INT)); /* get CS_INT workspace */
    N = (csn*)cs_calloc(1, sizeof(csn));         /* allocate result */
    if (!x || !xi || !N) {
        return (cs_ndone(N, NULL, xi, x, 0));
    }
    N->L = L = cs_spalloc(n, n, lnz, 1, 0);        /* allocate result L */
    N->U = U = cs_spalloc(n, n, unz, 1, 0);        /* allocate result U */
    N->pinv = pinv = (CS_INT*)cs_malloc(n, sizeof(CS_INT)); /* allocate result pinv */
    if (!L || !U || !pinv) {
        return (cs_ndone(N, NULL, xi, x, 0));
    }
    Lp = L->p;
    Up = U->p;
    for (i = 0; i < n; i++) {
        x[i] = cs_createFromDouble(0);
    } /* clear workspace */
    for (i = 0; i < n; i++) {
        pinv[i] = -1;
    } /* no rows pivotal yet */
    for (k = 0; k <= n; k++) {
        Lp[k] = 0;
    } /* no cols of L yet */
    lnz = unz = 0;
    for (k = 0; k < n; k++) { /* compute L(:,k) and U(:,k) */
        /* --- Triangular solve --------------------------------------------- */
        Lp[k] = lnz; /* L(:,k) starts here */
        Up[k] = unz; /* U(:,k) starts here */
        if ((lnz + n > L->nzmax && !cs_sprealloc(L, 2 * L->nzmax + n)) ||
            (unz + n > U->nzmax && !cs_sprealloc(U, 2 * U->nzmax + n))) {
            return (cs_ndone(N, NULL, xi, x, 0));
        }
        Li = L->i;
        Lx = L->x;
        Ui = U->i;
        Ux = U->x;
        col = q ? (q[k]) : k;
        top = cs_spsolve(L, A, col, xi, x, pinv, 1); /* x = L\A(:,col) */
        /* --- Find pivot --------------------------------------------------- */
        ipiv = -1;
        a = -1;
        for (p = top; p < n; p++) {
            i = xi[p];         /* x(i) is nonzero */
            if (pinv[i] < 0) { /* row i is not yet pivotal */
                if ((t = CS_ABS(x[i])) > a) {
                    a = t; /* largest pivot candidate so far */
                    ipiv = i;
                }
            } else { /* x(i) is the entry U(pinv[i],k) */
                Ui[unz] = pinv[i];
                Ux[unz++] = x[i];
            }
        }
        if (ipiv == -1 || a <= 0) {
            return (cs_ndone(N, NULL, xi, x, 0));
        }
        /* tol=1 for  partial pivoting; tol<1 gives preference to diagonal */
        if (pinv[col] < 0 && CS_ABS(x[col]) >= a * tol) {
            ipiv = col;
        }
        /* --- Divide by pivot ---------------------------------------------- */
        pivot = x[ipiv]; /* the chosen pivot */
        Ui[unz] = k;     /* last entry in U(:,k) is U(k,k) */
        Ux[unz++] = pivot;
        pinv[ipiv] = k; /* ipiv is the kth pivot row */
        Li[lnz] = ipiv; /* first entry in L(:,k) is L(k,k) = 1 */
        Lx[lnz++] = cs_createFromDouble(1);
        for (p = top; p < n; p++) { /* L(k+1:n,k) = x / pivot */
            i = xi[p];
            if (pinv[i] < 0) {            /* x(i) is an entry in L(:,k) */
                Li[lnz] = i;              /* save unpermuted row in L */
                Lx[lnz++] = cs_divcc(x[i], pivot); /* scale pivot column */
            }
            x[i] = cs_createFromDouble(0); /* x [0..n-1] = 0 for next k */
        }
    }
    /* --- Finalize L and U ------------------------------------------------- */
    Lp[n] = lnz;
    Up[n] = unz;
    Li = L->i; /* fix row indices of L for final pinv */
    for (p = 0; p < lnz; p++) {
        Li[p] = pinv[Li[p]];
    }
    cs_sprealloc(L, 0); /* remove extra space from L and U */
    cs_sprealloc(U, 0);
    return (cs_ndone(N, NULL, xi, x, 1)); /* success */
}

/* cs_lusol.c */

#include "cs.h"
/* x=A\b where A is unsymmetric; b overwritten with solution */
CS_INT cs_lusol(CS_INT order, const cs* A, CS_ENTRY* b, double tol) {
    CS_ENTRY* x;
    css* S;
    csn* N;
    CS_INT n, ok;
    if (!CS_CSC(A) || !b) {
        return (0);
    } /* check inputs */
    n = A->n;
    S = cs_sqr(order, A, 0);            /* ordering and symbolic analysis */
    N = cs_lu(A, S, tol);               /* numeric LU factorization */
    x = (CS_ENTRY*)cs_malloc(n, sizeof(CS_ENTRY)); /* get workspace */
    ok = (S && N && x);
    if (ok) {
        cs_ipvec(N->pinv, b, x, n); /* x = b(p) */
        cs_lsolve(N->L, x);         /* x = L\x */
        cs_usolve(N->U, x);         /* x = U\x */
        cs_ipvec(S->q, x, b, n);    /* b(q) = x */
    }
    cs_free(x);
    cs_sfree(S);
    cs_nfree(N);
    return (ok);
}

/* cs_malloc.c */

#include "cs.h"
#ifdef MATLAB_MEX_FILE
#define malloc mxMalloc
#define free mxFree
#define realloc mxRealloc
#define calloc mxCalloc
#endif

/* wrapper for malloc */
void* cs_malloc(CS_INT n, size_t size) {
    return (malloc(CS_MAX(n, 1) * size));
}

/* wrapper for calloc */
void* cs_calloc(CS_INT n, size_t size) {
    return (calloc(CS_MAX(n, 1), size));
}

/* wrapper for free */
void* cs_free(void* p) {
    if (p) {
        free(p);
    }              /* free p if it is not already NULL */
    return (NULL); /* return NULL to simplify the use of cs_free */
}

/* wrapper for realloc */
void* cs_realloc(void* p, CS_INT n, size_t size, CS_INT* ok) {
    void* pnew;
    pnew = realloc(p, CS_MAX(n, 1) * size); /* realloc the block */
    *ok = (pnew != NULL);                   /* realloc fails if pnew is NULL */
    return ((*ok) ? pnew : p);              /* return original p if failure */
}

/* cs_maxtrans.c */

#include "cs.h"
/* find an augmenting path starting at column k and extend the match if found */
static void cs_augment(CS_INT k,
                       const cs* A,
                       CS_INT* jmatch,
                       CS_INT* cheap,
                       CS_INT* w,
                       CS_INT* js,
                       CS_INT* is,
                       CS_INT* ps) {
    CS_INT found = 0, p, i = -1, *Ap = A->p, *Ai = A->i, head = 0, j;
    js[0] = k; /* start with just node k in jstack */
    while (head >= 0) {
        /* --- Start (or continue) depth-first-search at node j ------------- */
        j = js[head];    /* get j from top of jstack */
        if (w[j] != k) { /* 1st time j visited for kth path */
            w[j] = k;    /* mark j as visited for kth path */
            for (p = cheap[j]; p < Ap[j + 1] && !found; p++) {
                i = Ai[p]; /* try a cheap assignment (i,j) */
                found = (jmatch[i] == -1);
            }
            cheap[j] = p; /* start here next time j is traversed*/
            if (found) {
                is[head] = i; /* column j matched with row i */
                break;        /* end of augmenting path */
            }
            ps[head] = Ap[j]; /* no cheap match: start dfs for j */
        }
        /* --- Depth-first-search of neighbors of j ------------------------- */
        for (p = ps[head]; p < Ap[j + 1]; p++) {
            i = Ai[p]; /* consider row i */
            if (w[jmatch[i]] == k) {
                continue;
            }                       /* skip jmatch [i] if marked */
            ps[head] = p + 1;       /* pause dfs of node j */
            is[head] = i;           /* i will be matched with j if found */
            js[++head] = jmatch[i]; /* start dfs at column jmatch [i] */
            break;
        }
        if (p == Ap[j + 1]) {
            head--;
        } /* node j is done; pop from stack */
    }     /* augment the match if path found: */
    if (found)
        for (p = head; p >= 0; p--) {
            jmatch[is[p]] = js[p];
        }
}

/* find a maximum transveral */
CS_INT* cs_maxtrans(const cs* A, CS_INT seed) { /*[jmatch [0..m-1]; imatch [0..n-1]]*/
    CS_INT i, j, k, n, m, p, n2 = 0, m2 = 0, *Ap, *jimatch, *w, *cheap, *js, *is, *ps, *Ai, *Cp,
                             *jmatch, *imatch, *q;
    cs* C;
    if (!CS_CSC(A)) {
        return (NULL);
    } /* check inputs */
    n = A->n;
    m = A->m;
    Ap = A->p;
    Ai = A->i;
    w = jimatch = (CS_INT*)cs_calloc(m + n, sizeof(CS_INT)); /* allocate result */
    if (!jimatch) {
        return (NULL);
    }
    for (k = 0, j = 0; j < n; j++) { /* count nonempty rows and columns */
        n2 += (Ap[j] < Ap[j + 1]);
        for (p = Ap[j]; p < Ap[j + 1]; p++) {
            w[Ai[p]] = 1;
            k += (j == Ai[p]); /* count entries already on diagonal */
        }
    }
    if (k == CS_MIN(m, n)) { /* quick return if diagonal zero-free */
        jmatch = jimatch;
        imatch = jimatch + m;
        for (i = 0; i < k; i++) {
            jmatch[i] = i;
        }
        for (; i < m; i++) {
            jmatch[i] = -1;
        }
        for (j = 0; j < k; j++) {
            imatch[j] = j;
        }
        for (; j < n; j++) {
            imatch[j] = -1;
        }
        return (cs_idone(jimatch, NULL, NULL, 1));
    }
    for (i = 0; i < m; i++) {
        m2 += w[i];
    }
    C = (m2 < n2) ? cs_transpose(A, 0) : ((cs*)A); /* transpose if needed */
    if (!C) {
        return (cs_idone(jimatch, (m2 < n2) ? C : NULL, NULL, 0));
    }
    n = C->n;
    m = C->m;
    Cp = C->p;
    jmatch = (m2 < n2) ? jimatch + n : jimatch;
    imatch = (m2 < n2) ? jimatch : jimatch + m;
    w = (CS_INT*)cs_malloc(5 * n, sizeof(CS_INT)); /* get workspace */
    if (!w) {
        return (cs_idone(jimatch, (m2 < n2) ? C : NULL, w, 0));
    }
    cheap = w + n;
    js = w + 2 * n;
    is = w + 3 * n;
    ps = w + 4 * n;
    for (j = 0; j < n; j++) {
        cheap[j] = Cp[j];
    } /* for cheap assignment */
    for (j = 0; j < n; j++) {
        w[j] = -1;
    } /* all columns unflagged */
    for (i = 0; i < m; i++) {
        jmatch[i] = -1;
    }                         /* nothing matched yet */
    q = cs_randperm(n, seed); /* q = random permutation */
    for (k = 0; k < n; k++) { /* augment, starting at column q[k] */
        cs_augment(q ? q[k] : k, C, jmatch, cheap, w, js, is, ps);
    }
    cs_free(q);
    for (j = 0; j < n; j++) {
        imatch[j] = -1;
    } /* find row match */
    for (i = 0; i < m; i++)
        if (jmatch[i] >= 0) {
            imatch[jmatch[i]] = i;
        }
    return (cs_idone(jimatch, (m2 < n2) ? C : NULL, w, 1));
}

/* cs_multiply.c */

#include "cs.h"
/* C = A*B */
cs* cs_multiply(const cs* A, const cs* B) {
    CS_INT p, j, nz = 0, anz, *Cp, *Ci, *Bp, m, n, bnz, *w, values, *Bi;
    CS_ENTRY *x, *Bx, *Cx;
    cs* C;
    if (!CS_CSC(A) || !CS_CSC(B)) {
        return (NULL);
    } /* check inputs */
    if (A->n != B->m) {
        return (NULL);
    }
    m = A->m;
    anz = A->p[A->n];
    n = B->n;
    Bp = B->p;
    Bi = B->i;
    Bx = B->x;
    bnz = Bp[n];
    w = (CS_INT*)cs_calloc(m, sizeof(CS_INT)); /* get workspace */
    values = (A->x != NULL) && (Bx != NULL);
    x = values ? (CS_ENTRY*)cs_malloc(m, sizeof(CS_ENTRY)) : NULL; /* get workspace */
    C = cs_spalloc(m, n, anz + bnz, values, 0);         /* allocate result */
    if (!C || !w || (values && !x)) {
        return (cs_done(C, w, x, 0));
    }
    Cp = C->p;
    for (j = 0; j < n; j++) {
        if (nz + m > C->nzmax && !cs_sprealloc(C, 2 * (C->nzmax) + m)) {
            return (cs_done(C, w, x, 0)); /* out of memory */
        }
        Ci = C->i;
        Cx = C->x;  /* C->i and C->x may be reallocated */
        Cp[j] = nz; /* column j of C starts here */
        for (p = Bp[j]; p < Bp[j + 1]; p++) {
            nz = cs_scatter(A, Bi[p], Bx ? Bx[p] : cs_createFromDouble(1), w, x, j + 1, C, nz);
        }
        if (values)
            for (p = Cp[j]; p < nz; p++) {
                Cx[p] = x[Ci[p]];
            }
    }
    Cp[n] = nz;                   /* finalize the last column of C */
    cs_sprealloc(C, 0);           /* remove extra space from C */
    return (cs_done(C, w, x, 1)); /* success; free workspace, return C */
}

/* cs_norm.c */

#include "cs.h"
/* 1-norm of a sparse matrix = max (sum (abs (A))), largest column sum */
double cs_norm(const cs* A) {
    CS_INT p, j, n, *Ap;
    CS_ENTRY* Ax;
    double norm = 0, s;
    if (!CS_CSC(A) || !A->x) {
        return (-1);
    } /* check inputs */
    n = A->n;
    Ap = A->p;
    Ax = A->x;
    for (j = 0; j < n; j++) {
        for (s = 0, p = Ap[j]; p < Ap[j + 1]; p++) {
            s += CS_ABS(Ax[p]);
        }
        norm = CS_MAX(norm, s);
    }
    return (norm);
}

/* cs_permute.c */

#include "cs.h"
/* C = A(p,q) where p and q are permutations of 0..m-1 and 0..n-1. */
cs* cs_permute(const cs* A, const CS_INT* pinv, const CS_INT* q, CS_INT values) {
    CS_INT t, j, k, nz = 0, m, n, *Ap, *Ai, *Cp, *Ci;
    CS_ENTRY *Cx, *Ax;
    cs* C;
    if (!CS_CSC(A)) {
        return (NULL);
    } /* check inputs */
    m = A->m;
    n = A->n;
    Ap = A->p;
    Ai = A->i;
    Ax = A->x;
    C = cs_spalloc(m, n, Ap[n], values && Ax != NULL, 0); /* alloc result */
    if (!C) {
        return (cs_done(C, NULL, NULL, 0));
    } /* out of memory */
    Cp = C->p;
    Ci = C->i;
    Cx = C->x;
    for (k = 0; k < n; k++) {
        Cp[k] = nz; /* column k of C is column q[k] of A */
        j = q ? (q[k]) : k;
        for (t = Ap[j]; t < Ap[j + 1]; t++) {
            if (Cx) {
                Cx[nz] = Ax[t];
            } /* row i of A is row pinv[i] of C */
            Ci[nz++] = pinv ? (pinv[Ai[t]]) : Ai[t];
        }
    }
    Cp[n] = nz; /* finalize the last column of C */
    return (cs_done(C, NULL, NULL, 1));
}

/* cs_pinv.c */

#include "cs.h"
/* pinv = p', or p = pinv' */
CS_INT* cs_pinv(CS_INT const* p, CS_INT n) {
    CS_INT k, *pinv;
    if (!p) {
        return (NULL);
    }                                    /* p = NULL denotes identity */
    pinv = (CS_INT*)cs_malloc(n, sizeof(CS_INT)); /* allocate result */
    if (!pinv) {
        return (NULL);
    } /* out of memory */
    for (k = 0; k < n; k++) {
        pinv[p[k]] = k;
    }              /* invert the permutation */
    return (pinv); /* return result */
}

/* cs_post.c */

#include "cs.h"
/* post order a forest */
CS_INT* cs_post(const CS_INT* parent, CS_INT n) {
    CS_INT j, k = 0, *post, *w, *head, *next, *stack;
    if (!parent) {
        return (NULL);
    }                                     /* check inputs */
    post = (CS_INT*)cs_malloc(n, sizeof(CS_INT));  /* allocate result */
    w = (CS_INT*)cs_malloc(3 * n, sizeof(CS_INT)); /* get workspace */
    if (!w || !post) {
        return (cs_idone(post, NULL, w, 0));
    }
    head = w;
    next = w + n;
    stack = w + 2 * n;
    for (j = 0; j < n; j++) {
        head[j] = -1;
    }                              /* empty linked lists */
    for (j = n - 1; j >= 0; j--) { /* traverse nodes in reverse order*/
        if (parent[j] == -1) {
            continue;
        }                          /* j is a root */
        next[j] = head[parent[j]]; /* add j to list of its parent */
        head[parent[j]] = j;
    }
    for (j = 0; j < n; j++) {
        if (parent[j] != -1) {
            continue;
        } /* skip j if it is not a root */
        k = cs_tdfs(j, k, head, next, post, stack);
    }
    return (cs_idone(post, NULL, w, 1)); /* success; free w, return post */
}

/* cs_print.c */

#include "cs.h"
/* print a sparse matrix; use %g for integers to avoid differences with CS_INT */
CS_INT cs_print(const cs* A, CS_INT brief) {
    CS_INT p, j, m, n, nzmax, nz, *Ap, *Ai;
    CS_ENTRY* Ax;
    if (!A) {
        printf("(null)\n");
        return (0);
    }
    m = A->m;
    n = A->n;
    Ap = A->p;
    Ai = A->i;
    Ax = A->x;
    nzmax = A->nzmax;
    nz = A->nz;
    printf("CXSparse Version %d.%d.%d, %s.  %s\n", CS_VER, CS_SUBVER, CS_SUBSUB, CS_DATE,
           CS_COPYRIGHT);
    if (nz < 0) {
        printf("%g-by-%g, nzmax: %g nnz: %g, 1-norm: %g\n", (double)m, (double)n, (double)nzmax,
               (double)(Ap[n]), cs_norm(A));
        for (j = 0; j < n; j++) {
            printf("    col %g : locations %g to %g\n", (double)j, (double)(Ap[j]),
                   (double)(Ap[j + 1] - 1));
            for (p = Ap[j]; p < Ap[j + 1]; p++) {
                printf("      %g : ", (double)(Ai[p]));
#ifdef CS_COMPLEX
                printf("(%g, %g)\n", Ax ? CS_REAL(Ax[p]) : 1, Ax ? CS_IMAG(Ax[p]) : 0);
#else
                printf("%g\n", Ax ? Ax[p] : 1);
#endif
                if (brief && p > 20) {
                    printf("  ...\n");
                    return (1);
                }
            }
        }
    } else {
        printf("triplet: %g-by-%g, nzmax: %g nnz: %g\n", (double)m, (double)n, (double)nzmax,
               (double)nz);
        for (p = 0; p < nz; p++) {

            printf("    %g %g : ", (double)(Ai[p]), (double)(Ap[p]));
#ifdef CS_COMPLEX
            printf("(%g, %g)\n", Ax ? CS_REAL(Ax[p]) : 1, Ax ? CS_IMAG(Ax[p]) : 0);
#else
            printf("%g\n", Ax ? Ax[p] : 1);
#endif
            if (brief && p > 20) {
                printf("  ...\n");
                return (1);
            }
        }
    }
    return (1);
}

/* cs_pvec.c */

#include "cs.h"
/* x = b(p), for dense vectors x and b; p=NULL denotes identity */
CS_INT cs_pvec(const CS_INT* p, const CS_ENTRY* b, CS_ENTRY* x, CS_INT n) {
    CS_INT k;
    if (!x || !b) {
        return (0);
    } /* check inputs */
    for (k = 0; k < n; k++) {
        x[k] = b[p ? p[k] : k];
    }
    return (1);
}

/* cs_qr.c */

#include "cs.h"
/* sparse QR factorization [V,beta,pinv,R] = qr (A) */
csn* cs_qr(const cs* A, const css* S) {
    CS_ENTRY *Rx, *Vx, *Ax, *x;
    double* Beta;
    CS_INT i, k, p, n, vnz, p1, top, m2, len, col, rnz, *s, *leftmost, *Ap, *Ai, *parent, *Rp,
        *Ri, *Vp, *Vi, *w, *pinv, *q;
    cs *R, *V;
    csn* N;
    if (!CS_CSC(A) || !S) {
        return (NULL);
    }
    n = A->n;
    Ap = A->p;
    Ai = A->i;
    Ax = A->x;
    q = S->q;
    parent = S->parent;
    pinv = S->pinv;
    m2 = S->m2;
    vnz = (CS_INT)S->lnz;
    rnz = (CS_INT)S->unz;
    leftmost = (CS_INT*)S->leftmost;
    w = (CS_INT*)cs_malloc(m2 + n, sizeof(CS_INT)); /* get CS_INT workspace */
    x = (CS_ENTRY*)cs_malloc(m2, sizeof(CS_ENTRY));   /* get CS_ENTRY workspace */
    N = (csn*)cs_calloc(1, sizeof(csn));         /* allocate result */
    if (!w || !x || !N) {
        return (cs_ndone(N, NULL, w, x, 0));
    }
    s = w + m2; /* s is size n */
    for (k = 0; k < m2; k++) {
        x[k] = cs_createFromDouble(0);
    }                                           /* clear workspace x */
    N->L = V = cs_spalloc(m2, n, vnz, 1, 0);    /* allocate result V */
    N->U = R = cs_spalloc(m2, n, rnz, 1, 0);    /* allocate result R */
    N->B = Beta = (double*)cs_malloc(n, sizeof(double)); /* allocate result Beta */
    if (!R || !V || !Beta) {
        return (cs_ndone(N, NULL, w, x, 0));
    }
    Rp = R->p;
    Ri = R->i;
    Rx = R->x;
    Vp = V->p;
    Vi = V->i;
    Vx = V->x;
    for (i = 0; i < m2; i++) {
        w[i] = -1;
    } /* clear w, to mark nodes */
    rnz = 0;
    vnz = 0;
    for (k = 0; k < n; k++) { /* compute V and R */
        Rp[k] = rnz;          /* R(:,k) starts here */
        Vp[k] = p1 = vnz;     /* V(:,k) starts here */
        w[k] = k;             /* add V(k,k) to pattern of V */
        Vi[vnz++] = k;
        top = n;
        col = q ? q[k] : k;
        for (p = Ap[col]; p < Ap[col + 1]; p++) {     /* find R(:,k) pattern */
            i = leftmost[Ai[p]];                      /* i = min(find(A(i,q))) */
            for (len = 0; w[i] != k; i = parent[i]) { /* traverse up to k */
                s[len++] = i;
                w[i] = k;
            }
            while (len > 0) {
                s[--top] = s[--len];
            }                        /* push path on stack */
            i = pinv[Ai[p]];         /* i = permuted row of A(:,col) */
            x[i] = Ax[p];            /* x (i) = A(:,col) */
            if (i > k && w[i] < k) { /* pattern of V(:,k) = x (k+1:m) */
                Vi[vnz++] = i;       /* add i to pattern of V(:,k) */
                w[i] = k;
            }
        }
        for (p = top; p < n; p++) {      /* for each i in pattern of R(:,k) */
            i = s[p];                    /* R(i,k) is nonzero */
            cs_happly(V, i, Beta[i], x); /* apply (V(i),Beta(i)) to x */
            Ri[rnz] = i;                 /* R(i,k) = x(i) */
            Rx[rnz++] = x[i];
            x[i] = cs_createFromDouble(0);
            if (parent[i] == k) {
                vnz = cs_scatter(V, i, cs_createFromDouble(0), w, NULL, k, V, vnz);
            }
        }
        for (p = p1; p < vnz; p++) { /* gather V(:,k) = x */
            Vx[p] = x[Vi[p]];
            x[Vi[p]] = cs_createFromDouble(0);
        }
        Ri[rnz] = k;                                       /* R(k,k) = norm (x) */
        Rx[rnz++] = cs_house(Vx + p1, Beta + k, vnz - p1); /* [v,beta]=house(x) */
    }
    Rp[n] = rnz;                         /* finalize R */
    Vp[n] = vnz;                         /* finalize V */
    return (cs_ndone(N, NULL, w, x, 1)); /* success */
}

/* cs_qrsol.c */

#include "cs.h"
/* x=A\b where A can be rectangular; b overwritten with solution */
CS_INT cs_qrsol(CS_INT order, const cs* A, CS_ENTRY* b) {
    CS_ENTRY* x;
    css* S;
    csn* N;
    cs* AT = NULL;
    CS_INT k, m, n, ok;
    if (!CS_CSC(A) || !b) {
        return (0);
    } /* check inputs */
    n = A->n;
    m = A->m;
    if (m >= n) {
        S = cs_sqr(order, A, 1);                        /* ordering and symbolic analysis */
        N = cs_qr(A, S);                                /* numeric QR factorization */
        x = (CS_ENTRY*)cs_calloc(S ? S->m2 : 1, sizeof(CS_ENTRY)); /* get workspace */
        ok = (S && N && x);
        if (ok) {
            cs_ipvec(S->pinv, b, x, m); /* x(0:m-1) = b(p(0:m-1) */
            for (k = 0; k < n; k++) {   /* apply Householder refl. to x */
                cs_happly(N->L, k, N->B[k], x);
            }
            cs_usolve(N->U, x);      /* x = R\x */
            cs_ipvec(S->q, x, b, n); /* b(q(0:n-1)) = x(0:n-1) */
        }
    } else {
        AT = cs_transpose(A, 1);                        /* Ax=b is underdetermined */
        S = cs_sqr(order, AT, 1);                       /* ordering and symbolic analysis */
        N = cs_qr(AT, S);                               /* numeric QR factorization of A' */
        x = (CS_ENTRY*)cs_calloc(S ? S->m2 : 1, sizeof(CS_ENTRY)); /* get workspace */
        ok = (AT && S && N && x);
        if (ok) {
            cs_pvec(S->q, b, x, m);        /* x(q(0:m-1)) = b(0:m-1) */
            cs_utsolve(N->U, x);           /* x = R'\x */
            for (k = m - 1; k >= 0; k--) { /* apply Householder refl. to x */
                cs_happly(N->L, k, N->B[k], x);
            }
            cs_pvec(S->pinv, x, b, n); /* b(0:n-1) = x(p(0:n-1)) */
        }
    }
    cs_free(x);
    cs_sfree(S);
    cs_nfree(N);
    cs_spfree(AT);
    return (ok);
}

/* cs_scatter.c */

#include "cs.h"
/* x = x + beta * A(:,j), where x is a dense vector and A(:,j) is sparse */
CS_INT cs_scatter(const cs* A,
                  CS_INT j,
                  CS_ENTRY beta,
                  CS_INT* w,
                  CS_ENTRY* x,
                  CS_INT mark,
                  cs* C,
                  CS_INT nz) {
    CS_INT i, p, *Ap, *Ai, *Ci;
    CS_ENTRY* Ax;
    if (!CS_CSC(A) || !w || !CS_CSC(C)) {
        return (-1);
    } /* check inputs */
    Ap = A->p;
    Ai = A->i;
    Ax = A->x;
    Ci = C->i;
    for (p = Ap[j]; p < Ap[j + 1]; p++) {
        i = Ai[p]; /* A(i,j) is nonzero */
        if (w[i] < mark) {
            w[i] = mark;  /* i is new entry in column j */
            Ci[nz++] = i; /* add i to pattern of C(:,j) */
            if (x) {
                x[i] = cs_mulcc(beta, Ax[p]);
            } /* x(i) = beta*A(i,j) */
        } else if (x) {
            x[i] = cs_pluscc(x[i], cs_mulcc(beta, Ax[p]));
        } /* i exists in C(:,j) already */
    }
    return (nz);
}

/* cs_scc.c */

#include "cs.h"
/* find the strongly connected components of a square matrix */
csd* cs_scc(cs* A) { /* matrix A temporarily modified, then restored */
    CS_INT n, i, k, b, nb = 0, top, *xi, *pstack, *p, *r, *Ap, *ATp, *rcopy, *Blk;
    cs* AT;
    csd* D;
    if (!CS_CSC(A)) {
        return (NULL);
    } /* check inputs */
    n = A->n;
    Ap = A->p;
    D = cs_dalloc(n, 0);                       /* allocate result */
    AT = cs_transpose(A, 0);                   /* AT = A' */
    xi = (CS_INT*)cs_malloc(2 * n + 1, sizeof(CS_INT)); /* get workspace */
    if (!D || !AT || !xi) {
        return (cs_ddone(D, AT, xi, 0));
    }
    Blk = xi;
    rcopy = pstack = xi + n;
    p = D->p;
    r = D->r;
    ATp = AT->p;
    top = n;
    for (i = 0; i < n; i++) { /* first dfs(A) to find finish times (xi) */
        if (!CS_MARKED(Ap, i)) {
            top = cs_dfs(i, A, top, xi, pstack, NULL);
        }
    }
    for (i = 0; i < n; i++) {
        CS_MARK(Ap, i);
    } /* restore A; unmark all nodes*/
    top = n;
    nb = n;
    for (k = 0; k < n; k++) { /* dfs(A') to find strongly connnected comp */
        i = xi[k];            /* get i in reverse order of finish times */
        if (CS_MARKED(ATp, i)) {
            continue;
        }              /* skip node i if already ordered */
        r[nb--] = top; /* node i is the start of a component in p */
        top = cs_dfs(i, AT, top, p, pstack, NULL);
    }
    r[nb] = 0; /* first block starts at zero; shift r up */
    for (k = nb; k <= n; k++) {
        r[k - nb] = r[k];
    }
    D->nb = nb = n - nb;       /* nb = # of strongly connected components */
    for (b = 0; b < nb; b++) { /* sort each block in natural order */
        for (k = r[b]; k < r[b + 1]; k++) {
            Blk[p[k]] = b;
        }
    }
    for (b = 0; b <= nb; b++) {
        rcopy[b] = r[b];
    }
    for (i = 0; i < n; i++) {
        p[rcopy[Blk[i]]++] = i;
    }
    return (cs_ddone(D, AT, xi, 1));
}

/* cs_schol.c */

#include "cs.h"
/* ordering and symbolic analysis for a Cholesky factorization */
css* cs_schol(CS_INT order, const cs* A) {
    CS_INT n, *c, *post, *P;
    cs* C;
    css* S;
    if (!CS_CSC(A)) {
        return (NULL);
    } /* check inputs */
    n = A->n;
    S = (css*)cs_calloc(1, sizeof(css)); /* allocate result S */
    if (!S) {
        return (NULL);
    }                        /* out of memory */
    P = cs_amd(order, A);    /* P = amd(A+A'), or natural */
    S->pinv = cs_pinv(P, n); /* find inverse permutation */
    cs_free(P);
    if (order && !S->pinv) {
        return (cs_sfree(S));
    }
    C = cs_symperm(A, S->pinv, 0);        /* C = spones(triu(A(P,P))) */
    S->parent = cs_etree(C, 0);           /* find etree of C */
    post = cs_post(S->parent, n);         /* postorder the etree */
    c = cs_counts(C, S->parent, post, 0); /* find column counts of chol(C) */
    cs_free(post);
    cs_spfree(C);
    S->cp = (CS_INT*)cs_malloc(n + 1, sizeof(CS_INT)); /* allocate result S->cp */
    S->unz = S->lnz = cs_cumsum(S->cp, c, n); /* find column pointers for L */
    cs_free(c);
    return ((S->lnz >= 0) ? S : cs_sfree(S));
}

/* cs_sqr.c */

#include "cs.h"
/* compute nnz(V) = S->lnz, S->pinv, S->leftmost, S->m2 from A and S->parent */
static CS_INT cs_vcount(const cs* A, css* S) {
    CS_INT i, k, p, pa, n = A->n, m = A->m, *Ap = A->p, *Ai = A->i, *next, *head, *tail, *nque,
                        *pinv, *leftmost, *w, *parent = S->parent;
    S->pinv = pinv = (CS_INT*)cs_malloc(m + n, sizeof(CS_INT));     /* allocate pinv, */
    S->leftmost = leftmost = (CS_INT*)cs_malloc(m, sizeof(CS_INT)); /* and leftmost */
    w = (CS_INT*)cs_malloc(m + 3 * n, sizeof(CS_INT));              /* get workspace */
    if (!pinv || !w || !leftmost) {
        cs_free(w); /* pinv and leftmost freed later */
        return (0); /* out of memory */
    }
    next = w;
    head = w + m;
    tail = w + m + n;
    nque = w + m + 2 * n;
    for (k = 0; k < n; k++) {
        head[k] = -1;
    } /* queue k is empty */
    for (k = 0; k < n; k++) {
        tail[k] = -1;
    }
    for (k = 0; k < n; k++) {
        nque[k] = 0;
    }
    for (i = 0; i < m; i++) {
        leftmost[i] = -1;
    }
    for (k = n - 1; k >= 0; k--) {
        for (p = Ap[k]; p < Ap[k + 1]; p++) {
            leftmost[Ai[p]] = k; /* leftmost[i] = min(find(A(i,:)))*/
        }
    }
    for (i = m - 1; i >= 0; i--) { /* scan rows in reverse order */
        pinv[i] = -1;              /* row i is not yet ordered */
        k = leftmost[i];
        if (k == -1) {
            continue;
        } /* row i is empty */
        if (nque[k]++ == 0) {
            tail[k] = i;
        }                  /* first row in queue k */
        next[i] = head[k]; /* put i at head of queue k */
        head[k] = i;
    }
    S->lnz = 0;
    S->m2 = m;
    for (k = 0; k < n; k++) { /* find row permutation and nnz(V)*/
        i = head[k];          /* remove row i from queue k */
        S->lnz++;             /* count V(k,k) as nonzero */
        if (i < 0) {
            i = S->m2++;
        }            /* add a fictitious row */
        pinv[i] = k; /* associate row i with V(:,k) */
        if (--nque[k] <= 0) {
            continue;
        }                             /* skip if V(k+1:m,k) is empty */
        S->lnz += nque[k];            /* nque [k] is nnz (V(k+1:m,k)) */
        if ((pa = parent[k]) != -1) { /* move all rows to parent of k */
            if (nque[pa] == 0) {
                tail[pa] = tail[k];
            }
            next[tail[k]] = head[pa];
            head[pa] = next[i];
            nque[pa] += nque[k];
        }
    }
    for (i = 0; i < m; i++)
        if (pinv[i] < 0) {
            pinv[i] = k++;
        }
    cs_free(w);
    return (1);
}

/* symbolic ordering and analysis for QR or LU */
css* cs_sqr(CS_INT order, const cs* A, CS_INT qr) {
    CS_INT n, k, ok = 1, *post;
    css* S;
    if (!CS_CSC(A)) {
        return (NULL);
    } /* check inputs */
    n = A->n;
    S = (css*)cs_calloc(1, sizeof(css)); /* allocate result S */
    if (!S) {
        return (NULL);
    }                        /* out of memory */
    S->q = cs_amd(order, A); /* fill-reducing ordering */
    if (order && !S->q) {
        return (cs_sfree(S));
    }
    if (qr) { /* QR symbolic analysis */
        cs* C = order ? cs_permute(A, NULL, S->q, 0) : ((cs*)A);
        S->parent = cs_etree(C, 1); /* etree of C'*C, where C=A(:,q) */
        post = cs_post(S->parent, n);
        S->cp = cs_counts(C, S->parent, post, 1); /* col counts chol(C'*C) */
        cs_free(post);
        ok = C && S->parent && S->cp && cs_vcount(C, S);
        if (ok)
            for (S->unz = 0, k = 0; k < n; k++) {
                S->unz += S->cp[k];
            }
        if (order) {
            cs_spfree(C);
        }
    } else {
        S->unz = 4 * (A->p[n]) + n; /* for LU factorization only, */
        S->lnz = S->unz;            /* guess nnz(L) and nnz(U) */
    }
    return (ok ? S : cs_sfree(S)); /* return result S */
}

/* cs_symperm.c */

#include "cs.h"
/* C = A(p,p) where A and C are symmetric the upper part stored; pinv not p */
cs* cs_symperm(const cs* A, const CS_INT* pinv, CS_INT values) {
    CS_INT i, j, p, q, i2, j2, n, *Ap, *Ai, *Cp, *Ci, *w;
    CS_ENTRY *Cx, *Ax;
    cs* C;
    if (!CS_CSC(A)) {
        return (NULL);
    } /* check inputs */
    n = A->n;
    Ap = A->p;
    Ai = A->i;
    Ax = A->x;
    C = cs_spalloc(n, n, Ap[n], values && (Ax != NULL), 0); /* alloc result*/
    w = (CS_INT*)cs_calloc(n, sizeof(CS_INT));                       /* get workspace */
    if (!C || !w) {
        return (cs_done(C, w, NULL, 0));
    } /* out of memory */
    Cp = C->p;
    Ci = C->i;
    Cx = C->x;
    for (j = 0; j < n; j++) {    /* count entries in each column of C */
        j2 = pinv ? pinv[j] : j; /* column j of A is column j2 of C */
        for (p = Ap[j]; p < Ap[j + 1]; p++) {
            i = Ai[p];
            if (i > j) {
                continue;
            }                        /* skip lower triangular part of A */
            i2 = pinv ? pinv[i] : i; /* row i of A is row i2 of C */
            w[CS_MAX(i2, j2)]++;     /* column count of C */
        }
    }
    cs_cumsum(Cp, w, n); /* compute column pointers of C */
    for (j = 0; j < n; j++) {
        j2 = pinv ? pinv[j] : j; /* column j of A is column j2 of C */
        for (p = Ap[j]; p < Ap[j + 1]; p++) {
            i = Ai[p];
            if (i > j) {
                continue;
            }                        /* skip lower triangular part of A*/
            i2 = pinv ? pinv[i] : i; /* row i of A is row i2 of C */
            Ci[q = w[CS_MAX(i2, j2)]++] = CS_MIN(i2, j2);
            if (Cx) {
                Cx[q] = (i2 <= j2) ? Ax[p] : CS_CONJ(Ax[p]);
            }
        }
    }
    return (cs_done(C, w, NULL, 1)); /* success; free workspace, return C */
}

/* cs_tdfs.c */

#include "cs.h"
/* depth-first search and postorder of a tree rooted at node j */
CS_INT cs_tdfs(CS_INT j, CS_INT k, CS_INT* head, const CS_INT* next, CS_INT* post, CS_INT* stack) {
    CS_INT i, p, top = 0;
    if (!head || !next || !post || !stack) {
        return (-1);
    }                   /* check inputs */
    stack[0] = j;       /* place j on the stack */
    while (top >= 0) {  /* while (stack is not empty) */
        p = stack[top]; /* p = top of stack */
        i = head[p];    /* i = youngest child of p */
        if (i == -1) {
            top--;         /* p has no unordered children left */
            post[k++] = p; /* node p is the kth postordered node */
        } else {
            head[p] = next[i]; /* remove i from children of p */
            stack[++top] = i;  /* start dfs on child node i */
        }
    }
    return (k);
}

/* cs_transpose.c */

#include "cs.h"
/* C = A' */
cs* cs_transpose(const cs* A, CS_INT values) {
    CS_INT p, q, j, *Cp, *Ci, n, m, *Ap, *Ai, *w;
    CS_ENTRY *Cx, *Ax;
    cs* C;
    if (!CS_CSC(A)) {
        return (NULL);
    } /* check inputs */
    m = A->m;
    n = A->n;
    Ap = A->p;
    Ai = A->i;
    Ax = A->x;
    C = cs_spalloc(n, m, Ap[n], values && Ax, 0); /* allocate result */
    w = (CS_INT*)cs_calloc(m, sizeof(CS_INT));             /* get workspace */
    if (!C || !w) {
        return (cs_done(C, w, NULL, 0));
    } /* out of memory */
    Cp = C->p;
    Ci = C->i;
    Cx = C->x;
    for (p = 0; p < Ap[n]; p++) {
        w[Ai[p]]++;
    }                    /* row counts */
    cs_cumsum(Cp, w, m); /* row pointers */
    for (j = 0; j < n; j++) {
        for (p = Ap[j]; p < Ap[j + 1]; p++) {
            Ci[q = w[Ai[p]]++] = j; /* place A(i,j) as entry C(j,i) */
            if (Cx) {
                Cx[q] = (values > 0) ? CS_CONJ(Ax[p]) : Ax[p];
            }
        }
    }
    return (cs_done(C, w, NULL, 1)); /* success; free w and return C */
}

/* cs_compress.c */

#include "cs.h"
/* C = compressed-column form of a triplet matrix T */
cs* cs_compress(const cs* T) {
    CS_INT m, n, nz, p, k, *Cp, *Ci, *w, *Ti, *Tj;
    CS_ENTRY *Cx, *Tx;
    cs* C;
    if (!CS_TRIPLET(T)) {
        return (NULL);
    } /* check inputs */
    m = T->m;
    n = T->n;
    Ti = T->i;
    Tj = T->p;
    Tx = T->x;
    nz = T->nz;
    C = cs_spalloc(m, n, nz, Tx != NULL, 0); /* allocate result */
    w = (CS_INT*)cs_calloc(n, sizeof(CS_INT));        /* get workspace */
    if (!C || !w) {
        return (cs_done(C, w, NULL, 0));
    } /* out of memory */
    Cp = C->p;
    Ci = C->i;
    Cx = C->x;
    for (k = 0; k < nz; k++) {
        w[Tj[k]]++;
    }                    /* column counts */
    cs_cumsum(Cp, w, n); /* column pointers */
    for (k = 0; k < nz; k++) {
        Ci[p = w[Tj[k]]++] = Ti[k]; /* A(i,j) is the pth entry in C */
        if (Cx) {
            Cx[p] = Tx[k];
        }
    }
    return (cs_done(C, w, NULL, 1)); /* success; free w and return C */
}

/* cs_updown.c */

#include "cs.h"
/* sparse Cholesky update/downdate, L*L' + sigma*w*w' (sigma = +1 or -1) */
CS_INT cs_updown(cs* L, CS_INT sigma, const cs* C, const CS_INT* parent) {
    CS_INT n, p, f, j, *Lp, *Li, *Cp, *Ci;
    CS_ENTRY *Lx, *Cx, alpha, gamma, w1, w2, *w, tmp;
    double beta = 1, beta2 = 1, delta;
#ifdef CS_COMPLEX
    CS_ENTRY phase;
#endif
    if (!CS_CSC(L) || !CS_CSC(C) || !parent) {
        return (0);
    } /* check inputs */
    Lp = L->p;
    Li = L->i;
    Lx = L->x;
    n = L->n;
    Cp = C->p;
    Ci = C->i;
    Cx = C->x;
    if ((p = Cp[0]) >= Cp[1]) {
        return (1);
    }                                   /* return if C empty */
    w = (CS_ENTRY*)cs_malloc(n, sizeof(CS_ENTRY)); /* get workspace */
    if (!w) {
        return (0);
    } /* out of memory */
    f = Ci[p];
    for (; p < Cp[1]; p++) {
        f = CS_MIN(f, Ci[p]);
    } /* f = min (find (C)) */
    for (j = f; j != -1; j = parent[j]) {
        w[j] = cs_createFromDouble(0);
    } /* clear workspace w */
    for (p = Cp[0]; p < Cp[1]; p++) {
        w[Ci[p]] = Cx[p];
    }                                     /* w = C */
    for (j = f; j != -1; j = parent[j]) { /* walk path f up to root */
        p = Lp[j];
        alpha = cs_divcc(w[j], Lx[p]); /* alpha = w(j) / L(j,j) */
        beta2 = beta * beta + cs_muldc(sigma, cs_mulcc(alpha, CS_CONJ(alpha)));
        if (beta2 <= 0) {
            break;
        } /* not positive definite */
        beta2 = sqrt(beta2);
        delta = (sigma > 0) ? (beta / beta2) : (beta2 / beta);
        gamma = cs_divcc(cs_mulcc(cs_createFromDouble(sigma), CS_CONJ(alpha)), cs_createFromDouble(beta2 * beta));
        tmp = ((sigma > 0) ? (cs_mulcc(gamma, w[j])) : cs_createFromDouble(0));
        Lx[p] = cs_pluscc(cs_mulcc(cs_createFromDouble(delta), Lx[p]), tmp);
        beta = beta2;
#ifdef CS_COMPLEX
        phase = cs_divcc(cs_createFromDouble(CS_ABS(Lx[p])), Lx[p]); /* phase = abs(L(j,j))/L(j,j)*/
        Lx[p] = cs_mulcc(Lx[p], phase);                /* L(j,j) = L(j,j) * phase */
#endif
        for (p++; p < Lp[j + 1]; p++) {
            w1 = w[Li[p]];
            w[Li[p]] = w2 = cs_subcc(w1, cs_mulcc(alpha, Lx[p]));
            Lx[p] = cs_pluscc(cs_mulcc(cs_createFromDouble(delta), Lx[p]), cs_mulcc(gamma, ((sigma > 0) ? w1 : w2)));
#ifdef CS_COMPLEX
            Lx[p] = cs_mulcc(Lx[p], phase); /* L(i,j) = L(i,j) * phase */
#endif
        }
    }
    cs_free(w);
    return (beta2 > 0);
}

/* cs_usolve.c */

#include "cs.h"
/* solve Ux=b where x and b are dense.  x=b on input, solution on output. */
CS_INT cs_usolve(const cs* U, CS_ENTRY* x) {
    CS_INT p, j, n, *Up, *Ui;
    CS_ENTRY* Ux;
    if (!CS_CSC(U) || !x) {
        return (0);
    } /* check inputs */
    n = U->n;
    Up = U->p;
    Ui = U->i;
    Ux = U->x;
    for (j = n - 1; j >= 0; j--) {
        x[j] = cs_divcc(x[j], Ux[Up[j + 1] - 1]);
        for (p = Up[j]; p < Up[j + 1] - 1; p++) {
            x[Ui[p]] = cs_subcc(x[Ui[p]], cs_mulcc(Ux[p], x[j]));
        }
    }
    return (1);
}

/* cs_utsolve.c */

#include "cs.h"
/* solve U'x=b where x and b are dense.  x=b on input, solution on output. */
CS_INT cs_utsolve(const cs* U, CS_ENTRY* x) {
    CS_INT p, j, n, *Up, *Ui;
    CS_ENTRY* Ux;
    if (!CS_CSC(U) || !x) {
        return (0);
    } /* check inputs */
    n = U->n;
    Up = U->p;
    Ui = U->i;
    Ux = U->x;
    for (j = 0; j < n; j++) {
        for (p = Up[j]; p < Up[j + 1] - 1; p++) {
            x[j] = cs_subcc(x[j], cs_mulcc(CS_CONJ(Ux[p]), x[Ui[p]]));
        }
        x[j] = cs_divcc(x[j], CS_CONJ(Ux[Up[j + 1] - 1]));
    }
    return (1);
}

/* cs_util.c */

#include "cs.h"
/* allocate a sparse matrix (triplet form or compressed-column form) */
cs* cs_spalloc(CS_INT m, CS_INT n, CS_INT nzmax, CS_INT values, CS_INT triplet) {
    cs* A = (cs*)cs_calloc(1, sizeof(cs)); /* allocate the cs struct */
    if (!A) {
        return (NULL);
    }         /* out of memory */
    A->m = m; /* define dimensions and nzmax */
    A->n = n;
    A->nzmax = nzmax = CS_MAX(nzmax, 1);
    A->nz = triplet ? 0 : -1; /* allocate triplet or comp.col */
    A->p = (CS_INT*)cs_malloc(triplet ? nzmax : n + 1, sizeof(CS_INT));
    A->i = (CS_INT*)cs_malloc(nzmax, sizeof(CS_INT));
    A->x = values ? (CS_ENTRY*)cs_malloc(nzmax, sizeof(CS_ENTRY)) : NULL;
    return ((!A->p || !A->i || (values && !A->x)) ? cs_spfree(A) : A);
}

/* change the max # of entries sparse matrix */
CS_INT cs_sprealloc(cs* A, CS_INT nzmax) {
    CS_INT ok, oki, okj = 1, okx = 1;
    if (!A) {
        return (0);
    }
    if (nzmax <= 0) {
        nzmax = (CS_CSC(A)) ? (A->p[A->n]) : A->nz;
    }
    nzmax = CS_MAX(nzmax, 1);
    A->i = (CS_INT*)cs_realloc(A->i, nzmax, sizeof(CS_INT), &oki);
    if (CS_TRIPLET(A)) {
        A->p = (CS_INT*)cs_realloc(A->p, nzmax, sizeof(CS_INT), &okj);
    }
    if (A->x) {
        A->x = (CS_ENTRY*)cs_realloc(A->x, nzmax, sizeof(CS_ENTRY), &okx);
    }
    ok = (oki && okj && okx);
    if (ok) {
        A->nzmax = nzmax;
    }
    return (ok);
}

/* free a sparse matrix */
cs* cs_spfree(cs* A) {
    if (!A) {
        return (NULL);
    } /* do nothing if A already NULL */
    cs_free(A->p);
    cs_free(A->i);
    cs_free(A->x);
    return ((cs*)cs_free(A)); /* free the cs struct and return NULL */
}

/* free a numeric factorization */
csn* cs_nfree(csn* N) {
    if (!N) {
        return (NULL);
    } /* do nothing if N already NULL */
    cs_spfree(N->L);
    cs_spfree(N->U);
    cs_free(N->pinv);
    cs_free(N->B);
    return ((csn*)cs_free(N)); /* free the csn struct and return NULL */
}

/* free a symbolic factorization */
css* cs_sfree(css* S) {
    if (!S) {
        return (NULL);
    } /* do nothing if S already NULL */
    cs_free(S->pinv);
    cs_free(S->q);
    cs_free(S->parent);
    cs_free(S->cp);
    cs_free(S->leftmost);
    return ((css*)cs_free(S)); /* free the css struct and return NULL */
}

/* allocate a cs_dmperm or cs_scc result */
csd* cs_dalloc(CS_INT m, CS_INT n) {
    csd* D;
    D = (csd*)cs_calloc(1, sizeof(csd));
    if (!D) {
        return (NULL);
    }
    D->p = (CS_INT*)cs_malloc(m, sizeof(CS_INT));
    D->r = (CS_INT*)cs_malloc(m + 6, sizeof(CS_INT));
    D->q = (CS_INT*)cs_malloc(n, sizeof(CS_INT));
    D->s = (CS_INT*)cs_malloc(n + 6, sizeof(CS_INT));
    return ((!D->p || !D->r || !D->q || !D->s) ? cs_dfree(D) : D);
}

/* free a cs_dmperm or cs_scc result */
csd* cs_dfree(csd* D) {
    if (!D) {
        return (NULL);
    } /* do nothing if D already NULL */
    cs_free(D->p);
    cs_free(D->q);
    cs_free(D->r);
    cs_free(D->s);
    return ((csd*)cs_free(D)); /* free the csd struct and return NULL */
}

/* free workspace and return a sparse matrix result */
cs* cs_done(cs* C, void* w, void* x, CS_INT ok) {
    cs_free(w); /* free workspace */
    cs_free(x);
    return (ok ? C : cs_spfree(C)); /* return result if OK, else free it */
}

/* free workspace and return CS_INT array result */
CS_INT* cs_idone(CS_INT* p, cs* C, void* w, CS_INT ok) {
    cs_spfree(C);                          /* free temporary matrix */
    cs_free(w);                            /* free workspace */
    return (ok ? p : (CS_INT*)cs_free(p)); /* return result, or free it */
}

/* free workspace and return a numeric factorization (Cholesky, LU, or QR) */
csn* cs_ndone(csn* N, cs* C, void* w, void* x, CS_INT ok) {
    cs_spfree(C); /* free temporary matrix */
    cs_free(w);   /* free workspace */
    cs_free(x);
    return (ok ? N : cs_nfree(N)); /* return result if OK, else free it */
}

/* free workspace and return a csd result */
csd* cs_ddone(csd* D, cs* C, void* w, CS_INT ok) {
    cs_spfree(C);                  /* free temporary matrix */
    cs_free(w);                    /* free workspace */
    return (ok ? D : cs_dfree(D)); /* return result if OK, else free it */
}

/* cs_reach.c */

#include "cs.h"
/* xi [top...n-1] = nodes reachable from graph of G*P' via nodes in B(:,k).
 * xi [n...2n-1] used as workspace */
CS_INT cs_reach(cs* G, const cs* B, CS_INT k, CS_INT* xi, const CS_INT* pinv) {
    CS_INT p, n, top, *Bp, *Bi, *Gp;
    if (!CS_CSC(G) || !CS_CSC(B) || !xi) {
        return (-1);
    } /* check inputs */
    n = G->n;
    Bp = B->p;
    Bi = B->i;
    Gp = G->p;
    top = n;
    for (p = Bp[k]; p < Bp[k + 1]; p++) {
        if (!CS_MARKED(Gp, Bi[p])) { /* start a dfs at unmarked node i */
            top = cs_dfs(Bi[p], G, top, xi, xi + n, pinv);
        }
    }
    for (p = top; p < n; p++) {
        CS_MARK(Gp, xi[p]);
    } /* restore G */
    return (top);
}

/* cs_spsolve.c */

#include "cs.h"
/* solve Gx=b(:,k), where G is either upper (lo=0) or lower (lo=1) triangular */
CS_INT
cs_spsolve(cs* G, const cs* B, CS_INT k, CS_INT* xi, CS_ENTRY* x, const CS_INT* pinv, CS_INT lo) {
    CS_INT j, J, p, q, px, top, n, *Gp, *Gi, *Bp, *Bi;
    CS_ENTRY *Gx, *Bx;
    if (!CS_CSC(G) || !CS_CSC(B) || !xi || !x) {
        return (-1);
    }
    Gp = G->p;
    Gi = G->i;
    Gx = G->x;
    n = G->n;
    Bp = B->p;
    Bi = B->i;
    Bx = B->x;
    top = cs_reach(G, B, k, xi, pinv); /* xi[top..n-1]=Reach(B(:,k)) */
    for (p = top; p < n; p++) {
        x[xi[p]] = cs_createFromDouble(0);
    } /* clear x */
    for (p = Bp[k]; p < Bp[k + 1]; p++) {
        x[Bi[p]] = Bx[p];
    } /* scatter B */
    for (px = top; px < n; px++) {
        j = xi[px];               /* x(j) is nonzero */
        J = pinv ? (pinv[j]) : j; /* j maps to col J of G */
        if (J < 0) {
            continue;
        }                                           /* column J is empty */
        x[j] = cs_divcc(x[j], Gx[lo ? (Gp[J]) : (Gp[J + 1] - 1)]); /* x(j) /= G(j,j) */
        p = lo ? (Gp[J] + 1) : (Gp[J]);             /* lo: L(j,j) 1st entry */
        q = lo ? (Gp[J + 1]) : (Gp[J + 1] - 1);     /* up: U(j,j) last entry */
        for (; p < q; p++) {
            x[Gi[p]] = cs_subcc(x[Gi[p]], cs_mulcc(Gx[p], x[j])); /* x(i) -= G(i,j) * x(j) */
        }
    }
    return (top); /* return top of stack */
}

/* cs_ereach.c */

#include "cs.h"
/* find nonzero pattern of Cholesky L(k,1:k-1) using etree and triu(A(:,k)) */
CS_INT cs_ereach(const cs* A, CS_INT k, const CS_INT* parent, CS_INT* s, CS_INT* w) {
    CS_INT i, p, n, len, top, *Ap, *Ai;
    if (!CS_CSC(A) || !parent || !s || !w) {
        return (-1);
    } /* check inputs */
    top = n = A->n;
    Ap = A->p;
    Ai = A->i;
    CS_MARK(w, k); /* mark node k as visited */
    for (p = Ap[k]; p < Ap[k + 1]; p++) {
        i = Ai[p]; /* A(i,k) is nonzero */
        if (i > k) {
            continue;
        }                                                /* only use upper triangular part of A */
        for (len = 0; !CS_MARKED(w, i); i = parent[i]) { /* traverse up etree*/
            s[len++] = i;                                /* L(k,i) is nonzero */
            CS_MARK(w, i);                               /* mark i as visited */
        }
        while (len > 0) {
            s[--top] = s[--len];
        } /* push path onto stack */
    }
    for (p = top; p < n; p++) {
        CS_MARK(w, s[p]);
    }              /* unmark all nodes */
    CS_MARK(w, k); /* unmark node k */
    return (top);  /* s [top..n-1] contains pattern of L(k,:)*/
}

/* cs_leaf.c */

#include "cs.h"
/* consider A(i,j), node j in ith row subtree and return lca(jprev,j) */
CS_INT cs_leaf(CS_INT i,
               CS_INT j,
               const CS_INT* first,
               CS_INT* maxfirst,
               CS_INT* prevleaf,
               CS_INT* ancestor,
               CS_INT* jleaf) {
    CS_INT q, s, sparent, jprev;
    if (!first || !maxfirst || !prevleaf || !ancestor || !jleaf) {
        return (-1);
    }
    *jleaf = 0;
    if (i <= j || first[j] <= maxfirst[i]) {
        return (-1);
    }                       /* j not a leaf */
    maxfirst[i] = first[j]; /* update max first[j] seen so far */
    jprev = prevleaf[i];    /* jprev = previous leaf of ith subtree */
    prevleaf[i] = j;
    *jleaf = (jprev == -1) ? 1 : 2; /* j is first or subsequent leaf */
    if (*jleaf == 1) {
        return (i);
    } /* if 1st leaf, q = root of ith subtree */
    for (q = jprev; q != ancestor[q]; q = ancestor[q])
        ;
    for (s = jprev; s != q; s = sparent) {
        sparent = ancestor[s]; /* path compression */
        ancestor[s] = q;
    }
    return (q); /* q = least common ancester (jprev,j) */
}

/* cs_randperm.c */

#include "cs.h"
/* return a random permutation vector, the identity perm, or p = n-1:-1:0.
 * seed = -1 means p = n-1:-1:0.  seed = 0 means p = identity.  otherwise
 * p = random permutation.  */
CS_INT* cs_randperm(CS_INT n, CS_INT seed) {
    CS_INT *p, k, j, t;
    if (seed == 0) {
        return (NULL);
    }                                 /* return p = NULL (identity) */
    p = (CS_INT*)cs_malloc(n, sizeof(CS_INT)); /* allocate result */
    if (!p) {
        return (NULL);
    } /* out of memory */
    for (k = 0; k < n; k++) {
        p[k] = n - k - 1;
    }
    if (seed == -1) {
        return (p);
    }            /* return reverse permutation */
    srand(seed); /* get new random number seed */
    for (k = 0; k < n; k++) {
        j = k + (rand() % (n - k)); /* j = rand integer in range k to n-1 */
        t = p[j];                   /* swap p[k] and p[j] */
        p[j] = p[k];
        p[k] = t;
    }
    return (p);
}

/* cs_operator.c */

#include "cs.h"
#include <math.h>
/**
 * Microsoft Visual Stuido does not support standard C99 complex.
 * But it has limited complex support(_DComplex).
 * Add several wrapper functions for operators that Visual Studio complex does not support.
 **/

/**
 * Divide two CS_ENTRYs
 */ 
CS_ENTRY cs_divcc(CS_ENTRY lhs, CS_ENTRY rhs) {
#if !defined(__cplusplus) && defined(_MSC_VER) && !defined(__ICL) && defined(CS_COMPLEX)
    double r,i;
    if (fabs(creal(rhs)) > fabs(cimag(rhs))) {
    double bim = cimag(rhs) / creal(rhs);
    double d = creal(rhs) + bim * cimag(rhs);
    r = (creal(lhs) + bim * cimag(lhs)) / d;
    i = (cimag(lhs) - bim * creal(lhs)) / d;
    } else {
    double bim = creal(rhs) / cimag(rhs);
    double d = cimag(rhs) + bim * creal(rhs);
    r = (bim * creal(lhs) + cimag(lhs)) / d;
    i = (bim * cimag(lhs) - creal(lhs)) / d;
    }
    return _DCOMPLEX_(r,i);
#else
    return lhs / rhs;
#endif
}

/**
 * Multiply two CS_ENTRYs
 */ 
CS_ENTRY cs_mulcc(CS_ENTRY lhs, CS_ENTRY rhs) {
#if !defined(__cplusplus) && defined(_MSC_VER) && !defined(__ICL) && defined(CS_COMPLEX)
    double r = creal(lhs)*creal(rhs) - cimag(lhs)*cimag(rhs);
    double i = cimag(lhs)*creal(rhs) + creal(lhs)*cimag(rhs);
    return _DCOMPLEX_(r, i);
#else
    return lhs * rhs;
#endif
}

/**
 * Subtract two CS_ENTRYs
 */ 
CS_ENTRY cs_subcc(CS_ENTRY lhs, CS_ENTRY rhs) {
#if !defined(__cplusplus) && defined(_MSC_VER) && !defined(__ICL) && defined(CS_COMPLEX)
    return _DCOMPLEX_(creal(lhs) - creal(rhs), cimag(lhs) - cimag(rhs));
#else
    return lhs - rhs;
#endif
}

/**
 * Add two CS_ENTRYs
 */ 
CS_ENTRY cs_pluscc(CS_ENTRY lhs, CS_ENTRY rhs) {
#if !defined(__cplusplus) && defined(_MSC_VER) && !defined(__ICL) && defined(CS_COMPLEX)
    return _DCOMPLEX_(creal(lhs) + creal(rhs), cimag(lhs) + cimag(rhs));
#else
    return lhs + rhs;
#endif
}

/**
 * Create a CS_ENTRY form double
 */ 
CS_ENTRY cs_createFromDouble(double aVal) {
#if !defined(__cplusplus) && defined(_MSC_VER) && !defined(__ICL) && defined(CS_COMPLEX)
    return _DCOMPLEX_(aVal, 0);
#else
    return aVal;
#endif
}

/**
 * Determine two CS_ETNRYs are equal
 */ 
int cs_equalcc(CS_ENTRY lhs, CS_ENTRY rhs) {
#if !defined(__cplusplus) && defined(_MSC_VER) && !defined(__ICL) && defined(CS_COMPLEX)
    return ((creal(lhs) == creal(rhs)) && (cimag(lhs) == cimag(rhs))) ?  1 : 0;
#else
    return (lhs == rhs) ? 1 : 0;
#endif
}

/**
 * sqrt of CS_ENTRY
 * This function by design is doing real only sqrt.
 * If need to do complex, use csqrt.
 */ 
CS_ENTRY cs_sqrt(CS_ENTRY in) {
#if !defined(__cplusplus) && defined(_MSC_VER) && !defined(__ICL) && defined(CS_COMPLEX)
    return _DCOMPLEX_(sqrt(creal(in)), 0);
#else
    return sqrt(in);
#endif
}

/**
 * Change sign of CS_ENTRY
 */ 
CS_ENTRY cs_flipSign(CS_ENTRY in) {
#if !defined(__cplusplus) && defined(_MSC_VER) && !defined(__ICL) && defined(CS_COMPLEX)
    return _DCOMPLEX_(-creal(in), -cimag(in));
#else
    return -in;
#endif
}

/**
 * Multiply a double with CS_ENTRY and results a double
 */ 
double cs_muldc(double lhs, CS_ENTRY rhs) {
    return lhs * CS_REAL(rhs);
}
