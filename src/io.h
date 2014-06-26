#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <tuple>

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

enum KEYFRAME {DATA = 0, KEYS = 1};

std::tuple< std::vector< Vector3D< ScalarT > >, std::vector< ScalarT > >
Read3DVectorKeyFrames(std::istream& is,
                      ScalarT normFactor = ScalarT(1),
                      bool fixForCubicInterpolation = false) {
    std::vector< Vector3D< ScalarT > > colors;
    std::vector< ScalarT > keys;
    if(fixForCubicInterpolation) {
        colors.push_back(Vector3D< ScalarT >());
        keys.push_back(ScalarT());
    }
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
    if(fixForCubicInterpolation) {
        colors.push_back(colors.back());
        keys.push_back(keys.back());
        colors[0] = colors[1];
        keys[0] = keys[1];
    }
    return std::make_tuple(std::move(colors), std::move(keys));
}

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
                out.push_back(normFactor * CRomInterpolation(colors, dist, v));
            return d;
}


