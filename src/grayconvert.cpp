// clang++ -std=c++11 -stdlib=libc++ \
// ../src/grayconvert.cpp -I /opt/libjpeg-turbo/include \
// -L /opt/libjpeg-turbo/lib -lturbojpeg -o grayconvert

#include <string>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <algorithm>
#include <sstream>

#include "io.h"
#include "imageio.h"

using namespace std;

//------------------------------------------------------------------------------
std::vector< double > ReadFile(string path,
                               string prefix,
                               int n,
                               const string& suffix) {
    if(path.size() < 1) throw logic_error("Invalid  path size");
    if(path[path.size()-1] != '/') path += '/';
    prefix = path + prefix;
    const string fname = prefix + to_string(n) + suffix;
    ifstream in(fname, std::ifstream::in
                    | std::ifstream::binary);
    if(!in) throw std::runtime_error("Cannot read from file");
    in.seekg(0, ios::end);
    const size_t fileSize = in.tellg();
    in.seekg(0, ios::beg);
    std::vector< double > buf(fileSize / sizeof(double));
    in.read(reinterpret_cast< char* >(&buf.front()), fileSize);
    const double MAX = *max_element(buf.begin(), buf.end());
    const double MIN = *min_element(buf.begin(), buf.end());
    cout << "min: " << MIN << " max: " << MAX << endl;
    return buf;
}

string FrameNumToString(int f, int endFrame) {
    int totalDigits = 1;
    while(endFrame /= 10) ++totalDigits;
    int fr = f;
    int fdigits = 1;
    while(fr /= 10) ++fdigits;
    ostringstream oss;
    for(int i = 0; i != (totalDigits - fdigits); ++i) oss << "0";
    oss << f;
    return oss.str();
}
    
//------------------------------------------------------------------------------
int main(int argc, char** argv) {
    if(argc != 8) {
        std::cout << "usage: " 
                  << argv[0]
                  << "  <path> <prefix>"
                     "  <start frame #> <end frame #>"
                     " <suffix> <width> <height>\n";
        return 1;
    }
    const string path = argv[1];
    const string prefix = argv[2];
    const string suffix = argv[5];
    const int startFrame = stoi(argv[3]); //throws if arg not valid
    const int endFrame   = stoi(argv[4]); //throws if arg not valid
    const int width = stoi(argv[6]);
    const int height = stoi(argv[7]);
    JPEGWriter w;
    for(int f = startFrame; f != endFrame + 1; ++f) {
        std::vector< double > data = ReadFile(path, prefix, f, suffix);
        const std::vector< ColorType > pic = 
                            ScalarToGray(data, 0.0, 1.0, 255.0);
        const string outName = "out" + FrameNumToString(f, endFrame) + ".jpg";
        w.Save(width, height, outName.c_str(), pic);
    }
    return 0;
}

