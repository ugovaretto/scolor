#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>
int main(int argc, char** argv) {
    if(argc < 5) {
        std::cout << "usage: " << argv[0] 
                  << " <width> <height> <# levels> <file name>" << std::endl;
        return 1;
    }
    const int width = std::stoi(argv[1]);
    assert(width > 0);
    const int height = std::stoi(argv[2]);
    assert(height > 0);
    const int levels = std::stoi(argv[3]);
    assert(levels > 0);
    std::ofstream os(argv[4], std::ios::binary);
    assert(os);
    const int chunk = width / levels;
    for(int j = 0; j != height; ++j) {
        for(int i = 0; i < width; i += chunk) {
            const std::vector< double > d(chunk, double(i) / (width - chunk));
            os.write(reinterpret_cast< const char* >(&d[0]), 
                     d.size() * sizeof(double));
        }
        const std::vector< double > d(width % levels, 1.0);
        const double one = 1.0;
        for(int i = 0; i != width % levels; ++i) 
            os.write(reinterpret_cast< const char* >(&one), 
                     1 * sizeof(double));
    }
    return 0;
}