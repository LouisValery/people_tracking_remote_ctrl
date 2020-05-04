#include "livevideowindow.h"
#include "mainwindow.h"
#include "ui_livevideowindow.h"

LiveVideoWindow::LiveVideoWindow(QMainWindow *parent) :
    QDialog(parent),
    m_parent(parent),
    ui(new Ui::LiveVideoWindow)
{
    ui->setupUi(this);
    m_live_started = false;
    m_robot_ip = "192.168.1.49";
    //timer
    m_display_timerId = startTimer(100); //ms
}

LiveVideoWindow::~LiveVideoWindow()
{
    delete ui;
}

void LiveVideoWindow::on_backPushButton_clicked()
{
    hide();
    m_live_started = false;
    m_parent->show();
}

void LiveVideoWindow::on_startPushButton_clicked()
{
    if (!m_live_started){
        m_display_live_thread = std::thread(&LiveVideoWindow::display_live_video, this);
        m_live_started = true;
    }
}

void LiveVideoWindow::on_stopPushButton_clicked()
{
    m_live_started = false;
}

void LiveVideoWindow::timerEvent(QTimerEvent *e)
{
    if (m_live_started){

        // Display live_video

        guardian.lock();

        int image_width = 820;
        int image_height = 480;

        QPixmap myPix = QPixmap::fromImage(m_frame_Qimage).scaled(image_width, image_height);
        ui->video_label->setPixmap(myPix);

        guardian.unlock();
    }

}

void LiveVideoWindow::display_live_video(){
    sl::VIEW view_mode = sl::VIEW::LEFT;
    sl::Camera zed;
    // Set configuration parameters for the ZED
    sl::InitParameters init_parameters;
    init_parameters.depth_mode = sl::DEPTH_MODE::PERFORMANCE;
    init_parameters.coordinate_system = sl::COORDINATE_SYSTEM::RIGHT_HANDED_Y_UP; // OpenGL's coordinate system is right_handed
    init_parameters.sdk_verbose = true;

    sl::String stream_params;
    stream_params = m_robot_ip;
    init_parameters.input.setFromStream(m_robot_ip);

    // Open the camera
     sl::ERROR_CODE zed_open_state = zed.open(init_parameters);
//    if (zed_open_state != sl::ERROR_CODE::SUCCESS) {
//        print("Camera Open", zed_open_state, "Exit program.");
//        return sl::EXIT_FAILURE;
//    }
    std::cout << "Open the camera ok" << std::endl;

    // Create a Mat to store images
    sl::Mat zed_image;

    // Initialise camera setting
    //switchCameraSettings();

    // Capture new images until 'q' is pressed
    cv::Mat my_live_video_frame;

    while (m_live_started) {
        // Check that grab() is successful
        auto returned_state = zed.grab();
        if (returned_state == sl::ERROR_CODE::SUCCESS) {
            // Retrieve left image
            zed.retrieveImage(zed_image, view_mode);
            guardian.lock();
            // Convert sl::Mat to cv::Mat (share buffer)
            if (zed_image.getChannels() == 1){
                my_live_video_frame = cv::Mat((int) zed_image.getHeight(), (int) zed_image.getWidth(), CV_8UC1, zed_image.getPtr<sl::uchar1>(sl::MEM::CPU));
                m_frame_Qimage= QImage(my_live_video_frame.data, my_live_video_frame.cols, my_live_video_frame.rows, my_live_video_frame.step, QImage::Format_Indexed8);
            }
            else{
                my_live_video_frame = cv::Mat((int) zed_image.getHeight(), (int) zed_image.getWidth(), CV_8UC4, zed_image.getPtr<sl::uchar1>(sl::MEM::CPU));
                m_frame_Qimage = QImage(my_live_video_frame.data, my_live_video_frame.cols, my_live_video_frame.rows, my_live_video_frame.step, QImage::Format_RGB32);
            }
            //cv::imshow("my live", my_live_video_frame);
            guardian.unlock();
        }
        else {
//            print("Error during capture : ", returned_state);
            break;
        }
    }

    // Exit
    zed.close();
}

