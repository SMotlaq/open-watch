#ifndef _FONTS_H
#define _FONTS_H

#include "main.h"


//
//  Structure used to define fonts
//
typedef struct {
    const uint8_t FontWidth;    /* Font width in pixels */
    uint8_t FontHeight;         /* Font height in pixels */
    const uint16_t *data;       /* Pointer to data font data array */
} FontDef;

typedef struct {
    const uint8_t width;    /* Font width in pixels */
    uint8_t height;         /* Font height in pixels */
    const uint16_t *data;       /* Pointer to data font data array */
} ShapeDef;


//
//  Export the 3 available fonts
//

static const uint8_t new_map [] = {
0,0,0,0,0,
1,
0,0,
2,
3,
0,0,
4,
5,
6,
7,
8,
9,
10,
11,
12,
13,
14,
15,
16,
17,
18,
0,0,0,0,0,
19,
20,
21,
22,
23,
24,
25,
26,
27,
28,
29,
30,
31,
32,
33,
34,
35,
36,
37,
38,
39,
40,
41,
42,
43,
44,
45,
0,0,0,0,0,0,
46,
47,
48,
49,
50,
51,
52,
53,
54,
55,
56,
57,
58,
59,
60,
61,
62,
63,
64,
65,
66,
67,
68,
69,
70,
71,
0,
72,
0,0
};

extern FontDef Font_7x10;
extern FontDef Font_11x18;
extern FontDef Font_16x26;

extern ShapeDef Shape_battery;
extern ShapeDef Shape_battery2;
extern ShapeDef Shape_heart;
extern ShapeDef Shape_kafsh;
extern ShapeDef Shape_SpO2;
extern ShapeDef Shape_BT;
extern ShapeDef Shape_NC;
extern ShapeDef Shape_Connected;
extern ShapeDef Shape_Location;
extern ShapeDef Shape_Reject;
extern ShapeDef Shape_Accept;
extern ShapeDef Shape_UP;
extern ShapeDef Shape_DOWN;


#endif  // _FONTS_H
