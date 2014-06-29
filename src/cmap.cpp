#include <string>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <algorithm>
#include <sstream>

#include "io.h"
#include "imageio.h"
#include "LinearInterpolation.h"

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
    cout << "max: " << MAX << " min: " << MIN  << endl;
    return buf;
}

std::vector< Vector3D< double > > ReadColors(std::istream& is) {
    string buf;
    std::vector< Vector3D< double > > colors;
    while(is) {
        getline(is, buf);
        if(buf.empty()) continue;
        istringstream iss(buf);
        if(!iss) throw std::logic_error("Invalid color format");
        Vector3D< double > color;
        string r;
        iss >> r;
        if(!iss) throw std::logic_error("Invalid color format");
        string g;
        iss >> g;
        if(!iss) throw std::logic_error("Invalid color format");
        string b;
        iss >> b;
        if(r.find("0x") == 0) 
            color[0] = stoi(r, nullptr, 16) / 255.0;
        else
            color[0] = stod(r);
        if(g.find("0x") == 0) 
            color[1] = stoi(g, nullptr, 16) / 255.0;
        else
            color[1] = stod(g);
        if(b.find("0x") == 0) 
            color[2] = stoi(b, nullptr, 16) / 255.0;
        else
            color[2] = stod(b);
        colors.push_back(color);
    }
    return colors;
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
    if(argc < 8) {
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
    std::vector< Vector3D< double > > colors;
    if(argc > 8) {
       ifstream is(argv[8]);
       colors = ReadColors(is);
    } else { 
        std::vector< Vector3D< double > > scolors =
    //{{1,1,1}, {1, 1, 0}, {0, 1, 1}, {1, 0.5, 0.50}, {0, 0.5, 1}, {0.2, 0.4, 1}};
        {0xFFFFFF, 0xA3F9FF, 0x0FEFFF, 0x0EE1F0, 0x1FD2FF, 0x00C0F0};
        colors = scolors;
    }
    std::vector< double > keys(colors.size());
    for(int k = 0; k != keys.size(); ++k) {
        keys[k] = double(k) / (keys.size() - 1);
    }
    JPEGWriter w;
    for(int f = startFrame; f != endFrame + 1; ++f) {
        std::vector< double > data = ReadFile(path, prefix, f, suffix);
        const std::vector< ColorType > pic = 
                            //ScalarToRGB(data, colors, dist, 0.0, 1.0, 255.0);
                            LScalarToRGB(data, colors, keys, 255.0);
                            //SLScalarToRGB(data, colors, 0.0, 1.0, 255.0);
        const string outName = prefix + FrameNumToString(f, endFrame) + ".jpg";
        w.Save(width, height, outName.c_str(), pic);
    }
    return 0;
}

