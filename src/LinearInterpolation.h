#pragma once
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include "Vector3D.h"

template < typename ScalarT >
Vector3D< ScalarT >
LinearInterpolation(const std::vector< Vector3D< ScalarT > >& data,
                    const std::vector< ScalarT >& keys,
                    ScalarT t ) {
    if(std::abs(t) < 10E-8) t = ScalarT(0);
    assert(data.size() == keys.size());
    if(t >= keys.back()) return data.back();
    if(t <= keys.front()) return data.front(); 
    using I = typename std::vector< ScalarT >::const_iterator;
    I minv = std::lower_bound(keys.begin(), keys.end(), t);
    I maxv = std::upper_bound(keys.begin(), keys.end(), t);
    const std::size_t minidx = std::distance(keys.begin(), minv);
    const std::size_t maxidx = std::distance(keys.begin(), maxv);
    if(maxidx == minidx) return data[minidx];
    const ScalarT u = (t - *minv) / (*maxv - *minv);
    assert(u >= ScalarT(0) && u <= ScalarT(1));
    return data[minidx] * (ScalarT(1) - u) + u * data[maxidx];
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
