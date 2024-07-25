#ifndef IMAGECODEC_H
#define IMAGECODEC_H

#include <QObject>
#include <QImage>

class ImageCodec : public QObject
{
    Q_OBJECT
public:
    explicit ImageCodec(QObject *parent = nullptr);

    QImage getImage() { return m_image; }

public slots:
    void compressImage(const QString& imagePath, const QString& outputPath);
    void decompressImage(const QString& imagePath);

    void saveImage(const QString& imagePath);

signals:
    void imageChanged();

    void startCompressing();
    void finishCompressing();

private:
    QImage m_image;
};

#endif // IMAGECODEC_H
