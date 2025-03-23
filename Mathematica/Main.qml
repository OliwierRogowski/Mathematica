import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: root

    property bool expandPath: false
    property bool showLineNumbers: true
    property string currentFilePath: ""

    width: 1100
    height: 10000
    minimumWidth: 1000
    minimumHeight: 1000
    visible: true
    flags: Qt.Window | Qt.FramelessWindowHint
    title: qsTr("Mathematicaaaa")
}
