import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

ApplicationWindow {
    visible: true
    width: 600
    height: 700

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        Item {
            Column {
                spacing: 20
                anchors.centerIn: parent

                FileSelectorRow{
                    id: originalImage
                    actionButtonText: "Load Image"
                    fileDialogTitle: "Select Image"
                    fileDialogNameFilters: ["Image files (*.bmp)"]

                    onActionButtonClicked: {
                        imageReader.loadImage(path)
                    }
                }

                FileSelectorRow{
                    id: compressedFile
                    actionButtonText: "Compress"
                    fileDialogTitle: "Select Output File"
                    fileDialogNameFilters: ["Barch files (*.barch)"]
                    fileDialogMode: FileDialog.SaveFile

                    onActionButtonClicked: {
                        imageCodec.compressImage(originalImage.path, path)
                    }
                }

                Image {
                    id: image
                    width: 500
                    height: 500
                    fillMode: Image.PreserveAspectFit
                    Connections {
                        target: imageReader
                        onImageChanged: {
                            image.source = ""
                            image.source = "image://imageProviderCompressing/"
                        }
                    }
                }
            }
        }

        Item{
            Column {
                spacing: 20
                anchors.centerIn: parent

                FileSelectorRow{
                    id: fileToDecompress
                    actionButtonText: "Decompress"
                    fileDialogTitle: "Select Input File"
                    fileDialogNameFilters: ["Barch files (*.barch)"]

                    onActionButtonClicked: {
                        imageCodec.decompressImage(path)
                    }
                }

                FileSelectorRow{
                    id: restoredImage
                    actionButtonText: "Save Image"
                    fileDialogTitle: "Select Output Image"
                    fileDialogNameFilters: ["Image files (*.bmp)"]
                    fileDialogMode: FileDialog.SaveFile

                    onActionButtonClicked: {
                        imageCodec.saveImage(path)
                    }
                }

                Image {
                    id: decompressedimage
                    width: 500
                    height: 500
                    fillMode: Image.PreserveAspectFit
                    Connections {
                        target: imageCodec
                        onImageChanged: {
                            decompressedimage.source = ""
                            decompressedimage.source = "image://imageProviderDecompressing/"
                        }
                    }
                }
            }
        }
    }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex

        TabButton {
            text: "Compress"
        }
        TabButton {
            text: "Decompress"
        }
    }

    Dialog {
        id: dialog

        property bool isWorking: false

        title: isWorking ? "Compressing..." : "Compressed"
        modal: true
        anchors.centerIn: parent
        standardButtons: isWorking ? Dialog.NoButton : Dialog.Ok


        ProgressBar {
            id: progressBar
            anchors.fill: parent
            indeterminate: true
        }

        onOpened: {
            isWorking = true
            progressBar.visible = true
        }

        Connections {
            target: imageCodec
            onStartCompressing: {
                dialog.open()
            }

            onFinishCompressing: {
                dialog.isWorking = false
                progressBar.visible = false
            }
        }
    }
}
