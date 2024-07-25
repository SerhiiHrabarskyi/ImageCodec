#ifndef IMGCODEC_H
#define IMGCODEC_H

#include <string>
#include <vector>

namespace ImgCodec {
    struct RawImageData {
        int width;
        int height;
        std::vector<unsigned char> data;
    };

    void encode(const RawImageData& imageData, const std::string& filePath);
    RawImageData decode(const std::string& filePath);
}

#endif // IMGCODEC_H
