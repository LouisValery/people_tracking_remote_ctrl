#ifndef LIVEVIDEOWINDOW_H
#define LIVEVIDEOWINDOW_H

#include <QDialog>
#include <QMainWindow>
#include <QMutex>

// Standard includes
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <thread>

//opencv
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

//zed sl lib
#include <sl/Camera.hpp>


namespace Ui {
class LiveVideoWindow;
}

class LiveVideoWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LiveVideoWindow( QMainWindow* parent );
    ~LiveVideoWindow();

private slots:
    void on_backPushButton_clicked();
    void on_startPushButton_clicked();
    void on_stopPushButton_clicked();

public slots:
   void display_live_video();

protected:
    void timerEvent(QTimerEvent *e) override;

private:
    QMainWindow* m_parent;

    Ui::LiveVideoWindow *ui;

    std::thread m_display_live_thread;
    bool m_live_started;
    QMutex guardian;

    QImage m_frame_Qimage;
    sl::String m_robot_ip;
    int m_display_timerId;


};

#endif // LIVEVIDEOWINDOW_H
