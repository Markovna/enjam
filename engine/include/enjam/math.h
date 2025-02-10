#ifndef ENJAM_ENGINE_INCLUDE_ENJAM_STD140_H_
#define ENJAM_ENGINE_INCLUDE_ENJAM_STD140_H_

#include <cassert>
#include <cmath>
#include <enjam/type_traits_helpers.h>
#include <array>

namespace Enjam {

namespace math {

template<template<typename T> class VECTOR, typename T>
class VecAddOperators {
 public:
  template<typename U>
  constexpr VECTOR<T>& operator+=(const VECTOR<U>& v) {
    auto& lhs = static_cast<VECTOR<T>&>(*this);
    for (size_t i = 0; i < lhs.size(); i++) {
      lhs[i] += v[i];
    }
    return lhs;
  }

  template<typename U>
  friend inline constexpr
  VECTOR<arithmetic_result_t<T, U>> operator+(const VECTOR<T>& lv, const VECTOR<U>& rv) {
    VECTOR<arithmetic_result_t<T, U>> res(lv);
    res += rv;
    return res;
  }

  template<typename U, typename = enable_if_arithmetic_t<U>>
  friend inline constexpr
  VECTOR<arithmetic_result_t<T, U>> operator-(const VECTOR<T>& lv, U rv) {
    return lv - VECTOR<U>(rv);
  }
};

template<template<typename T> class VECTOR, typename T>
class VecUnaryOperators {
 public:
  constexpr VECTOR<T> operator-() const {
    VECTOR<T> r{};
    VECTOR<T> const& rv(static_cast<VECTOR<T> const&>(*this));
    for (size_t i = 0; i < r.size(); i++) {
      r[i] = -rv[i];
    }
    return r;
  }
};

template<template<typename T> class VECTOR, typename T>
class VecFunctions {
 private:
  template<typename U>
  friend constexpr inline
  arithmetic_result_t<T, U>
  dot(const VECTOR<T> &lv, const VECTOR<U> &rv) {
    arithmetic_result_t<T, U> r{};
    for (size_t i = 0; i < lv.size(); i++) {
      r += lv[i] * rv[i];
    }
    return r;
  }

  friend inline T
  norm(const VECTOR<T> &lv) {
    return std::sqrt(dot(lv, lv));
  }

  friend inline T
  length(const VECTOR<T> &lv) {
    return norm(lv);
  }

  friend inline VECTOR<T> normalize(const VECTOR<T>& lv) {
    return lv * (T(1) / length(lv));
  }
};

template<template<typename T> class VECTOR, typename T>
class VecProductOperators {
 public:
  template<typename U>
  constexpr VECTOR<T> &operator*=(const VECTOR<U> &v) {
    auto& lhs = static_cast<VECTOR<T>&>(*this);
    for (size_t i = 0; i < lhs.size(); i++) {
      lhs[i] *= v[i];
    }
    return lhs;
  }

 private:
  template<typename U>
  friend inline constexpr
  VECTOR<arithmetic_result_t<T, U>> operator*(const VECTOR<T>& lv, const VECTOR<U>& rv) {
    VECTOR<arithmetic_result_t<T, U>> res(lv);
    res *= rv;
    return res;
  }

  template<typename U, typename = enable_if_arithmetic_t<U>>
  friend inline constexpr
  VECTOR<arithmetic_result_t<T, U>> operator*(const VECTOR<T>& lv, U rv) {
    return lv * VECTOR<U>(rv);
  }

  template<typename U, typename = enable_if_arithmetic_t<U>>
  friend inline constexpr
  VECTOR<arithmetic_result_t<T, U>> operator*(U lv, const VECTOR<T>& rv) {
    return VECTOR<U>(lv) * rv;
  }
};

template<typename T>
class Vec2
    : public VecAddOperators<Vec2, T>
    , public VecFunctions<Vec2, T>
    , public VecUnaryOperators<Vec2, T>
    , public VecProductOperators<Vec2, T> {
 public:
  static constexpr size_t SIZE = 2;

  inline constexpr size_t size() const { return SIZE; }

  inline constexpr T const& operator[](size_t i) const noexcept {
    assert(i < SIZE);
    return v[i];
  }

  inline constexpr T& operator[](size_t i) noexcept {
    assert(i < SIZE);
    return v[i];
  }

  Vec2() = default;

  template<typename A, typename = enable_if_arithmetic_t<A>>
  constexpr Vec2(A v) noexcept : v{ T(v), T(v) } {}

  template<typename A, typename B, typename = enable_if_arithmetic_t<A, B>>
  constexpr Vec2(A x, B y) noexcept : v{ T(x), T(y) } {}

  template<typename A>
  constexpr Vec2(const Vec2<A>& v) noexcept : v{ T(v[0]), T(v[1]) } {}

  union {
    T v[SIZE];
    struct { T x, y; };
  };
};

template<typename T>
class Vec3
    : public VecAddOperators<Vec3, T>
    , public VecFunctions<Vec3, T>
    , public VecUnaryOperators<Vec3, T>
    , public VecProductOperators<Vec3, T>  {
 public:
  static constexpr size_t SIZE = 3;

  inline constexpr size_t size() const { return SIZE; }

  inline constexpr T const& operator[](size_t i) const noexcept {
    assert(i < SIZE);
    return v[i];
  }

  inline constexpr T& operator[](size_t i) noexcept {
    assert(i < SIZE);
    return v[i];
  }

  Vec3() = default;

  template<typename A>
  constexpr Vec3(A v) noexcept : v{ T(v), T(v), T(v) } {}

  template<typename A, typename B>
  constexpr Vec3(A x, B y) noexcept : v{ T(x), T(y) } {}

  template<typename A, typename B, typename C>
  constexpr Vec3(A x, B y, C z) noexcept : v{ T(x), T(y), T(z) } {}

  template<typename A>
  constexpr Vec3(const Vec3<A>& v) noexcept : v{ T(v[0]), T(v[1]), T(v[2]) } {}

  template<typename U>
  friend inline constexpr
  Vec3<arithmetic_result_t<T, U>> cross(const Vec3& u, const Vec3<U>& v) noexcept {
    return {
        u[1] * v[2] - u[2] * v[1],
        u[2] * v[0] - u[0] * v[2],
        u[0] * v[1] - u[1] * v[0] };
  }

  union {
    T v[SIZE];
    struct { T x, y, z; };
  };
};

template<typename T>
class Vec4
    : public VecAddOperators<Vec4, T>
    , public VecFunctions<Vec4, T>
    , public VecUnaryOperators<Vec4, T>
    , public VecProductOperators<Vec4, T> {
 public:
  static constexpr size_t SIZE = 4;

  inline constexpr size_t size() const { return SIZE; }

  inline constexpr T const& operator[](size_t i) const noexcept {
    assert(i < SIZE);
    return v[i];
  }

  inline constexpr T& operator[](size_t i) noexcept {
    assert(i < SIZE);
    return v[i];
  }

  Vec4() = default;

  template<typename A, typename = enable_if_arithmetic_t<A>>
  constexpr Vec4(A v) noexcept : v{ T(v), T(v), T(v), T(v) } {}

  template<typename A, typename B, typename = enable_if_arithmetic_t<A, B>>
  constexpr Vec4(A x, B y) noexcept : v{ T(x), T(y) } {}

  template<typename A, typename B, typename C, typename = enable_if_arithmetic_t<A, B, C>>
  constexpr Vec4(A x, B y, C z) noexcept : v{ T(x), T(y), T(z) } {}

  template<typename A, typename B, typename C, typename D>
  constexpr Vec4(A x, B y, C z, D w) noexcept : v{ T(x), T(y), T(z), T(w) } {}

  template<typename A>
  constexpr Vec4(const Vec3<A>& v) noexcept : v{ T(v[0]), T(v[1]), T(v[2]) } {}

  template<typename A, typename B>
  constexpr Vec4(const Vec3<A>& v, B w) noexcept : v{ T(v[0]), T(v[1]), T(v[2]), T(w) } {}

  template<typename A>
  constexpr Vec4(const Vec4<A>& v) noexcept : v{ T(v[0]), T(v[1]), T(v[2]), T(v[3]) } {}

  union {
    T v[SIZE];
    struct { T x, y, z, w; };
  };
};

template<typename T>
class Quaternion {
 public:
  static constexpr size_t SIZE = 4;

  template<typename A, typename B, typename = enable_if_arithmetic_t<A, B>>
  constexpr static Quaternion fromAxisAngle(const Vec3<A>& axis, B angle) {
    return Quaternion(std::sin(angle * 0.5) * normalize(axis), std::cos(angle * 0.5));
  }

  inline constexpr T const& operator[](size_t i) const noexcept {
    assert(i < SIZE);
    return v[i];
  }

  inline constexpr T& operator[](size_t i) noexcept {
    assert(i < SIZE);
    return v[i];
  }

  union {
    T v[SIZE];
    struct { T x, y, z, w; };
  };
};

template<typename T>
constexpr T epsilon() noexcept;

template<>
constexpr float epsilon<float>() noexcept { return 0.999f; }

template<>
constexpr double epsilon<double>() noexcept { return 0.9999; }

template<typename T>
class Mat33 {
 public:
  typedef Vec3<T> col_type;
  typedef Vec3<T> row_type;

  static constexpr size_t COL_SIZE = col_type::SIZE;
  static constexpr size_t ROW_SIZE = row_type::SIZE;
  static constexpr size_t NUM_ROWS = COL_SIZE;
  static constexpr size_t NUM_COLS = ROW_SIZE;

 public:
  inline constexpr col_type const& operator[](size_t column) const noexcept {
    assert(column < NUM_COLS);
    return values[column];
  }

  inline constexpr col_type& operator[](size_t column) noexcept {
    assert(column < NUM_COLS);
    return values[column];
  }

  constexpr Mat33() noexcept : values { {1, 0, 0}, {0, 1, 0}, {0, 0, 1}} {}

  template<typename U>
  constexpr explicit Mat33(U v) noexcept : values { {v, 0, 0}, {0, v, 0}, {0, 0, v}} {}

  template<typename U>
  constexpr explicit Mat33(const Vec3<U>& v) noexcept : values { {v[0], 0, 0}, {0, v[1], 0}, {0, 0, v[2]}} {}

  template<typename U>
  constexpr explicit Mat33(const Mat33<U>& rhs) noexcept : values { rhs.values } {}

  template<typename A, typename B, typename C>
  constexpr Mat33(const Vec3<A>& v0, const Vec3<B>& v1, const Vec3<C>& v2) noexcept
      : values { v0, v1, v2 } {}

  template<
      typename A, typename B, typename C,
      typename D, typename E, typename F,
      typename G, typename H, typename I>
  constexpr explicit Mat33(
      A m00, B m01, C m02,
      D m10, E m11, F m12,
      G m20, H m21, I m22) noexcept
      : values { {m00, m01, m02}, {m10, m11, m12}, {m20, m21, m22}} {}

  template<typename A, typename B>
  static Mat33<T> lookAt(const Vec3<A>& direction, const Vec3<B>& up) noexcept {
    auto const z_axis = direction;
    auto norm_up = up;
    if (std::abs(dot(z_axis, norm_up)) > epsilon<arithmetic_result_t<A, B>>()) {
      norm_up = { norm_up[2], norm_up[0], norm_up[1] };
    }

    auto const x_axis = normalize(cross(z_axis, norm_up));
    auto const y_axis = cross(x_axis, z_axis);
    return { x_axis, y_axis, -z_axis };
  }

 private:
  col_type values[NUM_COLS];
};

template<typename T>
class Mat44 {
 public:
  typedef Vec4<T> col_type;
  typedef Vec4<T> row_type;

  static constexpr size_t COL_SIZE = col_type::SIZE;
  static constexpr size_t ROW_SIZE = row_type::SIZE;
  static constexpr size_t NUM_ROWS = COL_SIZE;
  static constexpr size_t NUM_COLS = ROW_SIZE;

 public:
  inline constexpr col_type const& operator[](size_t column) const noexcept {
    assert(column < NUM_COLS);
    return values[column];
  }

  inline constexpr col_type& operator[](size_t column) noexcept {
    assert(column < NUM_COLS);
    return values[column];
  }

  constexpr Mat44() noexcept
      : values { {1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}} {}

  template<typename U>
  constexpr explicit Mat44(U v) noexcept
      : values { {v, 0, 0, 0}, {0, v, 0, 0}, {0, 0, v, 0}, {0, 0, 0, v}} {}

  template<typename U>
  constexpr explicit Mat44(const Vec4<U>& v) noexcept
      : values { {v[0], 0, 0, 0}, {0, v[1], 0, 0}, {0, 0, v[2], 0}, {0, 0, 0, v[3]}} {}

  template<typename U>
  constexpr explicit Mat44(const Mat44<U>& rhs) noexcept
      : values { rhs.values } {}

  template<typename A, typename B, typename C>
  constexpr Mat44(const Vec4<A>& v0, const Vec4<B>& v1, const Vec4<C>& v2, const Vec4<C>& v3) noexcept
      : values { v0, v1, v2, v3 } {}

  template<
      typename A, typename B, typename C, typename D,
      typename E, typename F, typename G, typename H,
      typename I, typename K, typename L, typename M,
      typename N, typename O, typename P, typename Q>
  constexpr explicit Mat44(
      A m00, B m01, C m02, D m03,
      E m10, F m11, G m12, H m13,
      I m20, K m21, L m22, M m23,
      N m30, O m31, P m32, Q m33) noexcept
      : values { {m00, m01, m02, m03},
                 {m10, m11, m12, m13 },
                 {m20, m21, m22, m23 },
                 {m30, m31, m32, m33 } } {}

  static Mat44 perspective(T fov, T aspect, T near, T far) noexcept {
    T h, w;

    w = std::tan(fov * M_PI / 360.0f) * near;
    h = w / aspect;
    return frustum(-w, w, -h, h, near, far);
  }

  static constexpr Mat44 frustum(T left, T right, T bottom, T top, T near, T far) noexcept {
    Mat44 m;
    m[0][0] = (2 * near) / (right - left);
    // 0
    // 0
    // 0

    // 0
    m[1][1] = (2 * near) / (top - bottom);
    // 0
    // 0

    m[2][0] = (right + left) / (right - left);
    m[2][1] = (top + bottom) / (top - bottom);
    m[2][2] = -(far + near) / (far - near);
    m[2][3] = -1;

    // 0
    // 0
    m[3][2] = -(2 * far * near) / (far - near);
    m[3][3] = 0;
    return m;
  }

  template<typename A, typename B, typename C>
  static Mat44 lookAt(const Vec3<A>& position, const Vec3<B>& direction, const Vec3<C>& up) noexcept {
    auto r = Mat33<T>::lookAt(direction, up);
    return Mat44 {
        Vec4<T>{ r[0], 0 },
        Vec4<T>{ r[1], 0 },
        Vec4<T>{ r[2], 0 },
        Vec4<T>{ position, 1 } };
  }

  template<typename A>
  static constexpr Mat44 translation(const Vec3<A>& t) noexcept {
    Mat44 r;
    r[3] = Vec4<T>{ t, 1 };
    return r;
  }

 private:
  col_type values[NUM_COLS];
};

using vec2f = Vec2<float>;
using vec3f = Vec3<float>;
using vec4f = Vec3<float>;
using vec2i = Vec2<int32_t>;
using vec3i = Vec3<int32_t>;
using vec4i = Vec3<int32_t>;
using mat3f = Mat33<float>;
using mat4f = Mat44<float>;
using quat = Quaternion<float>;

}

namespace std140 {

struct alignas(16) vec3 : public std::array<float, 3> {};
struct alignas(16) vec4 : public std::array<float, 4> {};

struct mat33 : public std::array<vec3, 3> {
  mat33 &operator=(math::mat3f const &rhs) noexcept {
    for (int i = 0; i < 3; i++) {
      (*this)[i][0] = rhs[i][0];
      (*this)[i][1] = rhs[i][1];
      (*this)[i][2] = rhs[i][2];
    }
    return *this;
  }
};

struct mat44 : public std::array<vec4, 4> {
  mat44 &operator=(math::mat4f const &rhs) noexcept {
    for (int i = 0; i < 4; i++) {
      (*this)[i][0] = rhs[i][0];
      (*this)[i][1] = rhs[i][1];
      (*this)[i][2] = rhs[i][2];
      (*this)[i][3] = rhs[i][3];
    }
    return *this;
  }
};

}

}

#endif //ENJAM_ENGINE_INCLUDE_ENJAM_STD140_H_
