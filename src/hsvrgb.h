#pragma once
#include <cmath>
#include <vector>
#include "Vector3D.h"

// HSV to RGB conversion formula
// When 0 ≤ H < 360, 0 ≤ S ≤ 1 and 0 ≤ V ≤ 1:
// C = V × S
// X = C × (1 - |(H / 60º) mod 2 - 1|)
// m = V - C

// (R,G,B) = (R'+m, G'+m, B'+m)
struct rgb {
    double r = 0;       // percent
    double g = 0;       // percent
    double b = 0;       // percent
};

struct hsv {
    double h = 0;       // angle in degrees
    double s = 0;       // percent
    double v = 0;       // percent
    hsv(double H, double S, double V) : h(H), s(S), v(V) {}
    hsv(const hsv&) = default;
    hsv(hsv&&) = default;
    hsv() = default;
};



hsv rgb2hsv(rgb in) {
    hsv         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.v = max;                                // v
    delta = max - min;
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0              
            // s = 0, v is undefined
        out.s = 0.0;
        out.h = NAN;                            // its now undefined
        return out;
    }
    if( in.r >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
    else
    if( in.g >= max )
        out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}


rgb hsv2rgb(hsv in) {
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;     
}


#if 0
//#if 0
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
#endif

