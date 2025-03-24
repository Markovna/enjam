#ifndef INCLUDE_ENJAM_TYPE_TRAITS_HELPERS_H_
#define INCLUDE_ENJAM_TYPE_TRAITS_HELPERS_H_

namespace Enjam {

// explicit deduction guide (not needed as of C++20)
template<class... Ts>
struct overloaded : Ts ... { using Ts::operator()...; };

template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

template<class T, class... Ts>
using is_same_with_any = std::disjunction<std::is_same<T, Ts>...>;

template<typename A, typename B = int, typename C = int, typename D = int>
using enable_if_arithmetic_t = std::enable_if_t<
      std::is_arithmetic<A>::value &&
      std::is_arithmetic<B>::value &&
      std::is_arithmetic<C>::value &&
      std::is_arithmetic<D>::value>;

template<typename T, typename U>
struct arithmetic_result {
  using type = decltype(std::declval<T>() + std::declval<U>());
};

template<typename T, typename U>
using arithmetic_result_t = typename arithmetic_result<T, U>::type;

}

#endif //INCLUDE_ENJAM_TYPE_TRAITS_HELPERS_H_
