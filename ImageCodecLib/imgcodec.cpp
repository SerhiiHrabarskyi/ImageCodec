#include "imgcodec.h"

#include <fstream>
#include <iostream>
#include <cstring>
#include <vector>

using namespace ImgCodec;

const size_t PixelAlignment = 4;

class BitArray {
public:
    BitArray() = default;

    explicit BitArray(size_t size, bool defaultValue = false)
        : bits((size + CHAR_BIT - 1) / CHAR_BIT, defaultValue ? 0xFF : 0)
        , bitCount(size)
    {

    }

    void resize(size_t size) {
        bits.resize((size + CHAR_BIT - 1) / CHAR_BIT, 0);
        bitCount = size;
    }

    bool get(size_t index) const {
        if (index >= bitCount) {
            throw std::out_of_range("Bit index out of range");
        }
        return (bits[index / CHAR_BIT] & (1 << (index % CHAR_BIT))) != 0;
    }

    void set(size_t index, bool value) {
        if (index >= bitCount) {
            throw std::out_of_range("Bit index out of range");
        }
        if (value) {
            bits[index / CHAR_BIT] |= (1 << (index % CHAR_BIT));
        } else {
            bits[index / CHAR_BIT] &= ~(1 << (index % CHAR_BIT));
        }
    }

    void toggleBit(size_t index) {
        if (index >= bitCount) {
            throw std::out_of_range("Bit index out of range");
        }
        bits[index / CHAR_BIT] ^= (1 << (index % CHAR_BIT));
    }

    void setBitsFromArray(const unsigned char* array, size_t arraySize) {
        bits = std::vector<unsigned char>(array, array + arraySize);
        bitCount = arraySize * CHAR_BIT;
    }

    size_t size() const {
        return bitCount;
    }

    void clear(bool defaultValue = false) {
        std::fill(bits.begin(), bits.end(), defaultValue ? 0xFF : 0);
    }

    const unsigned char* data() const
    {
        return bits.data();
    }

private:
    std::vector<unsigned char> bits;
    size_t bitCount = 0;
};

struct EncodedFile
{
    const std::string formatId = "BA";
    int width;
    int height;
    BitArray rowsIndex;
    std::vector<unsigned char> encodedData;

    void readFromFile(const std::string& filePath)
    {
        std::ifstream file(filePath, std::ios::binary);

        if (!file) {
            throw std::runtime_error("Cannot open file for reading: " + filePath);
        }

        char format[2];
        file.read(format, 2);
        if (format[0] != 'B' || format[1] != 'A') {
            throw std::runtime_error("Invalid format identifier in file: " + filePath);
        }

        file.read(reinterpret_cast<char*>(&width), sizeof(width));
        file.read(reinterpret_cast<char*>(&height), sizeof(height));

        auto rowsIndexArrSize = height / CHAR_BIT;
        std::vector<unsigned char> rowsIndexArr(rowsIndexArrSize);
        file.read(reinterpret_cast<char*>(rowsIndexArr.data()), rowsIndexArrSize);
        rowsIndex.setBitsFromArray(rowsIndexArr.data(), rowsIndexArrSize);;

        encodedData = std::vector<unsigned char>(std::istreambuf_iterator<char>(file), {});

        file.close();
    }

    void writeToFile(const std::string& filePath)
    {
        std::ofstream file(filePath, std::ios::binary);

        if (!file) {
            throw std::runtime_error("Cannot open file for writting: " + filePath);
        }

        file.write("BA", 2);

        file.write(reinterpret_cast<const char*>(&width), sizeof(width));
        file.write(reinterpret_cast<const char*>(&height), sizeof(height));
        file.write(reinterpret_cast<const char*>(rowsIndex.data()), rowsIndex.size() / 8);
        file.write(reinterpret_cast<const char*>(encodedData.data()), encodedData.size());

        file.close();
    }
};

void ImgCodec::encode(const RawImageData& imageData, const std::string& filePath)
{
    EncodedFile file;

    file.width = imageData.width;
    file.height = imageData.height;

    file.rowsIndex.resize(imageData.height);
    file.rowsIndex.clear(true);

    const size_t rowWidth = (imageData.width + PixelAlignment - 1) / PixelAlignment * PixelAlignment;

    for (size_t i = 0; i < imageData.height; i++)
    {
        for (size_t j = 0; j < imageData.width; j++)
        {
            if (imageData.data[i * rowWidth + j] != 0xff)
            {
                file.rowsIndex.toggleBit(i);
                break;
            }
        }
    }

    const size_t minBuffSize = imageData.width * imageData.height / (PixelAlignment * CHAR_BIT);

    file.encodedData.reserve(minBuffSize);

    for (size_t i = 0; i < file.rowsIndex.size(); i++)
    {
        if (file.rowsIndex.get(i))
            continue;

        BitArray bits(8);
        size_t bitsCursor = 0;

        for (size_t j = 0; j < rowWidth; j += PixelAlignment)
        {
            bool isWhite = true, isBlack = true;

            for (size_t k = 0; k < PixelAlignment; k++)
            {
                if (j + k >= imageData.width)
                    break;

                auto pixel = imageData.data[i * rowWidth + j + k];
                if (pixel != 0xff) isWhite = false;
                if (pixel != 0x00) isBlack = false;
            }

            if (isWhite)
            {
                if (bitsCursor >= bits.size())
                    bits.resize(bits.size() + CHAR_BIT);

                bitsCursor++;
            }
            else if (isBlack)
            {
                if ((int)bits.size() - bitsCursor < 2)
                {
                    bits.resize(bits.size() + CHAR_BIT);
                }

                bits.toggleBit(bitsCursor);
                bitsCursor += 2;
            }
            else
            {
                if ((int)bits.size() - bitsCursor < 2)
                {
                    bits.resize(bits.size() + CHAR_BIT);
                }
                bits.toggleBit(bitsCursor);
                bits.toggleBit(bitsCursor + 1);

                for (int i = 0; i < bits.size() / CHAR_BIT; i++)
                {
                    file.encodedData.push_back(bits.data()[i]);
                }

                for (int k = 0; k < PixelAlignment; k++)
                {
                    auto index = i * rowWidth + j + k;
                    if (index % rowWidth < imageData.width)
                        file.encodedData.push_back(imageData.data[index]);
                }

                bitsCursor = 0;
                bits.clear();
                bits.resize(CHAR_BIT);
            }

            if (bitsCursor > 0 && j == rowWidth - PixelAlignment)
            {
                for (size_t i = 0; i < bits.size() / 8; i++)
                {
                    file.encodedData.push_back(bits.data()[i]);
                }
            }
        }
    }

    file.writeToFile(filePath);
}

RawImageData ImgCodec::decode(const std::string& filePath)
{
    EncodedFile file;
    file.readFromFile(filePath);

    RawImageData imageData;
    imageData.width = file.width;
    imageData.height = file.height;

    const size_t rowWidth = (imageData.width + PixelAlignment - 1) / PixelAlignment * PixelAlignment;
    auto numOfEmptyPixels = rowWidth - imageData.width;

    size_t buffCursor = 0;

    for (size_t i = 0; i < file.rowsIndex.size(); i++)
    {
        if (file.rowsIndex.get(i))
        {
            for (int j = 0; j < imageData.width; j++)
                imageData.data.push_back(0xff);

            for (int k = 0; k < numOfEmptyPixels; k++)
                imageData.data.push_back(0x00);
        }
        else
        {
            for (;;)
            {
                BitArray bits(8);
                bits.setBitsFromArray(file.encodedData.data() + buffCursor, 1);

                for (size_t j = 0; j < CHAR_BIT; j++)
                {
                    if (!bits.get(j))
                    {
                        for (int k = 0; k < PixelAlignment; k++)
                        {
                            if (imageData.data.size() % rowWidth < imageData.width)
                                imageData.data.push_back(0xff);
                        }
                    }
                    else
                    {
                        if (j == CHAR_BIT - 1)
                        {
                            buffCursor++;
                            bits.setBitsFromArray(file.encodedData.data() + buffCursor, 1);
                            j = 0;
                        }
                        else
                        {
                            j++;
                        }

                        if (!bits.get(j))
                        {
                            for (size_t k = 0; k < PixelAlignment; k++)
                            {
                                if (imageData.data.size() % rowWidth < imageData.width)
                                    imageData.data.push_back(0x00);
                            }
                        }
                        else
                        {
                            for (size_t k = 0; k < PixelAlignment; k++)
                            {
                                if (imageData.data.size() % rowWidth < imageData.width)
                                {
                                    buffCursor++;
                                    imageData.data.push_back(*(file.encodedData.data() + buffCursor));
                                }
                            }
                            break;
                        }
                    }
                }
                buffCursor++;

                if (imageData.data.size() % rowWidth == imageData.width)
                {
                    for (int k = 0; k < numOfEmptyPixels; k++)
                        imageData.data.push_back(0x00);

                    break;
                }
            }
        }
    }

    return imageData;
}
