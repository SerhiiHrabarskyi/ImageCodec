#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QQuickImageProvider>

template <class Source>
class ImageProvider : public QQuickImageProvider
{
public:
    ImageProvider(Source *source)
        : QQuickImageProvider(QQuickImageProvider::Image)
        , m_source(source)
    { }

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override
    {
        Q_UNUSED(id)
        Q_UNUSED(requestedSize)

        QImage image = m_source->getImage();
        if (size) {
            *size = image.size();
        }

        return image;
    }

private:
    Source *m_source = nullptr;
};

#endif // IMAGEPROVIDER_H
