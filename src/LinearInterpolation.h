#pragma once
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include "Vector3D.h"

template < typename ScalarT >
Vector3D< ScalarT >
LinearInterpolation(const std::vector< Vector3D< ScalarT > >& points,
                    const std::vector< ScalarT >& keys,
                    ScalarT t,
                    ScalarT minVal,
                    ScalarT maxVal ) {
    if(std::abs(t) < 10E-8) t = ScalarT(0);
    assert(points.size() == keys.size());
    assert(maxVal >= minVal);
    t = (t - minVal) / (maxVal - minVal);
    using K = std::vector< ScalarT >;
    typename K::const_iterator i = 
                                  std::lower_bound(keys.begin(), keys.end(), t);

    assert(i != keys.end());      
    if(t < *i) --i;                        
    typename K::const_iterator j = i;
    ++j;
    const ScalarT u = j == keys.end() ? ScalarT(0) : (t - *i) / (*j - *i);
    using V = Vector3D< double >;
    const std::size_t pidx0 = std::size_t(std::distance(keys.begin(), i));
    const std::size_t pidx1 = std::min(points.size() - 1, pidx0 + 1);
    const V& p0 = points[pidx0];
    const V& p1 = points[pidx1];
    return p0 * (ScalarT(1) - t) + p1 * t; 
}

template < typename ScalarT >
Vector3D< ScalarT >
SLinearInterpolation(const std::vector< Vector3D< ScalarT > >& data,
                     ScalarT minVal,
                     ScalarT maxVal,
                     ScalarT t ) {
    if(t < minVal) t = minVal;
    if(t > maxVal) t = maxVal;
    const ScalarT u = (t - minVal) / (maxVal - minVal);
    if(t == ScalarT(1.0)) return data.back();
    const std::size_t p0 = 
        std::size_t(std::floor(u * (data.size() - 1)));
    const std::size_t p1 = 
        std::size_t(std::ceil(u * (data.size() - 1)));
    const ScalarT s = u - std::floor(u * data.size() - 1);
    return data[p0] * (ScalarT(1) - s) + s * data[p1];
}
