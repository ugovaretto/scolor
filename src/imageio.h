#pragma once
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <turbojpeg.h>

///JPEG writer @todo add quality parameters as data members
class JPEGWriter {
public:
    JPEGWriter() {
        tj_ = tjInitCompress();
    }
    void Save(int width, int height, const char* fname, 
              const std::vector< unsigned char >& data) const {
        unsigned char* out = nullptr;
        unsigned long size = 0;
        tjCompress2(tj_, const_cast< unsigned char* >(&data[0]),
                    width, tjPixelSize[TJPF_RGB] * width, height,
                    TJPF_BGR, &out,
                    &size, TJSAMP_444, 100, TJXOP_VFLIP);
        std::ofstream os(fname, std::ios::out | std::ios::binary);
        if(!os) throw std::runtime_error("Cannot write to file");
        os.write(reinterpret_cast< char* >(out), size);
    }
    ~JPEGWriter() {
        tjDestroy(tj_);
    }
private:
    tjhandle tj_;
};
