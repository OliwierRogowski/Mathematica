import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 1000
    height: 1000
    title: "Mathematica"

    // Ładowanie stylu (np. MainStyle.qml)
    Component.onCompleted: {
        var style = Qt.createQmlObject('import QtQuick 2.15; MainStyle {}', parent);
    }

    // Zawartość okna, którą chcesz wyświetlić
    Rectangle {
        width: parent.width
        height: parent.height

        color: "lightblue"
        Text {
            anchors.centerIn: parent
            text: "Zawartość okna"
            font.pointSize: 20
        }
    }
}
