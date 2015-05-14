#pragma once

#include <QWidget>

class QPushButton;

class GLWidget;
class MainWindow;

class Window : public QWidget
{
Q_OBJECT

public:
    Window(MainWindow *mw);

private slots:
    void loadModel();

private:
    GLWidget *glWidget;
    QPushButton *openModelButton;
    MainWindow *mainWindow;
};