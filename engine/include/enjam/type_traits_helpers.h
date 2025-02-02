#ifndef INCLUDE_ENJAM_TYPE_TRAITS_HELPERS_H_
#define INCLUDE_ENJAM_TYPE_TRAITS_HELPERS_H_

namespace Enjam {

// explicit deduction guide (not needed as of C++20)
template<class... Ts>
struct overloaded : Ts ... { using Ts::operator()...; };

template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

}

#endif //INCLUDE_ENJAM_TYPE_TRAITS_HELPERS_H_
