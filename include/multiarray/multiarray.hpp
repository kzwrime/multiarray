#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>

namespace ma {

/**
 * @brief Return the name of this header-only library
 */
inline auto name() -> std::string { return "multiarray"; }

#ifdef MA_DIMS_TYPE
using dims_type = MA_DIMS_TYPE;
#else
using dims_type = std::size_t;
#endif

template <dims_type N, typename = void>
struct Dims_s;

template <dims_type N>
struct Dims_s<N, std::enable_if_t<(N > 0)>> {
  using type =
      decltype(std::tuple_cat(std::declval<typename Dims_s<N - 1>::type>(),
                              std::make_tuple(dims_type{})));
};

template <>
struct Dims_s<1, void> {
  using type = std::tuple<dims_type>;
};

template <>
struct Dims_s<0, void> {
  using type = std::tuple<>;
};

template <dims_type NDims>
using dims_t = typename Dims_s<NDims>::type;

template <typename Tuple, std::size_t N>
struct TupleToString {
  static void convert(std::ostringstream &ss, const Tuple &t) {
    TupleToString<Tuple, N - 1>::convert(ss, t);
    ss << ", " << std::get<N - 1>(t);
  }
};

template <typename Tuple>
struct TupleToString<Tuple, 1> {
  static void convert(std::ostringstream &ss, const Tuple &t) {
    ss << std::get<0>(t);
  }
};

template <typename... Args>
std::string tupleToString(const std::tuple<Args...> &t) {
  std::ostringstream ss;
  ss << "(";
  TupleToString<decltype(t), sizeof...(Args)>::convert(ss, t);
  ss << ")";
  return ss.str();
}

template <dims_type... Args>
std::string packNumToString() {
  std::ostringstream ss;
  ss << "(";
  bool first = true;
  ((ss << (first ? "" : ", ") << Args, first = false), ...);
  ss << ")";
  return ss.str();
}

template <int i, dims_type num>
constexpr dims_type get_i_of_pack_num() {
  static_assert(i == 0, "i must be 0 when there are only one num");
  return num;
}

template <int i, dims_type num, dims_type num1, dims_type... nums>
constexpr dims_type get_i_of_pack_num() {
  static_assert(i >= 0, "i should satisfy: i >= 0");
  static_assert(i < (2 + sizeof...(nums)),
                "i should satisfy: i < sizeof(pack of numbers)");
  if constexpr (i == 0)
    return num;
  else
    return get_i_of_pack_num<i - 1, num1, nums...>();
}

template <typename T, dims_type n_dims, bool ROW_MAJOR = true>
class multiarray {
  static_assert(std::is_pointer_v<T>, "Type T must be pointer");
  using T_no_pointer = typename std::remove_pointer<T>::type;

 public:
  T pointer = nullptr;
  dims_t<n_dims> dims;

  dims_type size() const {
    return std::apply([](auto &&...data) { return (data * ...); }, this->dims);
  }

  std::string string_dims() const { return tupleToString(this->dims); }

  template <typename... Types>
  void set_dims(Types... _dims) {
    static_assert(sizeof...(_dims) == n_dims,
                  "Number of _dims must be equal to n_dims");
    dims = std::make_tuple(_dims...);
  }

  template <typename... Types>
  void set(T _pointer, Types... _dims) {
    static_assert(sizeof...(_dims) == n_dims,
                  "Number of _dims must be equal to n_dims");
    this->pointer = _pointer;
    this->set_dims(_dims...);
  }

  // operator() overload for array access
  // calculate the offset in the linear array

  // at (column major)
  template <typename... Types>
  inline T_no_pointer &at_colm(Types... indices) {
    static_assert(sizeof...(indices) == n_dims,
                  "Number of indices must be equal to n_dims");
    return this->pointer[get_offset_col(indices...)];
  }

  // at (row major)
  template <typename... Types>
  inline T_no_pointer &at_row(Types... indices) {
    static_assert(sizeof...(indices) == n_dims,
                  "Number of indices must be equal to n_dims");
    return this->pointer[get_offset_row(indices...)];
  }

  template <typename... Types>
  inline T_no_pointer &operator()(Types... indices) {
    if constexpr (ROW_MAJOR)
      return this->pointer[get_offset_row(indices...)];
    else
      return this->pointer[get_offset_col(indices...)];
  }

  void clear() {
    this->pointer = nullptr;
    std::apply([](auto &...args) { ((args = 0), ...); }, dims);
  }

  multiarray() {}

  multiarray(T _pointer) : pointer(_pointer) {}

  multiarray(const multiarray<T, n_dims, ROW_MAJOR> &array)
      : pointer(array.pointer), dims(array.dims) {}

  template <typename... Types>
  multiarray(T _pointer, Types... _dims) {
    set(_pointer, _dims...);
  }

 private:
  template <typename Type0, typename... Types>
  inline dims_type get_offset_col(Type0 index, Types... indices) {
    return static_cast<dims_type>(index) +
           std::get<n_dims - sizeof...(indices) - 1>(this->dims) *
               get_offset_col(indices...);
  }

  template <typename Type0, typename... Types>
  inline dims_type get_offset_col(Type0 index) {
    return static_cast<dims_type>(index);
  }

  template <typename Type0, typename... Types>
  inline dims_type get_offset_row(Type0 index, Types... indices) {
    return get_offset_row_sum(static_cast<dims_type>(index), indices...);
  }

  template <typename Type0, typename... Types>
  inline dims_type get_offset_row_sum(dims_type part_sum, Type0 index,
                                      Types... indices) {
    return get_offset_row_sum(
        part_sum * std::get<n_dims - sizeof...(indices) - 1>(this->dims) +
            static_cast<dims_type>(index),
        indices...);
  }

  template <typename Type0, typename... Types>
  inline dims_type get_offset_row_sum(dims_type part_sum, Type0 index) {
    return part_sum * std::get<n_dims - 1>(this->dims) +
           static_cast<dims_type>(index);
  }
};

template <typename T, dims_type n_dims, bool ROW_MAJOR = true,
          dims_type... strides>
class multiarray_s : public multiarray<T, n_dims, ROW_MAJOR> {
  static_assert(std::is_pointer_v<T>, "Type T must be pointer");
  static_assert(sizeof...(strides) == n_dims,
                "Number of strides must be equal to n_dims");
  using T_no_pointer = typename std::remove_pointer<T>::type;

 public:
  std::string string_strides() const { return packNumToString<strides...>(); }

  // operator() overload for array access
  // calculate the offset in the linear array

  template <typename... Types>
  inline T_no_pointer &at_colm(Types... indices) {
    return this->pointer[get_offset_col(indices...)];
  }

  template <typename... Types>
  inline T_no_pointer &at_row(Types... indices) {
    return this->pointer[get_offset_row(indices...)];
  }

  template <typename... Types>
  inline T_no_pointer &operator()(Types... indices) {
    if constexpr (ROW_MAJOR)
      return this->pointer[get_offset_row(indices...)];
    else
      return this->pointer[get_offset_col(indices...)];
  }

  multiarray_s() : multiarray<T, n_dims, ROW_MAJOR>() {}

  multiarray_s(T _pointer) : multiarray<T, n_dims, ROW_MAJOR>(_pointer) {}

  multiarray_s(const multiarray_s<T, n_dims, ROW_MAJOR, strides...> &array)
      : multiarray<T, n_dims, ROW_MAJOR>(array) {}

  multiarray_s(const multiarray<T, n_dims, ROW_MAJOR> &array)
      : multiarray<T, n_dims, ROW_MAJOR>(array) {}

  template <typename... Types>
  multiarray_s(T _pointer, Types... _dims)
      : multiarray<T, n_dims, ROW_MAJOR>(_pointer, _dims...) {}

 private:
  template <typename Type0, typename... Types>
  inline dims_type get_offset_col(Type0 index, Types... indices) {
    constexpr dims_type i_dim = n_dims - sizeof...(indices) - 1;
    return static_cast<dims_type>(index) -
           get_i_of_pack_num<i_dim, strides...>() +
           std::get<i_dim>(this->dims) * get_offset_col(indices...);
  }

  template <typename Type0, typename... Types>
  inline dims_type get_offset_col(Type0 index) {
    constexpr dims_type i_dim = n_dims - 1;
    return static_cast<dims_type>(index) -
           get_i_of_pack_num<i_dim, strides...>();
  }

  template <typename Type0, typename... Types>
  inline dims_type get_offset_row(Type0 index, Types... indices) {
    return get_offset_row_sum(
        static_cast<dims_type>(index) - get_i_of_pack_num<0, strides...>(),
        indices...);
  }

  template <typename Type0, typename... Types>
  inline dims_type get_offset_row_sum(dims_type part_sum, Type0 index,
                                      Types... indices) {
    constexpr dims_type i_dim = n_dims - sizeof...(indices) - 1;
    return get_offset_row_sum(part_sum * std::get<i_dim>(this->dims) +
                                  static_cast<dims_type>(index) -
                                  get_i_of_pack_num<i_dim, strides...>(),
                              indices...);
  }

  template <typename Type0, typename... Types>
  inline dims_type get_offset_row_sum(dims_type part_sum, Type0 index) {
    constexpr dims_type i_dim = n_dims - 1;
    return part_sum * std::get<i_dim>(this->dims) +
           static_cast<dims_type>(index) -
           get_i_of_pack_num<i_dim, strides...>();
  }
};

}  // namespace ma