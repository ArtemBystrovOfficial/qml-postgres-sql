#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <qml_init.hpp>

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    QmlSingletonModels::Instanse();

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/MainWindow.qml")));

    return app.exec();
}
