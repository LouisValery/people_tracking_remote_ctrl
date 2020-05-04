#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "remotecontrolwindow.h"
#include "livevideowindow.h"
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


void MainWindow::on_remoteButton_clicked()
{
    hide();
    RemoteControlWindow *my_remote_control_window  = new RemoteControlWindow(this);
    my_remote_control_window->setModal(true);
    my_remote_control_window->exec();
}

void MainWindow::on_live_button_clicked()
{
    hide();
    LiveVideoWindow *my_live_video_window = new LiveVideoWindow(this);
    my_live_video_window->setModal(true);
    my_live_video_window->exec();
}


