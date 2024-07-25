import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs

Row {
    spacing: 10
    height: 30

    property alias actionButtonText: actionButton.text
    property alias fileDialogTitle: fileDialog.title

    property alias fileDialogMode: fileDialog.fileMode
    property alias fileDialogNameFilters: fileDialog.nameFilters

    property alias path: filePath.text

    signal actionButtonClicked()

    TextField {
        id: filePath
        width: 300
        height: parent.height
        placeholderText: "Enter file path or use button to browse..."
    }

    Button {
        text: "Browse"
        height: parent.height
        onClicked: {
            fileDialog.open()
        }
    }

    Button {
        id: actionButton
        height: parent.height
        text: "Action"
        onClicked: {
            actionButtonClicked()
        }
    }

    FileDialog {
        id: fileDialog
        nameFilters: ["All files (*)"]
        onAccepted: {
            filePath.text = fileDialog.selectedFile.toString().replace("file:///", "")
        }
    }
}
