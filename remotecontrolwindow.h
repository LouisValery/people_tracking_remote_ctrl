#ifndef REMOTECONTROLWINDOW_H
#define REMOTECONTROLWINDOW_H

#define _USE_MATH_DEFINES
#include <cmath>
#include <unistd.h>
//opencv
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

//zed sl lib
#include <sl/Camera.hpp>

//Qt
#include <QMainWindow>
#include <QDebug>
#include <QKeyEvent>
#include <QDialog>
#include <QLabel>
#include <QString>
#include <QObject>
#include <QApplication>
#include <QTimer>
#include <QMutex>
#include <QPainter>

// Standard includes
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <thread>

#include <restpp/RestRequester.hpp>


struct Quaternion {
    double w, x, y, z;
};

struct EulerAngles {
    double roll, pitch, yaw;
};


namespace Ui {
class RemoteControlWindow;
}

class RemoteControlWindow : public QDialog
{
    Q_OBJECT

public:
    explicit RemoteControlWindow(QMainWindow *parent);
    void enable_remote_control();
    void disable_remote_control();
    ~RemoteControlWindow();

private slots:
    void on_pushButton_clicked();
    void on_backPushButton_clicked();

public slots:
    //threads functions
    void publish_arrow_cmd();
    void get_robot_info();
    void display_live_video();

private:
    QMainWindow* m_parent;

    void connectSignalsSlots();
    EulerAngles ToEulerAngles(Quaternion q);

    Ui::RemoteControlWindow *ui;

    bool m_remote_control_enabled;
    // local remote control
    sl::String m_robot_ip;


    // arrows
    bool m_up_pressed;
    bool m_down_pressed;
    bool m_right_pressed;
    bool m_left_pressed;

    // timers
    int m_display_timerId;

    // odometry
    std::string m_x_pos;
    std::string m_y_pos;
    std::string m_z_pos;

    Quaternion m_robot_q;
    EulerAngles m_robot_angles;

    // costmap
    QImage m_costmap_image;
    cv::Mat my_costmap_image;

    // live video
    cv::Mat my_live_video_frame;

    // threads
    std::thread m_get_robot_info_thread;
    std::thread m_send_arrow_cmd_thread;
    std::thread m_display_live_thread;
    bool m_thread_started;
    QMutex guardian;

    // Application params
    restpp::RestRequester m_rest;
    std::string m_api_token;
    std::string m_workspace_id;
    std::string m_device_id;

protected:
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void timerEvent(QTimerEvent *e) override;


};

#endif // REMOTECONTROLWINDOW_H
