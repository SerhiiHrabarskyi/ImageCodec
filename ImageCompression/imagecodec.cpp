#include "imagecodec.h"

#include <QFileInfo>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>

#include <vector>

#include "imgcodec.h"

ImageCodec::ImageCodec(QObject *parent) : QObject(parent)
{

}

void ImageCodec::compressImage(const QString& imagePath, const QString& outputPath)
{
    emit startCompressing();

    QFuture task = QtConcurrent::task([this, imagePath, outputPath]() {
        QImage img(imagePath);
        ImgCodec::encode({img.width(), img.height(), std::vector(img.bits(), img.bits() + img.sizeInBytes())}, outputPath.toUtf8().toStdString());
    }).spawn().then(this, [this]() {
        emit finishCompressing();
    });
}

void ImageCodec::decompressImage(const QString& imagePath)
{
    auto rawData = ImgCodec::decode(imagePath.toUtf8().toStdString());

    m_image = QImage(rawData.data.data(), rawData.width, rawData.height, QImage::Format_Grayscale8).copy();

    emit imageChanged();
}

void ImageCodec::saveImage(const QString& imagePath)
{
    m_image.save(imagePath);
}
