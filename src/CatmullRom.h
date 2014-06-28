#pragma once

#include <iterator>
#include <vector>
#include <algorithm>
#include <tuple>
#include <cmath>
#include <cassert>

#include "Vector3D.h"

//------------------------------------------------------------------------------
///Compute at each point the sum of the distences from the first point
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
        res.push_back(Dist(*v1, *v0) + res.back());
        v0 = v1;
        ++v1;
    }
    return res;
}

///Find in the sorted array of distances the position of the element holding
///the closest value lower or equal to the passed value 
template < typename ScalarT >
std::size_t FindPos(const std::vector< ScalarT >& dist, ScalarT t) {
    assert(t >= ScalarT(0) && t <= ScalarT(1));
    const ScalarT totalSize = dist.back();
    const ScalarT d = t * totalSize;
    using V = std::vector< ScalarT >;
    typename V::const_iterator e = s
        td::lower_bound(dist.begin(), dist.end(), d);
    assert(e != dist.end());
    typename V::const_iterator f = 
    return std::size_t(std::distance(dist.begin(), e));
}

///Extract normalized ([0,1]) parameter and control points from global control
///point array.
///The point array *shall* have a size = dist.size + 4 because:
/// # two points are added to the original array to allow for proper
//interpolation
/// # the first and last two points are replicated and the first point
///   meaningful for interpolation is the secon one in a CR spline
template < typename ScalarT >
std::tuple< ScalarT,
            const Vector3D< ScalarT >&,
            const Vector3D< ScalarT >&,
            const Vector3D< ScalarT >&,
            const Vector3D< ScalarT >& >
Extract(const std::vector< ScalarT >& dist,
        const std::vector< Vector3D< ScalarT > >& points,//size = dist.size + 4,
        ScalarT t) {
   using V = Vector3D< ScalarT >;
   using T = std::tuple< ScalarT, const V&, const V&, 
                         const V&, const V& >;
   const std::size_t p = FindPos(dist, t);
   const std::size_t pidx1 = p + 1; //move to second point where
                                    //first point is replicated
   const std::size_t pidx0 = pidx1 - 1;
   const std::size_t pidx2 = pidx1 + 1;
   const std::size_t pidx3 = pidx2 + 1;
   ScalarT u = (t * dist.back() - dist[p]) 
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
                               const Vector3D< ScalarT >& P3) {
    assert(u >= ScalarT(0) && u <= ScalarT(1));
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
                  ScalarT t) {
    if(std::abs(t) < 10E-8) t = ScalarT(0);
    assert(points.size());
    assert(dist.size());
    assert(t >= ScalarT(0) && t <= ScalarT(1));
    assert(points.size() == dist.size() + 2);
    using V = Vector3D< ScalarT >;
    using T = std::tuple< ScalarT, const V&, const V&, const V&, const V& >;
    const T p(Extract(dist, points, t));
    return CatmullRom(std::get<0>(p), 
                      std::get<1>(p), std::get<2>(p), std::get<3>(p), 
                      std::get<4>(p));
}
//------------------------------------------------------------------------------
///Keyframed interpolation: each parameter value in the keys array matches
///a point in the points array
///keys and arrays must be of the same size and shall contain all the points
///in the original vector with the first and last points replicated
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
    //first point is replicated: if lower bound matches keys[0] we need to add
    //1
    const std::size_t d = std::size_t(std::distance(keys.begin(), i));
    const std::size_t pidx1 = d == 0 ? d + 1 : d;
    const std::size_t pidx0 = pidx1 - 1;
    const std::size_t pidx2 = pidx1 + 1;
    const std::size_t pidx3 = pidx2 + 1;
    const V& p0 = points[pidx0];
    const V& p1 = points[pidx1];
    const V& p2 = points[pidx2];
    const V& p3 = points[pidx3];
    return CatmullRom(u, p0, p1, p2, p3);
}
