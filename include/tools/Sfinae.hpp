#ifndef ALED_SFINAE_H
#define ALED_SFINAE_H

#include <utility>

namespace aled {

template <typename UnnamedType>
class container {
private:
  // We use std::declval to 'recreate' an object of 'UnnamedType'.
  // We use std::declval to also 'recreate' an object of type 'Param'.
  // We can use both of these recreated objects to test the validity!
  template <typename Param>
  constexpr auto testValidity(int) -> // int for precedence
  decltype(std::declval<UnnamedType>()(std::declval<Param>()), std::true_type()) {
    // If the substitution didn't fail return true_type
    return std::true_type();
  }

  template <typename Param>
  constexpr std::false_type testValidity(...) {
    // Sink hole returns false type
    return std::false_type();
  }

public:
  template <typename Param>
  constexpr auto operator()(const Param& p) {
    // The argument is forwarded to one of the two overloads.
    // The SFINAE on the 'true_type' will come into play to dispatch.
    // Once again, we use the int for the precedence.
    return testValidity<Param>(int());
  }
};

template <typename UnnamedType>
constexpr auto is_valid(const UnnamedType& t) {
  return container<UnnamedType>();
}

//
auto has_operator_eq = is_valid([](auto&& x) -> decltype(x.operator==(std::declval<decltype(x)>())) {});
auto has_operator_ne = is_valid([](auto&& x) -> decltype(x.operator!=(std::declval<decltype(x)>())) {});
auto has_operator_lt = is_valid([](auto&& x) -> decltype(x.operator<(std::declval<decltype(x)>())) {});
auto has_operator_le = is_valid([](auto&& x) -> decltype(x.operator<=(std::declval<decltype(x)>())) {});
auto has_operator_gt = is_valid([](auto&& x) -> decltype(x.operator>(std::declval<decltype(x)>())) {});
auto has_operator_ge = is_valid([](auto&& x) -> decltype(x.operator>=(std::declval<decltype(x)>())) {});
auto has_operator_plus  = is_valid([](auto&& x) -> decltype(x.operator+(std::declval<decltype(x)>())){});
auto has_operator_minus = is_valid([](auto&& x) -> decltype(x.operator-(std::declval<decltype(x)>())){});

template <class T>
struct is_scalar_object : std::integral_constant<bool,
  decltype(has_operator_plus(std::declval<T>()))::value  &&
  decltype(has_operator_minus(std::declval<T>()))::value > {};

template <class T, class Enable = void>
struct is_pseudo_scalar : std::false_type {};

template <class T>
struct is_pseudo_scalar<T, typename std::enable_if<is_scalar_object<T>::value>::type> :
  std::integral_constant<bool,
    std::is_same<T, decltype(std::declval<T>().operator+(std::declval<T>()))>::value &&
    std::is_same<T, decltype(std::declval<T>().operator-(std::declval<T>()))>::value &&
    std::is_same<T, decltype(std::declval<T>().operator=(std::declval<T>()))>::value> {};

// Comparable means ==, !=, <, >
template <class T>
struct is_comparable_object : std::integral_constant<bool,
   decltype(has_operator_eq(std::declval<T>()))::value &&
   decltype(has_operator_ne(std::declval<T>()))::value &&
   decltype(has_operator_lt(std::declval<T>()))::value &&
   decltype(has_operator_gt(std::declval<T>()))::value > {};


template <class T, class Enable = void>
struct is_comparable : std::false_type {};

template <class T>
struct is_comparable<T, typename std::enable_if<is_comparable_object<T>::value>::type> :
  std::integral_constant<bool,
    std::is_same<bool, decltype(std::declval<T>().operator==(std::declval<T>()))>::value &&
    std::is_same<bool, decltype(std::declval<T>().operator!=(std::declval<T>()))>::value &&
    std::is_same<bool, decltype(std::declval<T>().operator<(std::declval<T>()))>::value  &&
    std::is_same<bool, decltype(std::declval<T>().operator>(std::declval<T>()))>::value  > {};


// Fully comparable means ==, !=, < , >, <=, >=
template <class T>
struct is_fully_comparable_object : std::integral_constant<bool,
   decltype(has_operator_eq(std::declval<T>()))::value &&
   decltype(has_operator_ne(std::declval<T>()))::value &&
   decltype(has_operator_lt(std::declval<T>()))::value &&
   decltype(has_operator_gt(std::declval<T>()))::value &&
   decltype(has_operator_le(std::declval<T>()))::value &&
   decltype(has_operator_ge(std::declval<T>()))::value > {};

template <class T, class Enable = void>
struct is_fully_comparable : std::false_type {};

template <class T>
struct is_fully_comparable<T, typename std::enable_if<is_comparable_object<T>::value>::type> :
  std::integral_constant<bool,
    std::is_same<bool, decltype(std::declval<T>().operator==(std::declval<T>()))>::value &&
    std::is_same<bool, decltype(std::declval<T>().operator!=(std::declval<T>()))>::value &&
    std::is_same<bool, decltype(std::declval<T>().operator<(std::declval<T>()))>::value  &&
    std::is_same<bool, decltype(std::declval<T>().operator>(std::declval<T>()))>::value  &&
    std::is_same<bool, decltype(std::declval<T>().operator<=(std::declval<T>()))>::value &&
    std::is_same<bool, decltype(std::declval<T>().operator>=(std::declval<T>()))>::value > {};



}

#endif
