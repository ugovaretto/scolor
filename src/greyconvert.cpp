#include <string>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <fstream>

#include "io.h"
#include "imageio.h"

using namespace std;

//------------------------------------------------------------------------------
std::vector< double > ReadFileService(string path,
                                      string prefix,
                                      int n,
                                      const string& suffix) {
    if(path.size() < 1) throw logic_error("Invalid  path size");
    if(path[path.size()-1] != '/') path += '/';
    prefix = path + prefix;
    const string fname = prefix + to_string(startFrame) + suffix;
    ifstream in(fname, std::ifstream::in
                    | std::ifstream::binary);
    if(!in) throw std::runtime_error("Cannot read from file");
    in.seekg(0, ios::end);
    const size_t fileSize = in.tellg();
    in.seekg(0, ios::beg);
    shared_ptr< File > buf(new File(fileSize));
    in.read(&buf->front(), buf->size());
    q.Put(buf);
    startFrame = (startFrame + 1) % period;
}

//------------------------------------------------------------------------------
int main(int argc, char** argv) {
    if(argc != 6) {
        std::cout << "usage: " 
                  << argv[0]
                  << " <path> <prefix> <start frame #> <end frame #>"
                     " <suffix>\n";
        return 1;
    }
    const string path = argv[1];
    const string prefix = argv[2];
    const string suffix = argv[5];
    const int startFrame = stoi(argv[3]); //throws if arg not valid
    const int endFrame   = stoi(argv[4]); //throws if arg not valid
    std::vector< double > data = ReadFile(path, prefix, startFrame, suffix);
    const std::vector< Colortype > pic = ScalarToGray(data, 0.0, 1.0, 255.0);
    JPEGWriter w;
    w.Save("tmp.jpg", pic);
    return 0;
}

