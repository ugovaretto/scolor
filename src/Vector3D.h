#pragma once
#include <array>
#include <iostream>
#include <cmath>

//------------------------------------------------------------------------------
template < typename ScalarT >
struct Vector3D {
    using Scalar = ScalarT;
    std::array< ScalarT, 3 > data_;
    Vector3D(ScalarT r = ScalarT(0),
             ScalarT g = ScalarT(0),
             ScalarT b = ScalarT(0)) {
        data_[0] = r;
        data_[1] = g;
        data_[2] = b;
    }
    Vector3D(int c) { 
        data_[0] = ScalarT(c & 0xFF) / ScalarT(255.0);
        data_[1] = ScalarT((c >> 8) & 0xFF) / ScalarT(255.0);
        data_[2] = ScalarT((c >> 16) & 0xFF) / ScalarT(255.0);
    }
    Vector3D(const Vector3D&) = default;
    //Vector3D(Vector3D&&) = default;
    ScalarT operator[](std::size_t i) const { return data_[i]; }
    ScalarT& operator[](std::size_t i) { return data_[i]; }
};

template < typename ScalarT >
Vector3D< ScalarT > operator+(const Vector3D< ScalarT >& c1,
                           const Vector3D< ScalarT >& c2) {
    return Vector3D< ScalarT >(c1[0] + c2[0], c1[1] + c2[1], c1[2] + c2[2]);
}

template < typename ScalarT >
Vector3D< ScalarT > operator-(const Vector3D< ScalarT >& c1,
                           const Vector3D< ScalarT >& c2) {
    return Vector3D< ScalarT >(c1[0] - c2[0], c1[1] - c2[1], c1[2] - c2[2]);
}

template < typename ScalarT > 
Vector3D< ScalarT > operator*(ScalarT s, const Vector3D< ScalarT >& c) {
    return Vector3D< ScalarT >(s * c[0], s * c[1], s * c[2]);
}

template < typename ScalarT > 
Vector3D< ScalarT > operator*(const Vector3D< ScalarT >& c, ScalarT s) {
    return Vector3D< ScalarT >(s * c[0], s * c[1], s * c[2]);
}

template < typename ScalarT > 
Vector3D< ScalarT > operator/(const Vector3D< ScalarT >& c, ScalarT s) {
    return Vector3D< ScalarT >(c[0] / s, c[1] / s, c[2] / s);
}

template < typename ScalarT >
std::ostream& operator<<(std::ostream& os, const Vector3D< ScalarT >& c) {
    os << c[0] << ' ' << c[1] << ' ' << c[2];
    return os;
}

template < typename ScalarT >
ScalarT Dot(const Vector3D< ScalarT >& v1, const Vector3D< ScalarT >& v2) {
   return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
} 

template < typename ScalarT >
ScalarT Dist(const Vector3D< ScalarT >& v1, const Vector3D< ScalarT >& v2) {
    return std::sqrt(Dot(v1, v2));
}
