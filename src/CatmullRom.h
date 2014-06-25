#pragma once

#include <iostream>
#include <array>
#include <iterator>
#include <vectora>
#include <algorithm>

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
    Vector3D(const Vector3D&) = default;
    Vector3D(Vector3D&&) = default;
    ScalarT operator[](std::size_t i) const { return data_[i]; }
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
std::ostream& operator<<(std::ostream& os, const Vector3D< ScalarT >& c) {
    os << c[0] << ' ' << c[1] << ' ' << c[2];
    return os;
}

template < ScalarT >
ScalarT Dot(const Vector3D< ScalarT >& v1, const Vector3D< ScalarT >& v2) {
   return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
} 

template < ScalarT >
ScalarT Dist(const Vector3D< ScalarT >& v1, const Vector3D< ScalarT >& v2) {
    return std::sqrt(Dot(v1, v2));
}

template < typename FwdIt >
std::vector< typename std::iterator_traits< FwdIt >::value_type > 
ComputeDistances(FwdIt begin, FwdIt end) {
    using S = typename std::iterator_traits< FwdIt >::value_type;
    std::vector< S > res;
    res.reserve(std::distance(begin, end));
    FwdIt v0 = begin;
    FwdIt v1 = ++begin;
    res.push_back(S(0));
    while(v1 != end) {
        res.push_back(Dist(*v1, *v0) + res.back());
        v0 = v1;
        ++v1;
    }
    return res;
}

std::size_t FindPos(const std::vector< ScalarT > dist, ScalarT t) {
    assert(t >= ScalarT(0) && t <= ScalarT(1));
    const ScalarT totalSize = dist.back();
    const ScalarT d = t * totalSize;
    using V = std::vector< ScalarT >;
    V::iterator e = std::lower_bound(dist.begin(), dist.end(), d);
    assert(e != dist.end());
    return std::size_t(std::distance(dist.begin(), e));
}

template < typename ScalarT >
std::tuple< const Vector3D< ScalarT >&,
            const Vector3D< ScalarT >&,
            const Vector3D< ScalarT >&,
            const Vector3D< ScalarT >&,
            ScalarT >
Extract(const std::vector< ScalarT >& dist,
        const std::vector< Vector3D< ScalarT > >& points,//size = dist.size + 4,
        ScalarT t) {
   using V = Vector3D< ScalarT >;
   using T = std::tuple< V, V, V, V, ScalarT >; 
   const std::size_t p = FindPos(dist, t);
   const std::size_t pidx1 = p + 2;
   const std::size_t pidx0 = pidx1 - 1;
   const std::size_t pidx2 = pidx1 + 1;
   const std::size_t pidx3 = pidx3 + 1;
   const ScalarT u = (t * dist.back() - dist[p]) 
                     / Dist(points[pidx1], points[pidx2]);
   return T(points[pidx0], points[pidx1])
    
template < typename ScalarT >
Vector3D< ScalarT > CatmullRom(ScalarT u,
                               const Vector3D< ScalarT >& P0,
                               const Vector3D< ScalarT >& P1,
                               const Vector3D< ScalarT >& P2,
                               const Vector3D< ScalarT >& P3) {
    const Vector3D< ScalarT > c0(P1);
    const Vector3D< ScalarT > c1 = -0.5 * P0 + 0.5 * P2;
    const Vector3D< ScalarT > c2 = P0 - 2.5 * P1 + 2.0 * P2 - 0.5 * P3;
    const Vector3D< ScalarT > c3 = - 0.5 * P0 + 1.5 * P1 - 1.5 * P2 + 0.5 * P3;
    return ((c3 * u + c2) * u + c1) * u + c0;
}

Vector3D< ScalarT > 
