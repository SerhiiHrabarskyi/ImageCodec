#include "imagereader.h"

#include <QFile>

ImageReader::ImageReader(QObject *parent) : QObject(parent)
{
}

void ImageReader::loadImage(const QString &filePath)
{
    if (QFile::exists(filePath)) {
        m_image.load(filePath);
        emit imageChanged();
    } else {
        qWarning() << "File does not exist:" << filePath;
    }
}

QImage ImageReader::getImage() const
{
    return m_image;
}
