#pragma once

#include <iterator>
#include <vector>
#include <algorithm>
#include <tuple>
#include <cmath>
#include <cassert>


#include "Vector3D.h"
//------------------------------------------------------------------------------
///Compute at each point the sum of the *SQUARED* distences from the first point
template < typename FwdIt >
std::vector< typename std::iterator_traits< FwdIt >::value_type::Scalar > 
ComputeDistances(FwdIt begin, FwdIt end) {
    using S = typename std::iterator_traits< FwdIt >::value_type::Scalar;
    std::vector< S > res;
    res.reserve(std::distance(begin, end));
    FwdIt v0 = begin;
    FwdIt v1 = ++begin;
    res.push_back(S(0));
    while(v1 != end) {
        res.push_back(SqDist(*v1, *v0) + res.back());
        v0 = v1;
        ++v1;
    }
    const S total = res.back();
    std::transform(res.begin(), res.end(), 
                   res.begin(), [total](S v) { return v / total; });
    return res;
}



///Find in the sorted array of distances the position of the element holding
///the closest value lower or equal to the passed value 
template < typename ScalarT >
std::size_t FindPos(const std::vector< ScalarT >& dist, ScalarT t) {
    assert(t >= ScalarT(0) && t <= ScalarT(1));
    const ScalarT totalLength = dist.back();
    const ScalarT d = t * totalLength;
    using V = std::vector< ScalarT >;
    typename V::const_iterator e = 
        std::lower_bound(dist.begin(), dist.end(), d);
    assert(e != dist.end());
    return std::size_t(std::distance(dist.begin(), e));
}

///Extract normalized ([0,1]) parameter and control points from global control
///point array.
///Duplicate first and last point.
template < typename ScalarT >
std::tuple< ScalarT,
            const Vector3D< ScalarT >&,
            const Vector3D< ScalarT >&,
            const Vector3D< ScalarT >&,
            const Vector3D< ScalarT >& >
Extract(const std::vector< ScalarT >& dist,
        const std::vector< Vector3D< ScalarT > >& points,
        ScalarT t) {
   using V = Vector3D< ScalarT >;
   using T = std::tuple< ScalarT, const V&, const V&, 
                         const V&, const V& >;
   const std::size_t pidx1 = FindPos(dist, t);
   const std::size_t pidx0 = std::max(std::size_t(0), pidx1 - 1);
   const std::size_t pidx2 = pidx1 + 1;
   const std::size_t pidx3 = std::min(points.size() - 1, pidx2 + 1);
   ScalarT u = (t * dist.back() - dist[pidx1]) 
                     / Dist(points[pidx1], points[pidx2]);
   if(u != u) u = ScalarT(0);
   assert(u >= ScalarT(0));
   return T(u, points[pidx0], points[pidx1], points[pidx2], points[pidx3]);
}


//------------------------------------------------------------------------------
///Standard Catmull-Rom spline
template < typename ScalarT >
Vector3D< ScalarT > CatmullRom(ScalarT u,
                               const Vector3D< ScalarT >& P0,
                               const Vector3D< ScalarT >& P1,
                               const Vector3D< ScalarT >& P2,
                               const Vector3D< ScalarT >& P3,
                               ScalarT minVal = ScalarT(0),
                               ScalarT maxVal = ScalarT(1)) {
    assert(maxVal >= minVal);
    u = maxVal > minVal ? (u - minVal) / (maxVal - minVal) : maxVal;
    const Vector3D< ScalarT > c0(P1);
    const Vector3D< ScalarT > c1 = -0.5 * P0 + 0.5 * P2;
    const Vector3D< ScalarT > c2 = P0 - 2.5 * P1 + 2.0 * P2 - 0.5 * P3;
    const Vector3D< ScalarT > c3 = - 0.5 * P0 + 1.5 * P1 - 1.5 * P2 + 0.5 * P3;
    return ((c3 * u + c2) * u + c1) * u + c0;
}

//------------------------------------------------------------------------------
///Piecewise Catmull-Rom spline
template < typename ScalarT > Vector3D< ScalarT >
CRomInterpolation(const std::vector< Vector3D< ScalarT > >& points,
                  const std::vector< ScalarT >& dist,
                  ScalarT t,
                  ScalarT minVal = ScalarT(0),
                  ScalarT maxVal = ScalarT(1)) {
    if(std::abs(t) < 10E-8) t = ScalarT(0);
    assert(points.size());
    assert(dist.size());
    assert(points.size() == dist.size());
    using V = Vector3D< ScalarT >;
    using T = std::tuple< ScalarT, const V&, const V&, const V&, const V& >;
    const T p(Extract(dist, points, t));
    return CatmullRom(std::get<0>(p), 
                      std::get<1>(p), std::get<2>(p), std::get<3>(p), 
                      std::get<4>(p), minVal, maxVal);
}
//------------------------------------------------------------------------------
///Keyframed interpolation: each parameter value in the keys array matches
///a point in the points array
///keys and arrays must be of the same size and shall contain all the points
///in the original vector
template < typename ScalarT > Vector3D< ScalarT >
KeyFramedCRomInterpolation(const std::vector< Vector3D< ScalarT > >& points,
                           const std::vector< ScalarT >& keys,
                           ScalarT t,
                           ScalarT minVal = ScalarT(0),
                           ScalarT maxVal = ScalarT(1)) {
    assert(points.size());
    assert(keys.size());
    assert(points.size() == keys.size());
    using K = std::vector< ScalarT >;
    typename K::const_iterator i = 
                                  std::lower_bound(keys.begin(), keys.end(), t);

    assert(i != keys.end());      
    if(t < *i) --i;                        
    typename K::const_iterator j = i;
    ++j;
    const ScalarT u = j == keys.end() ? ScalarT(0) : (t - *i) / (*j - *i);
    using V = Vector3D< double >;
    const std::size_t pidx1 = std::size_t(std::distance(keys.begin(), i));
    const std::size_t pidx0 = std::max(pidx1 - 1, std::size_t(0));
    const std::size_t pidx2 = pidx1 + 1;
    const std::size_t pidx3 = std::min(points.size() - 1, pidx2 + 1);
    //const V& p0 = points[pidx0];
    const V& p1 = points[pidx1];
    const V& p2 = points[pidx2];
    //const V& p3 = points[pidx3];
    V p0;
    V p3;
    if(pidx1 - 1 < 0) {
        p0 = ScalarT(2) * p1 - p2;
    } else p0 = points[pidx0];
    if(pidx2 + 1 >= points.size()) {
        p3 = ScalarT(2) * p2 - p1;
    } else p3 = points[pidx3];
    return CatmullRom(u, p0, p1, p2, p3, minVal, maxVal);
}

    
