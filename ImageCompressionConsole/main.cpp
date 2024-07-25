#include <QCoreApplication>

#include <QString>
#include <QImage>
#include <QCommandLineParser>
#include <QDebug>

#include <vector>

#include "imgcodec.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCommandLineParser parser;

    QCommandLineOption encodeModeOption(QStringList() << "e" << "encode", "Encode mode");
    parser.addOption(encodeModeOption);

    QCommandLineOption decodeModeOption(QStringList() << "d" << "decode", "Decode mode");
    parser.addOption(decodeModeOption);

    QCommandLineOption inputFileOption(QStringList() << "i" << "input", "Input file", "Input file path");
    parser.addOption(inputFileOption);

    QCommandLineOption outputFileOption(QStringList() << "o" << "output", "Output file", "Output file path");
    parser.addOption(outputFileOption);

    parser.parse(a.arguments());

    if (!parser.isSet(inputFileOption))
    {
        qCritical() << "Error: Input file path is missing.";
        parser.showHelp(1);
    }

    if (!parser.isSet(outputFileOption))
    {
        qCritical() << "Error: Output file path is missing.";
        parser.showHelp(1);
    }

    if (!(parser.isSet(encodeModeOption) ^ parser.isSet(decodeModeOption)))
    {
        qCritical() << "Error: Mode (encode/decode) is missing.";
        parser.showHelp(1);
    }

    if (parser.isSet(encodeModeOption))
    {
        QImage img(parser.value(inputFileOption));

        auto rawImageData = ImgCodec::RawImageData{img.width(),
                                                   img.height(),
                                                   std::vector(img.bits(), img.bits() + img.sizeInBytes())};

        ImgCodec::encode(rawImageData, parser.value(outputFileOption).toStdString());

        qInfo() << "Image encoded";
    }
    else if (parser.isSet(decodeModeOption))
    {
        auto rawData = ImgCodec::decode(parser.value(inputFileOption).toStdString());
        auto m_image = QImage(rawData.data.data(), rawData.width, rawData.height, QImage::Format_Grayscale8);
        m_image.save(parser.value(outputFileOption));

        qInfo() << "Image decoded";
    }

    return 0;
}
