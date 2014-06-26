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
    void Save(const char* fname, const std::vector< unsigned char >& data) 
    const {
        char* out = nullptr;
        unsigned long size = 0;
        tjCompress2(tj_, &data[0], width, tjPixelSize[TJF_RGB] * width, height,
                    TJPF_RGB, &out, &size, TJSAMP_444, 80, TJXOP_VFLIP);
        std::ofstream os(fname, ios::out | ios::binary);
        if(!os) throw std::runtime_error("Cannot write to file");
        os.write(reinterpret_cast< char* >(out), size);
    }
    ~JPEGWriter() {
        tjDestroy(tj_);
    }
private:
    tjhandle tj_;
};
