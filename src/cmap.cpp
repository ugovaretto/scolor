
//clang++ -std=c++11 -stdlib=libc++ ../src/cmap.cpp -I /opt/libjpeg-turbo/include -L /opt/libjpeg-turbo/lib -lturbojpeg -o cmap
//./cmap ./ 400x100- 0 0 .out 400 100 -f ../maps/CoolWarmFloat33.csv -csv -stat

#include <string>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <tuple>
#include <map>

#include "io.h"
#include "imageio.h"
#include "LinearInterpolation.h"

#include "CatmullRom.h"

using namespace std;

//------------------------------------------------------------------------------
using Data = tuple< std::vector< double >, double, double >;
enum {DATASET = 0, DATASET_MIN = 1 , DATASET_MAX = 2};
Data ReadFile(string path,
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
    const double m = *min_element(buf.begin(), buf.end());
    const double M = *max_element(buf.begin(), buf.end());
    return make_tuple(buf, m, M);
}

std::vector< Vector3D< double > > ReadColors(std::istream& is, bool autonorm) {
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
        else {
            color[0] = stod(r);
            if(autonorm && color[0] > 1.0) color[0] /= 255.0;
        }
        if(g.find("0x") == 0) 
            color[1] = stoi(g, nullptr, 16) / 255.0;
        else{
            color[1] = stod(g);
            if(autonorm && color[1] > 1.0) color[1] /= 255.0;
        }
        if(b.find("0x") == 0) 
            color[2] = stoi(b, nullptr, 16) / 255.0;
        else {
            color[2] = stod(b);
            if(autonorm && color[2] > 1.0) color[2] /= 255.0;
        }
        colors.push_back(color);
    }
    return colors;
}


using KeyData = std::tuple< std::vector< Vector3D< double > >, std::vector< double > >;
KeyData ReadColorsCSV(istream& is, double norm) {
    return Read3DVectorKeyFramesCSV< double >(is, norm);
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
        std::cout << "\nusage: " 
                  << argv[0]
                  << "  <path> <prefix>"
                     "  <start frame #> <end frame #>"
                     " <suffix> <width> <height> [-cubic] [-dist] "
                     "[-f filename [-csv] [-norm]] [-stat]\n";
        std::cout << "-hsv: input is in HSV format\n" 
                  << "-cubic: use Catmull-Rom interpolation, default is linear\n"
                  << "-dist:  parameterization is proportional to (chord length)^2, default il uniform\n"
                  << "-csv:   keyfranmes in csv format: t,R,G,B first line skipped\n"
                  << "-norm:  force division by 255\n"
                  << "-stat:  print min, max, num levels and value with max num levels\n";            

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
    vector< string > args(argv, argv + argc);
    const bool distanceParameterization = find(args.begin(), args.end(), "-dist")
                                          != args.end();
    const bool csv = find(args.begin(), args.end(), "-csv") != args.end();
    const double norm = find(args.begin(), args.end(), "-norm") != args.end() ? 1./255. : 1.;
    const bool stat = find(args.begin(), args.end(), "-stat") != args.end();
    const bool cubicInterpolation = find(args.begin(), args.end(), "-cubic") != args.end();
    const bool hsv = find(args.begin(), args.end(), "-hsv") != args.end();
    vector< double > keyframes;
    if(find(args.begin(), args.end(), "-f") != args.end()
       && ++find(args.begin(), args.end(), "-f") != args.end()) {
        ifstream is(*++find(args.begin(), args.end(), "-f"));
        if(!is) {
            std::cerr << "Cannot open input file" << std::endl;
            return -1;
        }
        if(csv) {
            const KeyData kd = ReadColorsCSV(is, norm);
            colors = get< KEYFRAME::DATA >(kd);
            keyframes = get< KEYFRAME::KEYS >(kd);
        } else colors = ReadColors(is, !hsv);
    } else { 
        std::vector< Vector3D< double > > scolors =
    //{{1,1,1}, {1, 1, 0}, {0, 1, 1}, {1, 0.5, 0.50}, {0, 0.5, 1}, {0.2, 0.4, 1}};
        {0xFFFFFF, 0xA3F9FF, 0x0FEFFF, 0x0EE1F0, 0x1FD2FF, 0x00C0F0};
        colors = scolors;
    }
    std::vector< double > keys;
    if(keyframes.size()) keys = keyframes;
    else {
        if(distanceParameterization) {
            keys = ComputeDistances(colors.begin(), colors.end());
        } else {
            keys.resize(colors.size());
            for(int k = 0; k != keys.size(); ++k) {
                keys[k] = double(k) / (keys.size() - 1);
            }
        }
    }
    JPEGWriter w;
    for(int f = startFrame; f != endFrame + 1; ++f) {
        Data data = ReadFile(path, prefix, f, suffix);
        if(stat) {
            map<double, int> freq;
            const std::vector< double >& d = get<DATASET>(data);
            for_each(d.cbegin(), d.cend(), [&freq](double v) {freq[v]++;});
            using MV = map<double, int>::value_type;
            map<double, int>::iterator mi = max_element(freq.begin(),
                                                        freq.end(),
                                                        [](const MV& v1, const MV& v2){
                                                            return v1.second < v2.second;
                                                        });
            cout << path + prefix + to_string(f) + suffix
                 << ": min = " << get<DATASET_MIN>(data)
                 << "  max = " << get<DATASET_MAX>(data)
                 << "  # levels = " << freq.size()
                 << "  max levels = " << mi->first << "->" 
                 << mi->second  
                 << endl;
        }
        const double normFactor = 255.0;
        std::vector< ColorType > pic;
        if(!hsv) {
            pic = cubicInterpolation ?
            CRKScalarToRGB(get<DATASET>(data), 
                           colors, 
                           keys, 
                           normFactor,
                           get<DATASET_MIN>(data),
                           get<DATASET_MAX>(data))
            : LScalarToRGB(get<DATASET>(data), 
                           colors, 
                           keys, 
                           normFactor,
                           get<DATASET_MIN>(data),
                           get<DATASET_MAX>(data));
                          
        } else {
            pic = cubicInterpolation ?
            CRKScalarHSVToRGB(get<DATASET>(data), 
                           colors, 
                           keys, 
                           normFactor,
                           get<DATASET_MIN>(data),
                           get<DATASET_MAX>(data))
            : LScalarHSVToRGB(get<DATASET>(data), 
                           colors, 
                           keys, 
                           normFactor,
                           get<DATASET_MIN>(data),
                           get<DATASET_MAX>(data));    
        }
        const string outName = prefix + FrameNumToString(f, endFrame) + ".jpg";
        w.Save(width, height, outName.c_str(), pic);
    }
    return 0;
}

