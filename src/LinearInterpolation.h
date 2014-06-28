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
    
