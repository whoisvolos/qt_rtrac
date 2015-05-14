#include "glwidget.h"
#include "window.h"
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QPushButton>
#include <QApplication>
#include <QMessageBox>
#include <QtWidgets/qfiledialog.h>

Window::Window(MainWindow *mw)
        : mainWindow(mw)
{
    glWidget = new GLWidget;

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(glWidget);

    openModelButton = new QPushButton(tr("Load model"), this);
    connect(openModelButton, SIGNAL(clicked()), this, SLOT(loadModel()));
    mainLayout->addWidget(openModelButton);

    setLayout(mainLayout);

    setWindowTitle(tr("Model viewer"));
}

void Window::loadModel() {
    using namespace std;

    QFileDialog* fileDialog = new QFileDialog(this);

    fileDialog->setFileMode(QFileDialog::ExistingFile);
    fileDialog->setNameFilter(QString::fromLocal8Bit("Model (*.obj)"));

    QStringList fileNames;

    if (fileDialog->exec()) {
        fileNames = fileDialog->selectedFiles();
        if (fileNames.size() != 0) { // model loading
            string filePath = string(fileNames.at(0).toLocal8Bit().constData());
            emit glWidget->modelLoading(filePath.c_str());
        }
    }

    delete fileDialog;
}