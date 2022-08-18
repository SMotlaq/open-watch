/* Copyright 2019-2021 The MathWorks, Inc. */
/* Copied from
 * fullfile(matlabroot,'extern','include','coder','coder_array','coder_array_rtw_cpp11.h') */

#ifndef _mw_coder_array_cpp11_h
#define _mw_coder_array_cpp11_h

//  Usage:
//
//  coder::array<T, N>: T base type of data, N number of dimensions
//
//  coder::array()
//               : default constructor
//  coder::array(coder::array const &)
//               : copy constructor (always make a deep copy of other array)
//  coder::array(T const *data, SizeType const *sz)
//               : Set data with sizes of this array.
//               : (Data is not copied, data is not deleted)
//  coder::array::operator = (coder coder::array &)
//               : Assign into this array;
//               : delete its previous contents (if owning the data.)
//  set(T const *data, SizeType sz1, SizeType sz2, ...)
//               : Set data with dimensions.
//               : (Data is not copied, data is not deleted)
//  set_size(SizeType sz1, SizeType sz2, ...)
//               : Set sizes of array. Reallocate memory of data if needed.
//  bool is_owner() : Return true if the data is owned by the class.
//  void set_owner(b) : Set if the data is owned by the class.
//  SizeType capacity() : How many entries are reserved by memory allocation.
//  reshape( SizeType sz1, SizeType sz2, ...)
//               : Reshape array to a different ND shape. Do not copy the data.
//               : The number of elements must not be changed (numel()==sz1*sz2*...)
//               : Return the array with possibly new number of dimensions.
//  clear()      : Reset array to be empty.
//  SizeType numel() : Return the number of elements.
//  operator [] (SizeType index) : Extract element at linear index (0 based.)
//  size(SizeType dimension) : Size of array of the provided dimension.
//  SizeType * size() : Return the pointer to all the sizes of this array.
//  SizeType index(SizeType i1, SizeType i2, ...)
//               : Compute the linear index from ND index (i1,i2,...)
//  at(SizeType i1, SizeType i2, ...) : The element at index (i1,i2,...)

#include <cassert>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

#ifndef INT32_T
#include "rtwtypes.h"
#endif

namespace coder {

#ifndef CODER_ARRAY_NEW_DELETE
#define CODER_ARRAY_NEW_DELETE
#define CODER_NEW(T, N) new T[N]
#define CODER_DELETE(P) delete[](P)
#endif

#ifndef CODER_ARRAY_SIZE_TYPE_DEFINED
using SizeType = int;
#endif

namespace std = ::std;

namespace detail {

#ifndef CODER_ARRAY_DATA_PTR_DEFINED
template <typename T, typename SZ>
class data_ptr {
  public:
    using value_type = T;
    using size_type = SZ;

    data_ptr()
        : data_(nullptr)
        , size_(0)
        , capacity_(0)
        , owner_(false) {
    }
    data_ptr(T* _data, SZ _sz)
        : data_(_data)
        , size_(_sz)
        , capacity_(_sz)
        , owner_(false) {
    }

    data_ptr(data_ptr const& _other)
        : data_(_other.owner_ ? nullptr : _other.data_)
        , size_(_other.owner_ ? 0 : _other.size_)
        , capacity_(_other.owner_ ? 0 : _other.capacity_)
        , owner_(_other.owner_) {
        if (owner_) {
            resize(_other.size_);
            (void)std::copy(_other.data_, _other.data_ + size_, data_);
        }
    }

    ~data_ptr() {
        if (owner_) {
            CODER_DELETE(data_);
        }
    }
    SZ capacity() const {
        return capacity_;
    }
    void reserve(SZ _n) {
        if (_n > capacity_) {
            T* const new_data{CODER_NEW(T, _n)};
            (void)std::copy(data_, data_ + size_, new_data);
            if (owner_) {
                CODER_DELETE(data_);
            }
            data_ = new_data;
            capacity_ = _n;
            owner_ = true;
        }
    }
    void resize(SZ _n) {
        reserve(_n);
        size_ = _n;
    }

  private:
    // Prohibit use of assignment operator to prevent subtle bugs
    void operator=(data_ptr<T, SZ> const& _other);

  public:
    void set(T* _data, SZ _sz) {
        if (owner_) {
            CODER_DELETE(data_);
        }
        data_ = _data;
        size_ = _sz;
        owner_ = false;
        capacity_ = size_;
    }

    void copy(T const* const _data, SZ _size) {
        if (data_ == _data) {
            size_ = _size;
            return;
        }
        if (owner_) {
            CODER_DELETE(data_);
        }
        data_ = CODER_NEW(T, _size);
        owner_ = true;
        size_ = _size;
        capacity_ = size_;
        (void)std::copy(_data, _data + _size, data_);
    }

    void copy(data_ptr<T, SZ> const& _other) {
        copy(_other.data_, _other.size_);
    }

    operator T*() {
        return &data_[0];
    }

    operator T const *() const {
        return &data_[0];
    }

    T& operator[](SZ _index) {
        return data_[_index];
    }
    T const& operator[](SZ _index) const {
        return data_[_index];
    }

    T* operator->() {
        return data_;
    }

    T const* operator->() const {
        return data_;
    }

    bool is_null() const {
        return data_ == nullptr;
    }

    void clear() {
        if (owner_) {
            CODER_DELETE(data_);
        }
        data_ = nullptr;
        size_ = 0;
        capacity_ = 0;
        owner_ = false;
    }

    bool is_owner() const {
        return owner_;
    }

    void set_owner(bool _b) {
        owner_ = _b;
    }

  private:
    T* data_;
    SZ size_;
    SZ capacity_;
    bool owner_;
};
#endif

} // namespace detail

// Implementing the random access iterator class so coder::array can be
// used in STL iterators.
template <typename T>
class array_iterator : public std::iterator<std::random_access_iterator_tag,
                                            typename T::value_type,
                                            typename T::size_type> {
  public:
    array_iterator()
        : arr_(nullptr)
        , i_(0) {
    }
    array_iterator(array_iterator<T> const& other)
        : arr_(other.arr_)
        , i_(other.i_) {
    }
    ~array_iterator() {
    }
    typename T::value_type& operator*() const {
        return (*arr_)[i_];
    }
    typename T::value_type* operator->() const {
        return &(*arr_)[i_];
    }
    typename T::value_type& operator[](typename T::size_type _di) const {
        return (*arr_)[i_ + _di];
    }
    array_iterator<T>& operator++() {
        ++i_;
        return *this;
    }
    array_iterator<T>& operator--() {
        --i_;
        return *this;
    }
    array_iterator<T> operator++(int) {
        array_iterator<T> cp{*this};
        ++i_;
        return cp;
    }
    array_iterator<T> operator--(int) {
        array_iterator<T> cp{*this};
        --i_;
        return cp;
    }
    array_iterator<T>& operator=(array_iterator<T> const& _other) {
        this->i_ = _other.i_;
        return *this;
    }
    bool operator==(array_iterator<T> const& _other) const {
        return i_ == _other.i_;
    }
    bool operator!=(array_iterator<T> const& _other) const {
        return i_ != _other.i_;
    }
    bool operator<(array_iterator<T> const& _other) const {
        return i_ < _other.i_;
    }
    bool operator>(array_iterator<T> const& _other) const {
        return i_ > _other.i_;
    }
    bool operator<=(array_iterator<T> const& _other) const {
        return i_ <= _other.i_;
    }
    bool operator>=(array_iterator<T> const& _other) const {
        return i_ >= _other.i_;
    }
    array_iterator<T> operator+(typename T::size_type _add) const {
        array_iterator<T> cp{*this};
        cp.i_ += _add;
        return cp;
    }
    array_iterator<T>& operator+=(typename T::size_type _add) {
        this->i_ += _add;
        return *this;
    }
    array_iterator<T> operator-(typename T::size_type _subtract) const {
        array_iterator<T> cp{*this};
        cp.i_ -= _subtract;
        return cp;
    }
    array_iterator<T>& operator-=(typename T::size_type _subtract) {
        this->i_ -= _subtract;
        return *this;
    }
    typename T::size_type operator-(array_iterator<T> const& _other) const {
        return static_cast<typename T::size_type>(this->i_ - _other.i_);
    }

    array_iterator(T* _arr, typename T::size_type _i)
        : arr_(_arr)
        , i_(_i) {
    }

  private:
    T* arr_;
    typename T::size_type i_;
};

// Const version of the array iterator.
template <typename T>
class const_array_iterator : public std::iterator<std::random_access_iterator_tag,
                                                  typename T::value_type,
                                                  typename T::size_type> {
  public:
    const_array_iterator()
        : arr_(nullptr)
        , i_(0) {
    }
    const_array_iterator(const_array_iterator<T> const& other)
        : arr_(other.arr_)
        , i_(other.i_) {
    }
    ~const_array_iterator() {
    }
    typename T::value_type const& operator*() const {
        return (*arr_)[i_];
    }
    typename T::value_type const* operator->() const {
        return &(*arr_)[i_];
    }
    typename T::value_type const& operator[](typename T::size_type _di) const {
        return (*arr_)[i_ + _di];
    }
    const_array_iterator<T>& operator++() {
        ++i_;
        return *this;
    }
    const_array_iterator<T>& operator--() {
        --i_;
        return *this;
    }
    const_array_iterator<T> operator++(int) {
        const_array_iterator<T> copy{*this};
        ++i_;
        return copy;
    }
    const_array_iterator<T> operator--(int) {
        const_array_iterator copy{*this};
        --i_;
        return copy;
    }
    const_array_iterator<T>& operator=(const_array_iterator<T> const& _other) {
        this->i_ = _other.i_;
        return *this;
    }
    bool operator==(const_array_iterator<T> const& _other) const {
        return i_ == _other.i_;
    }
    bool operator!=(const_array_iterator<T> const& _other) const {
        return i_ != _other.i_;
    }
    bool operator<(const_array_iterator<T> const& _other) const {
        return i_ < _other.i_;
    }
    bool operator>(const_array_iterator<T> const& _other) const {
        return i_ > _other.i_;
    }
    bool operator<=(const_array_iterator<T> const& _other) const {
        return i_ <= _other.i_;
    }
    bool operator>=(const_array_iterator<T> const& _other) const {
        return i_ >= _other.i_;
    }
    const_array_iterator<T> operator+(typename T::size_type _add) const {
        const_array_iterator<T> cp{*this};
        cp.i_ += _add;
        return cp;
    }
    const_array_iterator<T>& operator+=(typename T::size_type _add) {
        this->i_ += _add;
        return *this;
    }
    const_array_iterator<T> operator-(typename T::size_type _subtract) const {
        const_array_iterator<T> cp{*this};
        cp.i_ -= _subtract;
        return cp;
    }

    const_array_iterator<T>& operator-=(typename T::size_type _subtract) {
        this->i_ -= _subtract;
        return *this;
    }

    typename T::size_type operator-(const_array_iterator<T> const& _other) const {
        return static_cast<typename T::size_type>(this->i_ - _other.i_);
    }

    const_array_iterator(T const* _arr, typename T::size_type _i)
        : arr_(_arr)
        , i_(_i) {
    }

  private:
    T const* arr_;
    typename T::size_type i_;
    typename T::size_type n_;
};

namespace detail {

// detail::numel<N>: Compile-time product of the given size vector of length N.
template <int N>
class numel {
  public:
    template <typename SZ>
    static SZ compute(SZ _size[]) {
        return _size[N - 1] * numel<N - 1>::compute(_size);
    }
};
template <>
class numel<0> {
  public:
    template <typename SZ>
    static SZ compute(SZ[]) {
        return 1;
    }
};

// Compute the product for a set of numeric arguments: product<int32_T>(10, 20, 30, ...) =>
// 10*20*30*...
template <typename SZ, typename First, typename... Rest>
struct product_i {
    static SZ compute(First _f, Rest... _rest) {
        return _f * product_i<SZ, Rest...>::compute(_rest...);
    }
};
template <typename SZ, typename Last>
struct product_i<SZ, Last> {
    static SZ compute(Last _l) {
        return _l;
    }
};

template <typename SZ, typename... Args>
SZ product(Args... args) {
    return product_i<SZ, Args...>::compute(args...);
}

// Compute flat index from (column-major) ND size vector and a list of indices.
template <int I>
class index_nd {
  public:
    template <typename SZ>
    static SZ compute(SZ const _size[], SZ const _indices[]) {
        SZ const weight{numel<I - 1>::compute(_size)};
        return weight * _indices[I - 1] + index_nd<I - 1>::compute(_size, _indices);
    }
};

template <>
class index_nd<0> {
  public:
    template <typename SZ>
    static SZ compute(SZ[], SZ[]) {
        return 0;
    }
};

template <bool Cond>
struct match_dimensions {};

template <>
struct match_dimensions<true> {
    static void check() {
    }
};

} // namespace detail

// Base class for code::array. SZ is the type used for sizes (currently int32_t.)
// Overloading up to 10 dimensions (not using variadic templates to
// stay compatible with C++98.)
template <typename T, typename SZ, int N>
class array_base {
  public:
    using value_type = T;
    using size_type = SZ;

    array_base() {
        (void)::memset(size_, 0, sizeof(SZ) * N);
    }

    array_base(T* _data, SZ const* _sz)
        : data_(_data, coder::detail::numel<N>::compute(_sz)) {
        (void)std::copy(_sz, _sz + N, size_);
    }

    array_base(array_base const&) = default;

    array_base& operator=(array_base const& _other) {
        data_.copy(_other.data_);
        (void)std::copy(_other.size_, _other.size_ + N, size_);
        return *this;
    }

    template <typename... Dims>
    void set(T* _data, Dims... dims) {
        coder::detail::match_dimensions<N == sizeof...(dims)>::check();
        data_.set(_data, coder::detail::product<SZ>(dims...));
        set_size_i<0>(dims...);
    }

    bool is_owner() const {
        return data_.is_owner();
    }

    void set_owner(bool b) {
        data_.set_owner(b);
    }

    SZ capacity() const {
        return data_.capacity();
    }

  private:
    template <SZ _i, typename First, typename... Rest>
    void set_size_i(First f, Rest... rest) {
        size_[_i] = f;
        set_size_i<_i + 1, Rest...>(rest...);
    }
    template <SZ _i, typename Last>
    void set_size_i(Last l) {
        size_[_i] = l;
    }

  public:
    void reserve(SZ _n) {
        ensureCapacity(_n);
    }

    template <typename... Dims>
    void set_size(Dims... dims) {
        coder::detail::match_dimensions<N == sizeof...(dims)>::check();
        set_size_i<0>(dims...);
        ensureCapacity(numel());
    }

    template <SizeType N1>
    array_base<T, SZ, N1> reshape_n(SZ const (&_ns)[N1]) const {
        array_base<T, SZ, N1> reshaped{const_cast<T*>(&data_[0]), _ns};
        return reshaped;
    }

    template <typename... Dims>
    array_base<T, SZ, static_cast<SZ>(sizeof...(Dims))> reshape(Dims... dims) const {
        SZ const ns[]{static_cast<SZ>(dims)...};
        return reshape_n(ns);
    }

    T& operator[](SZ _index) {
        return data_[_index];
    }

    T const& operator[](SZ _index) const {
        return data_[_index];
    }

    void clear() {
        data_.clear();
    }

    T* data() {
        return data_;
    }

    T const* data() const {
        return data_;
    }

    SZ const* size() const {
        return &size_[0];
    }

    SZ size(SZ _index) const {
        return size_[_index];
    }

    SZ numel() const {
        return coder::detail::numel<N>::compute(size_);
    }

    template <typename... Dims>
    SZ index(Dims... _dims) const {
        coder::detail::match_dimensions<N == sizeof...(_dims)>::check();
        SZ const indices[]{static_cast<SZ>(_dims)...};
        return coder::detail::index_nd<static_cast<SZ>(sizeof...(_dims))>::compute(size_, indices);
    }

    template <typename... Dims>
    T& at(Dims... _i) {
        coder::detail::match_dimensions<N == sizeof...(_i)>::check();
        return data_[index(_i...)];
    }

    template <typename... Dims>
    T const& at(Dims... _i) const {
        coder::detail::match_dimensions<N == sizeof...(_i)>::check();
        return data_[index(_i...)];
    }

    array_iterator<array_base<T, SZ, N> > begin() {
        return array_iterator<array_base<T, SZ, N> >(this, 0);
    }
    array_iterator<array_base<T, SZ, N> > end() {
        return array_iterator<array_base<T, SZ, N> >(this, this->numel());
    }
    const_array_iterator<array_base<T, SZ, N> > begin() const {
        return const_array_iterator<array_base<T, SZ, N> >(this, 0);
    }
    const_array_iterator<array_base<T, SZ, N> > end() const {
        return const_array_iterator<array_base<T, SZ, N> >(this, this->numel());
    }

  protected:
    coder::detail::data_ptr<T, SZ> data_;
    SZ size_[N];

  private:
    void ensureCapacity(SZ _newNumel) {
        if (_newNumel > data_.capacity()) {
            SZ i{data_.capacity()};
            if (i < 16) {
                i = 16;
            }

            while (i < _newNumel) {
                if (i > 1073741823) {
                    i = MAX_int32_T;
                } else {
                    i *= 2;
                }
            }
            data_.reserve(i);
        }
        data_.resize(_newNumel);
    }
};

// The standard coder::array class with base type and number of dimensions.
template <typename T, int N>
class array : public array_base<T, SizeType, N> {
  private:
    using Base = array_base<T, SizeType, N>;

  public:
    array()
        : Base() {
    }
    array(array<T, N> const& _other)
        : Base(_other) {
    }
    array(Base const& _other)
        : Base(_other) {
    }
    array(T* _data, SizeType const* _sz)
        : Base(_data, _sz) {
    }
};

// Specialize on char_T (row vector) for better support on strings.
template <>
class array<char_T, 2> : public array_base<char_T, SizeType, 2> {
  private:
    using Base = array_base<char_T, SizeType, 2>;

  public:
    array()
        : array_base() {
    }
    array(array<char_T, 2> const& _other)
        : Base(_other) {
    }
    array(Base const& _other)
        : Base(_other) {
    }

    array(std::string const& _str) {
        operator=(_str);
    }

    array(char_T const* const _str) {
        operator=(_str);
    }

    array(std::vector<char_T> const& _vec) {
        SizeType const n{static_cast<SizeType>(_vec.size())};
        set_size(1, n);
        data_.copy(&_vec[0], n);
    }

    array& operator=(std::string const& _str) {
        SizeType const n{static_cast<SizeType>(_str.size())};
        set_size(1, n);
        data_.copy(_str.c_str(), n);
        return *this;
    }

    array& operator=(char_T const* const _str) {
        SizeType const n{static_cast<SizeType>(strlen(_str))};
        set_size(1, n);
        data_.copy(_str, n);
        return *this;
    }

    operator std::string() const {
        return std::string(static_cast<char const*>(&(*this)[0]), static_cast<int>(size(1)));
    }
};

// Specialize on 2 dimensions for better support interactions with
// std::vector and row vectors.
template <typename T>
class array<T, 2> : public array_base<T, SizeType, 2> {
  private:
    using Base = array_base<T, SizeType, 2>;

  public:
    array()
        : Base() {
    }
    array(array<T, 2> const& _other)
        : Base(_other) {
    }
    array(Base const& _other)
        : Base(_other) {
    }
    array(std::vector<T> const& _vec) {
        operator=(_vec);
    }

    array& operator=(std::vector<T> const& _vec) {
        SizeType n{static_cast<SizeType>(_vec.size())};
        Base::set_size(1, n);
        Base::data_.copy(&_vec[0], n);
        return *this;
    }

    operator std::vector<T>() const {
        T const* p{&Base::data_[0]};
        return std::vector<T>(p, p + Base::numel());
    }
};

// Specialize on 1 dimension for better support with std::vector and
// column vectors.
template <typename T>
class array<T, 1> : public array_base<T, SizeType, 1> {
  private:
    using Base = array_base<T, SizeType, 1>;

  public:
    array()
        : Base() {
    }
    array(array<T, 1> const& _other)
        : Base(_other) {
    }
    array(Base const& _other)
        : Base(_other) {
    }
    array(std::vector<T> const& _vec) {
        operator=(_vec);
    }

    array& operator=(std::vector<T> const& _vec) {
        SizeType n{static_cast<SizeType>(_vec.size())};
        Base::set_size(n);
        Base::data_.copy(&_vec[0], n);
        return *this;
    }

    operator std::vector<T>() const {
        T const* p{&Base::data_[0]};
        return std::vector<T>(p, p + Base::numel());
    }
};
} // namespace coder

#endif
