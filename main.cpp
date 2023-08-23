#include "mainwindow.h"
#include <QMainWindow>
#include <QApplication>
#include <QPalette>

#include <QWidget>
#include <QPainter>
#include <QRegion>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QPalette palette;

    // Set the desired font color
    QColor fontColor(Qt::black); // You can change the color to your preference

    // Set the font color for all widget types
    palette.setColor(QPalette::WindowText, fontColor);
    palette.setColor(QPalette::Text, fontColor);
    palette.setColor(QPalette::ButtonText, fontColor);

    // Apply the palette to the application
    a.setPalette(palette);

    MainWindow w;

    //Window customization
    //w.setWindowFlags(w.windowFlags() | Qt::FramelessWindowHint);
    //w.setAttribute(Qt::WA_TranslucentBackground);
    w.setFixedSize(w.size());

    w.show();
    return a.exec();
}
