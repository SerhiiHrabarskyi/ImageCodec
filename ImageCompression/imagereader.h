#ifndef IMAGEREADER_H
#define IMAGEREADER_H

#include <QObject>
#include <QImage>
#include <QQuickImageProvider>

class ImageReader : public QObject
{
    Q_OBJECT
public:
    explicit ImageReader(QObject *parent = nullptr);

    Q_INVOKABLE void loadImage(const QString &filePath);

    QImage getImage() const;

signals:
    void imageChanged();

private:
    QImage m_image;
};

#endif // IMAGEREADER_H
