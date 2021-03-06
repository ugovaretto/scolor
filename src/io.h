#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <tuple>
#include <stdexcept>
#include <cassert>
#include <fstream>
#include <algorithm>

#include "Vector3D.h"
#include "hsvrgb.h"


//-----------------------------------------------------------------------------
template < typename ScalarT >
std::vector< Vector3D< ScalarT > >
Read3DVectors(std::istream& is,
              ScalarT normFactor = ScalarT(1),
              bool fixForCubicInterpolation = false) {
    std::vector< Vector3D< ScalarT > > colors;
    if(fixForCubicInterpolation) {
        colors.push_back(Vector3D< ScalarT >());
    }
    std::string buffer;
    while(std::getline(is, buffer)) {
        if(buffer.empty()) continue;
        std::istringstream iss(buffer);
        Vector3D< ScalarT > c;
        ScalarT s;
        for(int i = 0; i != 3; ++i) {
            if(!iss) throw std::logic_error("Wrong RGB format");
            iss >> s;
            c[i] = s;
        }
        colors.push_back(normFactor * c);
    }
    if(fixForCubicInterpolation) {
        colors.push_back(colors.push_back(colors.back()));
    }
    return colors; 
}

//-----------------------------------------------------------------------------
enum KEYFRAME {DATA = 0, KEYS = 1};

template < typename ScalarT >
std::tuple< std::vector< Vector3D< ScalarT > >, std::vector< ScalarT > >
Read3DVectorKeyFrames(std::istream& is,
                      ScalarT normFactor = ScalarT(1)) {
    std::vector< Vector3D< ScalarT > > colors;
    std::vector< ScalarT > keys;
    std::string buffer;
    while(std::getline(is, buffer)) {
        if(buffer.empty()) continue;
        std::istringstream iss(buffer);
        Vector3D< ScalarT > c;
        ScalarT s;
        ScalarT k;
        for(int i = 0; i != 4; ++i) {
            if(!iss) throw std::logic_error("Wrong keyframe format");
            iss >> s;
            if(i == 0) k = s; 
            else c[i] = s;
        }
        colors.push_back(normFactor * c);
        keys.push_back(k);
    }
    return std::make_tuple(std::move(colors), std::move(keys));
}

template < typename ScalarT >
std::tuple< std::vector< Vector3D< ScalarT > >, std::vector< ScalarT > >
Read3DVectorKeyFramesCSV(std::istream& is,
                         ScalarT normFactor = ScalarT(1)) {
    std::vector< Vector3D< ScalarT > > colors;
    std::vector< ScalarT > keys;
    std::string csvbuffer;
    std::getline(is, csvbuffer);
    //first line must be header: Scalar,R,G,B
    if(!csvbuffer.size()) throw std::logic_error("Wrong csv format");
    while(std::getline(is, csvbuffer)) {
        if(csvbuffer.empty()) continue;
        std::istringstream csv(csvbuffer);
        std::string buffer;
        Vector3D< ScalarT > c;
        ScalarT s;
        ScalarT k;
        int count = 0;
        while(std::getline(csv, buffer, ',')) {
            std::istringstream iss(buffer);
            if(!iss) 
              throw std::logic_error("Wrong keyframe format");
            iss >> s;
            if(count == 0) k = s; 
            else c[count - 1] = s;
            ++count;
        }
        if(count != 4) throw std::logic_error("Wrong keyframe format");
        colors.push_back(normFactor * c);
        keys.push_back(k);
    }
    return std::make_tuple(std::move(colors), std::move(keys));
}

//-----------------------------------------------------------------------------
using ColorType = unsigned char;

template < typename ScalarT >
std::vector< ColorType >
ScalarToRGB(const std::vector< ScalarT >& data,
            const std::vector< Vector3D< ScalarT > >& colors,
            const std::vector< ScalarT >& dist,
            ScalarT minVal,
            ScalarT maxVal,
            ScalarT normFactor = ScalarT(1)) {
    std::vector< ColorType > out;
    out.reserve(data.size() * 3);
    for(auto d: data) {
        assert(d >= minVal);
        const ScalarT u = (d - minVal) / (maxVal - minVal);
        const Vector3D< ScalarT > v = normFactor 
                                   * CRomInterpolation(colors, dist, u);
        out.push_back(ColorType(v[0]));
        out.push_back(ColorType(v[1]));
        out.push_back(ColorType(v[2]));
    }
    return out;
}

//-----------------------------------------------------------------------------
template < typename ScalarT >
std::vector< ColorType >
CRKScalarToRGB(const std::vector< ScalarT >& data,
               const std::vector< Vector3D< ScalarT > >& colors,
               const std::vector< ScalarT >& keys,
               ScalarT scalingFactor = ScalarT(1),
               ScalarT minVal = ScalarT(0),
               ScalarT maxVal = ScalarT(1)) {
    std::vector< ColorType > out;
    out.reserve(data.size() * 3);
    for(auto d: data) {
        const Vector3D< ScalarT > v = 
            scalingFactor * KeyFramedCRomInterpolation(colors, keys, d, 
                                                       minVal, maxVal);
        out.push_back(ColorType(v[0]));
        out.push_back(ColorType(v[1]));
        out.push_back(ColorType(v[2]));
    }
    return out;
}


//-----------------------------------------------------------------------------
template < typename ScalarT >
std::vector< ColorType >
CRKScalarHSVToRGB(const std::vector< ScalarT >& data,
               const std::vector< Vector3D< ScalarT > >& colors,
               const std::vector< ScalarT >& keys,
               ScalarT scalingFactor = ScalarT(1),
               ScalarT minVal = ScalarT(0),
               ScalarT maxVal = ScalarT(1)) {
    std::vector< ColorType > out;
    out.reserve(data.size() * 3);
    for(auto d: data) {
        const Vector3D< ScalarT > v = 
                           KeyFramedCRomInterpolation(colors, keys, d, 
                                                      minVal, maxVal);
        hsv h(v[0], v[1], v[2]);
        rgb c = hsv2rgb(h);                             
        out.push_back(ColorType(scalingFactor * c.r));
        out.push_back(ColorType(scalingFactor * c.g));
        out.push_back(ColorType(scalingFactor * c.b));
    }
    return out;
}


//-----------------------------------------------------------------------------
template < typename ScalarT >
std::vector< ColorType >
LScalarToRGB(const std::vector< ScalarT >& data,
             const std::vector< Vector3D< ScalarT > >& colors,
             const std::vector< ScalarT >& keys,
             ScalarT normFactor = ScalarT(1),
             ScalarT minVal = ScalarT(0),
             ScalarT maxVal = ScalarT(1)) {
    std::vector< ColorType > out;
    out.reserve(data.size() * 3);
    for(auto d: data) {
        const Vector3D< ScalarT > v = normFactor 
                                   * LinearInterpolation(colors, keys, d, 
                                                         minVal, maxVal);
        out.push_back(ColorType(v[0]));
        out.push_back(ColorType(v[1]));
        out.push_back(ColorType(v[2]));
    }
    return out;
}

//-----------------------------------------------------------------------------
template < typename ScalarT >
std::vector< ColorType >
LScalarHSVToRGB(const std::vector< ScalarT >& data,
                const std::vector< Vector3D< ScalarT > >& colors,
                const std::vector< ScalarT >& keys,
                ScalarT normFactor = ScalarT(1),
                ScalarT minVal = ScalarT(0),
                ScalarT maxVal = ScalarT(1)) {
    std::vector< ColorType > out;
    out.reserve(data.size() * 3);
    for(auto d: data) {
        const Vector3D< ScalarT > v =  
                                     LinearInterpolation(colors, keys, d, 
                                                         minVal, maxVal);
        
        hsv h(v[0], v[1], v[2]);
        rgb c = hsv2rgb(h);                         
        out.push_back(ColorType(normFactor * c.r));
        out.push_back(ColorType(normFactor * c.g));
        out.push_back(ColorType(normFactor * c.b));
    }
    return out;
}
//-----------------------------------------------------------------------------
template < typename ScalarT >
std::vector< ColorType >
SLScalarToRGB(const std::vector< ScalarT >& data,
              const std::vector< Vector3D< ScalarT > >& colors,
              ScalarT minVal,
              ScalarT maxVal,
              ScalarT normFactor = ScalarT(1)) {
    std::vector< ColorType > out;
    out.reserve(data.size() * 3);
    for(auto d: data) {
        const Vector3D< ScalarT > v = normFactor 
                                   * SLinearInterpolation(colors, minVal, 
                                                          maxVal, d);
        out.push_back(ColorType(v[0]));
        out.push_back(ColorType(v[1]));
        out.push_back(ColorType(v[2]));
    }
    return out;
}

template < typename ScalarT >
std::vector< ColorType >
ScalarToGray(const std::vector< ScalarT >& data,
             ScalarT minVal,
             ScalarT maxVal,
             ScalarT normFactor = ScalarT(1)) {
     std::vector< ColorType > out;
     out.reserve(data.size() * 3);
     for(auto d: data) {
         const ScalarT v = (d - minVal) / (maxVal - minVal);
        out.push_back(normFactor * v);
        out.push_back(normFactor * v);
        out.push_back(normFactor * v);
     }
     return out;
}

