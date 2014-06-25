#include <iostream>
#include <string>
#include "CatmullRom.h"

int main(int argc, char** argv) {
    if(argc != 2) {
        std::cerr << "usage: " << argv[0] << " value\n";
        return 1;
    }
    using Color = Vector3D< double >;
    const Color c1(0,0,0);
    const Color c2(0,1,0);
    const Color c3(0,0,1);
    const Color c4(1,0,0);
    const Color C = CatmullRom(std::stod(argv[1]), c1, c2, c3, c4);
    std::cout << C << std::endl;
    return 0;
}
