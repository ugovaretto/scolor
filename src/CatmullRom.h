#pragma once

#include <iterator>
#include <vector>
#include <algorithm>
#include <tuple>
#include <cmath>
#include <cassert>

#include "Vector3D.h"

//------------------------------------------------------------------------------
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

template < typename ScalarT >
std::size_t FindPos(const std::vector< ScalarT > dist, ScalarT t) {
    assert(t >= ScalarT(0) && t <= ScalarT(1));
    const ScalarT totalSize = dist.back();
    const ScalarT d = t * totalSize;
    using V = std::vector< ScalarT >;
    typename V::iterator e = std::lower_bound(dist.begin(), dist.end(), d);
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
   using T = std::tuple< const V&, const V&, const V&, const V&, ScalarT >;
   const std::size_t p = FindPos(dist, t);
   const std::size_t pidx1 = p + 2;
   const std::size_t pidx0 = pidx1 - 1;
   const std::size_t pidx2 = pidx1 + 1;
   const std::size_t pidx3 = pidx3 + 1;
   const ScalarT u = (t * dist.back() - dist[p]) 
                     / Dist(points[pidx1], points[pidx2]);
   return T(points[pidx0], points[pidx1], points[pidx2], points[pidx3], u);
}

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
template < typename ScalarT > Vector3D< ScalarT >
CRomInterpolation(const std::vector< Vector3D< ScalarT > >& points,
                  const std::vector< ScalarT >& dist,
                  ScalarT t) {
    assert(points.size());
    assert(dist.size());
    assert(t >= ScalarT(0) && t <= ScalarT(1));
    assert(points.size() == dist.size() + 4);
    using V = Vector3D< ScalarT >;
    using T = std::tuple< const V&, const V&, const V&, const V&, ScalarT >; 
    const T p = Extract(dist, points, t);
    return CatmullRom(t, std::get<0>(p), 
                      std::get<1>(p), std::get<2>(p), std::get<3>(p), 
                      std::get<4>(p));
}
//------------------------------------------------------------------------------
template < typename ScalarT > Vector3D< ScalarT >
KeyFramedCRomInterpolation(const std::vector< Vector3D< ScalarT > >& points,
                           const std::vector< ScalarT >& keys,
                           ScalarT t) {
    assert(points.size());
    assert(keys.size());
    assert(points.size() == keys.size());
    using V = Vector3D< ScalarT >;
    typename V::const_iterator i = 
                                  std::lower_bound(keys.begin(), keys.end(), t);
    assert(i != keys.end());
    typename V::const_iterator j = i;
    ++j;
    const ScalarT u = (t - *i) / (*j - *i);
    const std::size_t pidx1 = std::distance(keys.begin(), i);
    const std::size_t pidx0 = pidx1 - 1;
    const std::size_t pidx2 = pidx1 + 1;
    const std::size_t pidx3 = pidx2 + 1;
    const V& p0 = points[pidx0];
    const V& p1 = points[pidx1];
    const V& p2 = points[pidx2];
    const V& p3 = points[pidx3];
    return CatmullRom(u, p0, p1, p2, p3);
}
//------------------------------------------------------------------------------
template < typename ScalarT >
std::vector< char >
ScalarToRGB(const std::vector< ScalarT >& data,
            const std::vector< Vector3D< ScalarT > >& colors,
            const std::vector< ScalarT >& dist,
            ScalarT minVal,
            ScalarT maxVal,
            ScalarT normFactor = ScalarT(1)) {
            std::vector< char > out;
            out.reserve(data.size() * 3);
            for(auto d: data) {
                const ScalarT v = (d - minVal) / (maxVal - minVal);
                const Vector3D< ScalarT > c =
                    normFactor * CRomInterpolation(colors, dist, v);
                out.push_back(char(v[0]);
                out.push_back(char(v[1]);
                out.push_back(char(v[2]);
            }
            return d;
}
