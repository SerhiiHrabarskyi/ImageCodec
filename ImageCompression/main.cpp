#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QQmlContext>
#include <QQuickStyle>
#include "imagereader.h"
#include "imageprovider.h"
#include "imagecodec.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    ImageReader imageReader;
    engine.rootContext()->setContextProperty("imageReader", &imageReader);

    ImageCodec imageCodec;
    engine.rootContext()->setContextProperty("imageCodec", &imageCodec);

    ImageProvider<ImageReader> *imageProviderCompressing = new ImageProvider(&imageReader);
    engine.addImageProvider("imageProviderCompressing", imageProviderCompressing);

    ImageProvider<ImageCodec> *imageProviderDecompressing = new ImageProvider(&imageCodec);
    engine.addImageProvider("imageProviderDecompressing", imageProviderDecompressing);

    QQuickStyle::setStyle("Basic");

    const QUrl url(QStringLiteral("qrc:/ImageCompression/Main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
