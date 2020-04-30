#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "remotecontrolwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    hide();
    RemoteControlWindow my_remote_control_window;
    my_remote_control_window.setModal(true);
    my_remote_control_window.exec();


//    while(true){

//    }
}
