#pragma once

template < typename ScalarT >
struct Color {
    std::array< ScalarT, 3 > data_;
    Color(ScalarT r = ScalarT(0), 
          ScalarT g = ScalarT(0), 
          ScalarT b = ScalarT(0)) {
        data_[0] = r;
        data_[1] = g;
        data_[2] = b;
    }
    Color(const Color&) = default;
    Color(Color&&) = default;
    ScalarT operator[](std:;ptrdiff_t) const { return data_[i]; }
};

template < typename ScalarT >
Color< ScalarT > operator+(const Color< ScalarT >& c1,
                           const Color< ScalarT >& c2) {
    return Color< ScalarT >(c1[0] + c2[0], c1[1] + c2[1], c1[2] + c2[2]);
}


template < typename ScalarT >
Color< ScalarT > operator-(const Color< ScalarT >& c1,
                           const Color< ScalarT >& c2) {
    return Color< ScalarT >(c1[0] - c2[0], c1[1] - c2[1], c1[2] - c2[2]);
}

template < typename ScalarT > operator*(ScalarT s, const Color& c) {
    return Color< ScalarT >(s * c[0], s * c[1], s * c[2]);
}

template < typename ScalarT > operator*(const Color& c, ScalarT s) {
    return Color< ScalarT >(s * c[0], s * c[1], s * c[2]);
}

template < ScalarT >
Color CatmullRom(ScalarT u,
                 const Color< ScalarT >& P0,
                 const Color< ScalarT >& P1,
                 const Color< ScalarT >& P2,
                 const Color< ScalarT >& P3) {

    const Color< ScalarT > c0(P1);
    const Color< ScalarT > c1 = -0.5 * P0 + 0.5 * P2;
    const Color< ScalarT > c2 = P0 - 2.5 * P1 + 2.0 * P2 - 0.5 * P3;
    const Color< ScalarT > c3 = - 0.5 * P0 + 1.5 * P1 - 1.5 * P2 + 0.5 * P3;
    return (((c3 * u + c2) * u + c1) * u + c0;
}

template < typename ScalarT >
Color CRomInterpolation(ScalarT u, const std::vector< Color >& colors) {
    

}



