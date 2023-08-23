#include <QMouseEvent>
#include <QApplication>
#include "mainwindow.h"

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        // Store the position relative to the window's top-left corner
        dragStartPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        // Calculate the new position of the window based on the mouse movement
        QPoint newPos = event->globalPosition().toPoint() - dragStartPosition;
        move(newPos);
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    // Add any logic you need when the mouse button is released
    // ...

    // Call the base class implementation
    QMainWindow::mouseReleaseEvent(event);
}
