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
    VECTOR<T> r {};
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
  dot(const VECTOR<T>& lv, const VECTOR<U>& rv) {
    arithmetic_result_t<T, U> r {};
    for (size_t i = 0; i < lv.size(); i++) {
      r += lv[i] * rv[i];
    }
    return r;
  }

  friend inline T
  norm(const VECTOR<T>& lv) {
    return std::sqrt(dot(lv, lv));
  }

  friend inline T
  length(const VECTOR<T>& lv) {
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
  constexpr VECTOR<T>& operator*=(const VECTOR<U>& v) {
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
    : public VecAddOperators<Vec2, T>,
      public VecFunctions<Vec2, T>,
      public VecUnaryOperators<Vec2, T>,
      public VecProductOperators<Vec2, T> {
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
  constexpr Vec2(A v) noexcept : v { T(v), T(v) } {}

  template<typename A, typename B, typename = enable_if_arithmetic_t<A, B>>
  constexpr Vec2(A x, B y) noexcept : v { T(x), T(y) } {}

  template<typename A>
  constexpr Vec2(const Vec2<A>& v) noexcept : v { T(v[0]), T(v[1]) } {}

  union {
    T v[SIZE];
    struct { T x, y; };
  };
};

template<typename T>
class Vec3
    : public VecAddOperators<Vec3, T>,
      public VecFunctions<Vec3, T>,
      public VecUnaryOperators<Vec3, T>,
      public VecProductOperators<Vec3, T> {
 public:
  static constexpr size_t SIZE = 3;
  using value_type = T;

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
  constexpr Vec3(A v) noexcept : v { T(v), T(v), T(v) } {}

  template<typename A, typename B>
  constexpr Vec3(A x, B y) noexcept : v { T(x), T(y) } {}

  template<typename A, typename B, typename C>
  constexpr Vec3(A x, B y, C z) noexcept : v { T(x), T(y), T(z) } {}

  template<typename A>
  constexpr Vec3(const Vec3<A>& v) noexcept : v { T(v[0]), T(v[1]), T(v[2]) } {}

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
    Vec2<T> xy;
    struct {
      T x;
      struct { T y, z; };
    };
  };
};

template<typename T>
class Vec4
    : public VecAddOperators<Vec4, T>,
      public VecFunctions<Vec4, T>,
      public VecUnaryOperators<Vec4, T>,
      public VecProductOperators<Vec4, T> {
 public:
  static constexpr size_t SIZE = 4;
  using value_type = T;

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
  explicit constexpr Vec4(A v) noexcept : v { T(v), T(v), T(v), T(v) } {}

  template<typename A, typename B, typename = enable_if_arithmetic_t<A, B>>
  constexpr Vec4(A x, B y) noexcept : v { T(x), T(y) } {}

  template<typename A, typename B, typename C, typename = enable_if_arithmetic_t<A, B, C>>
  constexpr Vec4(A x, B y, C z) noexcept : v { T(x), T(y), T(z) } {}

  template<typename A, typename B, typename C, typename D>
  constexpr Vec4(A x, B y, C z, D w) noexcept : v { T(x), T(y), T(z), T(w) } {}

  template<typename A>
  constexpr Vec4(const Vec3<A>& v) noexcept : v { T(v.x), T(v.y), T(v.z), 0 } {}

  template<typename A, typename B>
  constexpr Vec4(const Vec3<A>& v, B w) noexcept : v { T(v.x), T(v.y), T(v.z), T(w) } {}

  template<typename A>
  constexpr Vec4(const Vec4<A>& v) noexcept : v { T(v.x), T(v.y), T(v.z), T(v.w) } {}

  union {
    T v[SIZE];
    union {
      Vec2<T> xy;
      Vec3<T> xyz;
      struct {
        T x, y, z, w;
      };
    };
  };
};

template<template<class> class TQuat, class T>
class QuatProductOperators {
  /* compound assignment from another quaternion of the same size but different
     * element type.
     */
  template<typename OTHER>
  constexpr TQuat<T>& operator*=(const TQuat<OTHER>& r) {
    TQuat<T>& q = static_cast<TQuat<T>&>(*this);
    q = q * r;
    return q;
  }

  /* compound assignment products by a scalar
   */
  template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
  constexpr TQuat<T>& operator*=(U v) {
    TQuat<T>& lhs = static_cast<TQuat<T>&>(*this);
    for (size_t i = 0; i < TQuat<T>::size(); i++) {
      lhs[i] *= v;
    }
    return lhs;
  }

  template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
  constexpr TQuat<T>& operator/=(U v) {
    TQuat<T>& lhs = static_cast<TQuat<T>&>(*this);
    for (size_t i = 0; i < TQuat<T>::size(); i++) {
      lhs[i] /= v;
    }
    return lhs;
  }

  /*
   * NOTE: the functions below ARE NOT member methods. They are friend functions
   * with they definition inlined with their declaration. This makes these
   * template functions available to the compiler when (and only when) this class
   * is instantiated, at which point they're only templated on the 2nd parameter
   * (the first one, BASE<T> being known).
   */

  /* The operators below handle operation between quaternions of the same size
   * but of a different element type.
   */
  template<typename U>
  friend inline constexpr
  TQuat<arithmetic_result_t<T, U>>  operator*(const TQuat<T>& q, const TQuat<U>& r) {
    // could be written as:
    //  return TQuat<T>(
    //            q.w*r.w - dot(q.xyz, r.xyz),
    //            q.w*r.xyz + r.w*q.xyz + cross(q.xyz, r.xyz));
    return {
        q.w * r.w - q.x * r.x - q.y * r.y - q.z * r.z,
        q.w * r.x + q.x * r.w + q.y * r.z - q.z * r.y,
        q.w * r.y - q.x * r.z + q.y * r.w + q.z * r.x,
        q.w * r.z + q.x * r.y - q.y * r.x + q.z * r.w
    };
  }

  template<typename U>
  friend inline constexpr
  Vec3<arithmetic_result_t<T, U>>  operator*(const TQuat<T>& q, const Vec3<U>& v) {
    // note: if q is known to be a unit quaternion, then this simplifies to:
    //  TVec3<T> t = 2 * cross(q.xyz, v)
    //  return v + (q.w * t) + cross(q.xyz, t)
    return imaginary(q * TQuat<U>(v, 0) * inverse(q));
  }


  /* For quaternions, we use explicit "by a scalar" products because it's much faster
   * than going (implicitly) through the quaternion multiplication.
   * For reference: we could use the code below instead, but it would be a lot slower.
   *  friend inline
   *  constexpr BASE<T> MATH_PURE operator *(const BASE<T>& q, const BASE<T>& r) {
   *      return BASE<T>(
   *              q.w*r.w - q.x*r.x - q.y*r.y - q.z*r.z,
   *              q.w*r.x + q.x*r.w + q.y*r.z - q.z*r.y,
   *              q.w*r.y - q.x*r.z + q.y*r.w + q.z*r.x,
   *              q.w*r.z + q.x*r.y - q.y*r.x + q.z*r.w);
   *
   */
  template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
  friend inline constexpr
  TQuat<arithmetic_result_t<T, U>>  operator*(TQuat<T> q, U scalar) {
    // don't pass q by reference because we need a copy anyway
    return TQuat<arithmetic_result_t<T, U>>(q *= scalar);
  }

  template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
  friend inline constexpr
  TQuat<arithmetic_result_t<T, U>>  operator*(U scalar, TQuat<T> q) {
    // don't pass q by reference because we need a copy anyway
    return TQuat<arithmetic_result_t<T, U>>(q *= scalar);
  }

  template<typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
  friend inline constexpr
  TQuat<arithmetic_result_t<T, U>>  operator/(TQuat<T> q, U scalar) {
    // don't pass q by reference because we need a copy anyway
    return TQuat<arithmetic_result_t<T, U>>(q /= scalar);
  }
};

template<typename T>
class Quaternion : public QuatProductOperators<Quaternion, T> {
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

namespace matrix {
//------------------------------------------------------------------------------
// This is taken from the Imath MatrixAlgo code, and is identical to Eigen.
//template<typename TMat>
//Quaternion<typename TMat::value_type> extractQuat(const TMat& mat) {
//  typedef typename TMat::value_type T;
//
//  Quaternion<T> quat(Quaternion<T>::NO_INIT);
//
//  // Compute the trace to see if it is positive or not.
//  const T trace = mat[0][0] + mat[1][1] + mat[2][2];
//
//  // check the sign of the trace
//  if (MATH_LIKELY(trace > 0)) {
//    // trace is positive
//    T s = std::sqrt(trace + 1);
//    quat.w = T(0.5) * s;
//    s = T(0.5) / s;
//    quat.x = (mat[1][2] - mat[2][1]) * s;
//    quat.y = (mat[2][0] - mat[0][2]) * s;
//    quat.z = (mat[0][1] - mat[1][0]) * s;
//  } else {
//    // trace is negative
//
//    // Find the index of the greatest diagonal
//    size_t i = 0;
//    if (mat[1][1] > mat[0][0]) { i = 1; }
//    if (mat[2][2] > mat[i][i]) { i = 2; }
//
//    // Get the next indices: (n+1)%3
//    static constexpr size_t next_ijk[3] = { 1, 2, 0 };
//    size_t j = next_ijk[i];
//    size_t k = next_ijk[j];
//    T s = std::sqrt((mat[i][i] - (mat[j][j] + mat[k][k])) + 1);
//    quat[i] = T(0.5) * s;
//    if (s != 0) {
//      s = T(0.5) / s;
//    }
//    quat.w  = (mat[j][k] - mat[k][j]) * s;
//    quat[j] = (mat[i][j] + mat[j][i]) * s;
//    quat[k] = (mat[i][k] + mat[k][i]) * s;
//  }
//  return quat;
//}

template<typename MATRIX_R, typename MATRIX_A, typename MATRIX_B,
    typename = std::enable_if_t<
        MATRIX_A::NUM_COLS == MATRIX_B::NUM_ROWS &&
            MATRIX_R::NUM_COLS == MATRIX_B::NUM_COLS &&
            MATRIX_R::NUM_ROWS == MATRIX_A::NUM_ROWS, int>>
constexpr MATRIX_R multiply(MATRIX_A lhs, MATRIX_B rhs) {
  // pre-requisite:
  //  lhs : D columns, R rows
  //  rhs : C columns, D rows
  //  res : C columns, R rows
  MATRIX_R res {};
  for (size_t col = 0; col < MATRIX_R::NUM_COLS; ++col) {
    res[col] = lhs * rhs[col];
  }
  return res;
}

//------------------------------------------------------------------------------
// 2x2 matrix inverse is easy.
template<typename MATRIX>
constexpr MATRIX fastInverse2(const MATRIX& x) {
  typedef typename MATRIX::value_type T;

  // Assuming the input matrix is:
  // | a b |
  // | c d |
  //
  // The analytic inverse is
  // | d -b |
  // | -c a | / (a d - b c)
  //
  // Importantly, our matrices are column-major!

  MATRIX inverted{};

  const T a = x[0][0];
  const T c = x[0][1];
  const T b = x[1][0];
  const T d = x[1][1];

  const T det((a * d) - (b * c));
  inverted[0][0] =  d / det;
  inverted[0][1] = -c / det;
  inverted[1][0] = -b / det;
  inverted[1][1] =  a / det;
  return inverted;
}

/*
 * Matrix inversion
 */
template<typename MATRIX>
constexpr MATRIX gaussJordanInverse(MATRIX src) {
  typedef typename MATRIX::value_type T;
  constexpr unsigned int N = MATRIX::NUM_ROWS;
  MATRIX inverted;

  for (size_t i = 0; i < N; ++i) {
    // look for largest element in i'th column
    size_t swap = i;
    T t = src[i][i] < 0 ? -src[i][i] : src[i][i];
    for (size_t j = i + 1; j < N; ++j) {
      const T t2 = src[j][i] < 0 ? -src[j][i] : src[j][i];
      if (t2 > t) {
        swap = j;
        t = t2;
      }
    }

    if (swap != i) {
      // swap columns.
      std::swap(src[i], src[swap]);
      std::swap(inverted[i], inverted[swap]);
    }

    const T denom(src[i][i]);
    for (size_t k = 0; k < N; ++k) {
      src[i][k] /= denom;
      inverted[i][k] /= denom;
    }

    // Factor out the lower triangle
    for (size_t j = 0; j < N; ++j) {
      if (j != i) {
        const T t = src[j][i];
        for (size_t k = 0; k < N; ++k) {
          src[j][k] -= src[i][k] * t;
          inverted[j][k] -= inverted[i][k] * t;
        }
      }
    }
  }

  return inverted;
}

//------------------------------------------------------------------------------
// From the Wikipedia article on matrix inversion's section on fast 3x3
// matrix inversion:
// http://en.wikipedia.org/wiki/Invertible_matrix#Inversion_of_3.C3.973_matrices
template<typename MATRIX>
constexpr MATRIX fastInverse3(const MATRIX& x) {
  typedef typename MATRIX::value_type T;

  // Assuming the input matrix is:
  // | a b c |
  // | d e f |
  // | g h i |
  //
  // The analytic inverse is
  // | A B C |^T
  // | D E F |
  // | G H I | / determinant
  //
  // Which is
  // | A D G |
  // | B E H |
  // | C F I | / determinant
  //
  // Where:
  // A = (ei - fh), B = (fg - di), C = (dh - eg)
  // D = (ch - bi), E = (ai - cg), F = (bg - ah)
  // G = (bf - ce), H = (cd - af), I = (ae - bd)
  //
  // and the determinant is a*A + b*B + c*C (The rule of Sarrus)
  //
  // Importantly, our matrices are column-major!

  MATRIX inverted{};

  const T a = x[0][0];
  const T b = x[1][0];
  const T c = x[2][0];
  const T d = x[0][1];
  const T e = x[1][1];
  const T f = x[2][1];
  const T g = x[0][2];
  const T h = x[1][2];
  const T i = x[2][2];

  // Do the full analytic inverse
  const T A = e * i - f * h;
  const T B = f * g - d * i;
  const T C = d * h - e * g;
  inverted[0][0] = A;                 // A
  inverted[0][1] = B;                 // B
  inverted[0][2] = C;                 // C
  inverted[1][0] = c * h - b * i;     // D
  inverted[1][1] = a * i - c * g;     // E
  inverted[1][2] = b * g - a * h;     // F
  inverted[2][0] = b * f - c * e;     // G
  inverted[2][1] = c * d - a * f;     // H
  inverted[2][2] = a * e - b * d;     // I

  const T det(a * A + b * B + c * C);
  for (size_t col = 0; col < 3; ++col) {
    for (size_t row = 0; row < 3; ++row) {
      inverted[col][row] /= det;
    }
  }

  return inverted;
}

template<typename MATRIX,
    typename = std::enable_if_t<MATRIX::NUM_ROWS == MATRIX::NUM_COLS, int>>
inline constexpr MATRIX inverse(const MATRIX& matrix) {
  return (MATRIX::NUM_ROWS == 2) ? fastInverse2<MATRIX>(matrix) :
         ((MATRIX::NUM_ROWS == 3) ? fastInverse3<MATRIX>(matrix) :
          gaussJordanInverse<MATRIX>(matrix));
}

}

template<template<class> class TMat, class T>
class TMatSquareFunctions {
 private:
  friend inline constexpr TMat<T> transpose(TMat<T> m) {
    TMat<T> result {};
    for (size_t col = 0; col < TMat<T>::NUM_COLS; ++col) {
      for (size_t row = 0; row < TMat<T>::NUM_ROWS; ++row) {
        result[col][row] = m[row][col];
      }
    }
    return result;
  }

  friend inline constexpr TMat<T> inverse(const TMat<T>& m) {
    return matrix::inverse(m);
  }
};

template<template<class> class TMat, class T>
class TMatTransform {
 public:
  template<class A, class TVec, class = enable_if_arithmetic_t<A>>
  static TMat<T> rotation(A radian, TVec about) {
    TMat<T> r;
    T c = std::cos(radian);
    T s = std::sin(radian);
    if (about[0] == 1 && about[1] == 0 && about[2] == 0) {
      r[1][1] = c;   r[2][2] = c;
      r[1][2] = s;   r[2][1] = -s;
    } else if (about[0] == 0 && about[1] == 1 && about[2] == 0) {
      r[0][0] = c;   r[2][2] = c;
      r[2][0] = s;   r[0][2] = -s;
    } else if (about[0] == 0 && about[1] == 0 && about[2] == 1) {
      r[0][0] = c;   r[1][1] = c;
      r[0][1] = s;   r[1][0] = -s;
    } else {
      TVec nabout = normalize(about);
      typename TVec::value_type x = nabout[0];
      typename TVec::value_type y = nabout[1];
      typename TVec::value_type z = nabout[2];
      T nc = 1 - c;
      T xy = x * y;
      T yz = y * z;
      T zx = z * x;
      T xs = x * s;
      T ys = y * s;
      T zs = z * s;
      r[0][0] = x*x*nc +  c;    r[1][0] =  xy*nc - zs;    r[2][0] =  zx*nc + ys;
      r[0][1] =  xy*nc + zs;    r[1][1] = y*y*nc +  c;    r[2][1] =  yz*nc - xs;
      r[0][2] =  zx*nc - ys;    r[1][2] =  yz*nc + xs;    r[2][2] = z*z*nc +  c;

      // Clamp results to -1, 1.
      for (size_t col = 0; col < 3; ++col) {
        for (size_t row = 0; row < 3; ++row) {
          r[col][row] = std::min(std::max(r[col][row], T(-1)), T(1));
        }
      }
    }
    return r;
  }

//  Quaternion<T> toQuaternion() const {
//    return matrix::extractQuat(static_cast<const TMat<T>&>(*this));
//  }
};

template<template<class> class BASE, class T, template<class> class VEC>
class TMatProductOperators {
 public:
  //  matrix *= matrix
  template<typename U>
  constexpr BASE<T>& operator*=(const BASE<U>& rhs) {
    BASE<T>& lhs(static_cast< BASE<T>& >(*this));
    lhs = matrix::multiply<BASE<T>>(lhs, rhs);
    return lhs;
  }

  // matrix *= scalar
  template<typename U, typename = enable_if_arithmetic_t<U>>
  constexpr BASE<T>& operator*=(U v) {
    BASE<T>& lhs(static_cast< BASE<T>& >(*this));
    for (size_t col = 0; col < BASE<T>::NUM_COLS; ++col) {
      lhs[col] *= v;
    }
    return lhs;
  }

  // matrix /= scalar
  template<typename U, typename = enable_if_arithmetic_t<U>>
  constexpr BASE<T>& operator/=(U v) {
    BASE<T>& lhs(static_cast< BASE<T>& >(*this));
    for (size_t col = 0; col < BASE<T>::NUM_COLS; ++col) {
      lhs[col] /= v;
    }
    return lhs;
  }

 private:
  /*
   * NOTE: the functions below ARE NOT member methods. They are friend functions
   * with they definition inlined with their declaration. This makes these
   * template functions available to the compiler when (and only when) this class
   * is instantiated, at which point they're only templated on the 2nd parameter
   * (the first one, BASE<T> being known).
   */

  //  matrix * matrix
  template<typename U>
  friend inline constexpr BASE<arithmetic_result_t<T, U>>
  operator*(BASE<T> lhs, BASE<U> rhs) {
    return matrix::multiply<BASE<arithmetic_result_t<T, U>>>(lhs, rhs);
  }

  // matrix * vector
  template<typename U>
  friend inline constexpr typename BASE<arithmetic_result_t<T, U>>::col_type
  operator*(const BASE<T>& lhs, const VEC<U>& rhs) {
    typename BASE<arithmetic_result_t<T, U>>::col_type result{};
    for (size_t col = 0; col < BASE<T>::NUM_COLS; ++col) {
      result += lhs[col] * rhs[col];
    }
    return result;
  }

  // row-vector * matrix
  template<typename U>
  friend inline constexpr typename BASE<arithmetic_result_t<T, U>>::row_type
  operator*(const VEC<U>& lhs, const BASE<T>& rhs) {
    typename BASE<arithmetic_result_t<T, U>>::row_type result{};
    for (size_t col = 0; col < BASE<T>::NUM_COLS; ++col) {
      result[col] = dot(lhs, rhs[col]);
    }
    return result;
  }

  // matrix * scalar
  template<typename U, typename = enable_if_arithmetic_t <U>>
  friend inline constexpr BASE<arithmetic_result_t < T, U>>
  operator*(const BASE<T>& lhs, U rhs) {
    BASE<arithmetic_result_t<T, U>> result{};
    for (size_t col = 0; col < BASE<T>::NUM_COLS; ++col) {
      result[col] = lhs[col] * rhs;
    }
    return result;
  }

  // scalar * matrix
  template<typename U, typename = enable_if_arithmetic_t<U>>
  friend inline constexpr BASE<arithmetic_result_t<T, U>>
  operator*(U rhs, const BASE<T>& lhs) {
    return lhs * rhs;
  }

  // matrix / scalar
  template<typename U, typename = enable_if_arithmetic_t<U>>
  friend inline constexpr BASE<arithmetic_result_t<T, U>>
  operator/(const BASE<T>& lhs, U rhs) {
    BASE<arithmetic_result_t<T, U>> result{};
    for (size_t col = 0; col < BASE<T>::NUM_COLS; ++col) {
      result[col] = lhs[col] / rhs;
    }
    return result;
  }
};

template<typename T>
class Mat33 :
    public TMatSquareFunctions<Mat33, T>,
    public TMatTransform<Mat33, T>,
    public TMatProductOperators<Mat33, T, Vec3>,
    public VecAddOperators<Mat33, T> {
 public:
  typedef T value_type;
  typedef Vec3<T> col_type;
  typedef Vec3<T> row_type;

  static constexpr size_t COL_SIZE = col_type::SIZE;
  static constexpr size_t ROW_SIZE = row_type::SIZE;
  static constexpr size_t NUM_ROWS = COL_SIZE;
  static constexpr size_t NUM_COLS = ROW_SIZE;

 public:
  constexpr inline size_t size() const { return ROW_SIZE; }

  inline constexpr col_type const& operator[](size_t column) const noexcept {
    assert(column < NUM_COLS);
    return values[column];
  }

  inline constexpr col_type& operator[](size_t column) noexcept {
    assert(column < NUM_COLS);
    return values[column];
  }

  constexpr Mat33() noexcept: values { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } } {}

  template<typename U>
  constexpr explicit Mat33(U v) noexcept : values { { v, 0, 0 }, { 0, v, 0 }, { 0, 0, v } } {}

  template<typename U>
  constexpr explicit Mat33(const Vec3<U>& v) noexcept : values { { v[0], 0, 0 }, { 0, v[1], 0 }, { 0, 0, v[2] } } {}

  template<typename U>
  constexpr explicit Mat33(const Mat33<U>& rhs) noexcept {
    for (size_t col = 0; col < NUM_COLS; ++col) {
      values[col] = col_type(rhs[col]);
    }
  }

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
      : values { { m00, m01, m02 }, { m10, m11, m12 }, { m20, m21, m22 } } {}

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
class Mat44 :
    public TMatSquareFunctions<Mat44, T>,
    public TMatTransform<Mat44, T>,
    public TMatProductOperators<Mat44, T, Vec4>,
    public VecAddOperators<Mat44, T> {
 public:
  typedef T value_type;
  typedef Vec4<T> col_type;
  typedef Vec4<T> row_type;

  static constexpr size_t COL_SIZE = col_type::SIZE;
  static constexpr size_t ROW_SIZE = row_type::SIZE;
  static constexpr size_t NUM_ROWS = COL_SIZE;
  static constexpr size_t NUM_COLS = ROW_SIZE;

 public:
  constexpr inline size_t size() const { return ROW_SIZE; }

  inline constexpr col_type const& operator[](size_t column) const noexcept {
    assert(column < NUM_COLS);
    return values[column];
  }

  inline constexpr col_type& operator[](size_t column) noexcept {
    assert(column < NUM_COLS);
    return values[column];
  }

  constexpr Mat44() noexcept
      : values { { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 0, 0, 0, 1 } } {}

  template<typename U>
  constexpr explicit Mat44(U v) noexcept
      : values { { v, 0, 0, 0 }, { 0, v, 0, 0 }, { 0, 0, v, 0 }, { 0, 0, 0, v } } {}

  template<typename U>
  constexpr explicit Mat44(const Vec4<U>& v) noexcept
      : values { { v.x, 0, 0, 0 }, { 0, v.y, 0, 0 }, { 0, 0, v.z, 0 }, { 0, 0, 0, v.w } } {}

  template<typename U>
  constexpr explicit Mat44(const Mat44<U>& rhs) noexcept {
    for (size_t col = 0; col < NUM_COLS; ++col) {
      values[col] = col_type(rhs[col]);
    }
  }

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
      : values { { m00, m01, m02, m03 },
                 { m10, m11, m12, m13 },
                 { m20, m21, m22, m23 },
                 { m30, m31, m32, m33 } } {}

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
        Vec4<T> { r[0], 0 },
        Vec4<T> { r[1], 0 },
        Vec4<T> { r[2], 0 },
        Vec4<T> { position, 1 } };
  }

  template<typename A>
  static constexpr Mat44 translation(const Vec3<A>& t) noexcept {
    Mat44 r;
    r[3] = Vec4<T> { t, 1 };
    return r;
  }

 private:
  col_type values[NUM_COLS];
};

using vec2f = Vec2<float>;
using vec3f = Vec3<float>;
using vec4f = Vec4<float>;
using vec2i = Vec2<int32_t>;
using vec3i = Vec3<int32_t>;
using vec4i = Vec4<int32_t>;
using mat3f = Mat33<float>;
using mat4f = Mat44<float>;
using quat = Quaternion<float>;

}

namespace std140 {

struct alignas(16) vec3 : public std::array<float, 3> {};
struct alignas(16) vec4 : public std::array<float, 4> {};

struct mat33 : public std::array<vec3, 3> {
  mat33& operator=(math::mat3f const& rhs) noexcept {
    for (int i = 0; i < 3; i++) {
      (*this)[i][0] = rhs[i][0];
      (*this)[i][1] = rhs[i][1];
      (*this)[i][2] = rhs[i][2];
    }
    return *this;
  }
};

struct mat44 : public std::array<vec4, 4> {
  mat44& operator=(math::mat4f const& rhs) noexcept {
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
