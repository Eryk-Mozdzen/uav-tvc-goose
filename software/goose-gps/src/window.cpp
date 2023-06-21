#include "window.h"
#include <QMessageBox>
#include <QWebEngineSettings>
#include <QtCore>

Window::Window(QWidget *parent) : QMainWindow{parent}, view{new QWebEngineView(this)}, loaded{false} {
    setCentralWidget(view);
    resize(640, 480);

    QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);

    QWebEnginePage *page = view->page();

    connect(page, &QWebEnginePage::loadFinished, this, &Window::pageLoaded);
    page->load(QUrl(QStringLiteral("qrc:/file.html")));

    connect(&timer, &QTimer::timeout, this, &Window::timerCallback);
    timer.setInterval(100);
    timer.start();
}

void Window::addMarker(double latitude, double longitude) {
    if(loaded) {
        QWebEnginePage *page = view->page();

        page->runJavaScript(QString("addMarker(%1, %2);").arg(QString::number(latitude, 'f', 20)).arg(QString::number(longitude, 'f', 20)));
    }
}

void Window::pageLoaded() {
    loaded = true;
}

void Window::timerCallback() {
    const double lat = 51.103525733902586 + QRandomGenerator::global()->generateDouble()*0.0001 - 0.00005;
    const double lng = 17.085345490751223 + QRandomGenerator::global()->generateDouble()*0.0001 - 0.00005;

    addMarker(lat, lng);
}
