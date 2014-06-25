#pragma once
#include <cmath>
#include "Vector3D.h"

Vector3D< ScalarT > 
HSVtoRGB(const Vector3D< ScalarT >& v) {
    enum {H = 0, S = 1, V = 2};

    if(v[S] == ScalarT(0)) {
        // achromatic (grey)
        return Vector3D< ScalarT >(v[V], v[V], v[V]);
    }
    using C = ScalarT;
    const C h = v[H] / 60;      // sector 0 to 5
    const C i = std::floor(h);
    const C f = h - i;          // factorial part of h
    const C p = v[V] * ( 1 - v[S] );
    const C q = v[V] * ( 1 - v[S] * f );
    const C t = v[V] * ( 1 - v[S] * ( 1 - f ) );
    C r = ScalarT();
    C g = ScalarT();
    C b = ScalarT();
    switch(i) {
        case 0:
            r = v[V];
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = v[V];
            b = p;
            break;
        case 2:
            r = p;
            g = v[V];
            b = t;
            break;
        case 3:
            r = p;
            g = q;
            b = v[V];
            break;
        case 4:
            r = t;
            g = p;
            b = v[V];
            break;
        default: // case 5:
            r = v[V];
            g = p;
            b = q;
            break;
    }
    return Vector3D< ScalarT >(r, g, b);
}
