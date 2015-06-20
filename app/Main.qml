import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQml 2.2

ApplicationWindow {
    id: appwindow
    title: qsTr(Qt.application.organization + "-" + Qt.application.name + "-"
                + Qt.application.version + "-" + Qt.platform.os)
    width: 400
    height: 200
    visible: true

    Button {
        id: button
        text: "Start"
        enabled: ConsoleRunner.isRunning()
    }

    Component.onCompleted: {
        button.clicked.connect(ConsoleRunner.onRun)
    }
}
