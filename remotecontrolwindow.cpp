#include "remotecontrolwindow.h"
#include "ui_remotecontrolwindow.h"
#include "portablesleep.h"

#define PI 3.141592


using namespace std;
using namespace restpp;



RemoteControlWindow::RemoteControlWindow(QMainWindow *parent) :
    QDialog(parent),
    m_parent(parent),
    ui(new Ui::RemoteControlWindow)
{
    ui->setupUi(this);
    // local remote control
    m_robot_ip = "192.168.1.49";

    //remote control
    m_remote_control_enabled = false;
    setFocusPolicy(Qt::NoFocus);
    m_thread_started = false;
    //arrows
    m_up_pressed = false;
    m_down_pressed = false;
    m_right_pressed = false;
    m_left_pressed = false;

    //timer
    m_display_timerId = startTimer(1000); //ms

    // REST
    m_api_token = "d1034f5c-1c84-406b-8313-0ce8dd00fd13";
    m_workspace_id = "17";
    m_device_id = "5324648";

    m_rest.init(m_api_token,1);

}

RemoteControlWindow::~RemoteControlWindow()
{
    delete ui;
}

void RemoteControlWindow::on_backPushButton_clicked()
{
    hide();
    m_thread_started = false;
    m_parent->show();
}

// Push button enable remote/disable control
void RemoteControlWindow::on_pushButton_clicked()
{
    if (!m_thread_started){
        m_get_robot_info_thread = std::thread(&RemoteControlWindow::get_robot_info, this);
        m_send_arrow_cmd_thread = std::thread(&RemoteControlWindow::publish_arrow_cmd, this);
        m_display_live_thread = std::thread(&RemoteControlWindow::display_live_video, this);
        m_thread_started = true;
    }

    std::string shell_script = "~/People_tracking_with_thread/remote_control_signal.sh";
    std::string enable_remote_control_bool;
    std::string device_parameters = "https://cloud-eu-1.stereolabs.com/api/v1 17 5324648 d1034f5c-1c84-406b-8313-0ce8dd00fd13";

    if (m_remote_control_enabled){
        m_remote_control_enabled = false;

        // Stop receiving keyboard input
        disable_remote_control();

        // Change button text
        ui->pushButton->setText("Enable Remote control");

        // Send info to CMP
        enable_remote_control_bool = " false ";
        std::system((shell_script + enable_remote_control_bool + device_parameters).c_str());
        std::system((shell_script + enable_remote_control_bool + device_parameters).c_str());
    }
    else {
        m_remote_control_enabled = true;

        // Start receiving keyboard input
        enable_remote_control();

        // Change button text
        ui->pushButton->setText("Back to automatic control");

        // Send info to CMP
        enable_remote_control_bool = " true ";
        std::system((shell_script + enable_remote_control_bool + device_parameters).c_str());
        std::system((shell_script + enable_remote_control_bool + device_parameters).c_str());
    }
}


void RemoteControlWindow::enable_remote_control()
{
    setFocusPolicy(Qt::StrongFocus);
}

void RemoteControlWindow::disable_remote_control()
{
    setFocusPolicy(Qt::NoFocus);
    ui->arrow_down->setStyleSheet("QLabel { background-color : red; color : blue; }");
    ui->arrow_up->setStyleSheet("QLabel { background-color : red; color : blue; }");
    ui->arrow_right->setStyleSheet("QLabel { background-color : red; color : blue; }");
    ui->arrow_left->setStyleSheet("QLabel { background-color : red; color : blue; }");
}


void RemoteControlWindow::keyPressEvent(QKeyEvent *e)
{
    if (m_remote_control_enabled){
        switch (e->key()){
        case Qt::Key_Up:
            m_up_pressed = true;
            ui->arrow_up->setStyleSheet("QLabel { background-color : green; color : blue; }");
            break;

        case Qt::Key_Down:
            m_down_pressed = true;
            ui->arrow_down->setStyleSheet("QLabel { background-color : green; color : blue; }");
            break;

        case Qt::Key_Right:
            m_right_pressed = true;
            ui->arrow_right->setStyleSheet("QLabel { background-color : green; color : blue; }");
            break;

        case Qt::Key_Left:
            m_left_pressed = true;
            ui->arrow_left->setStyleSheet("QLabel { background-color : green; color : blue; }");
            break;
        }
    }
}

void RemoteControlWindow::keyReleaseEvent(QKeyEvent *e)
{
    if (m_remote_control_enabled){
        switch (e->key()){
            case Qt::Key_Up:
                m_up_pressed = false;
                ui->arrow_up->setStyleSheet("QLabel { background-color : red; color : blue; }");
                break;
            case Qt::Key_Down:
                m_down_pressed = false;
                ui->arrow_down->setStyleSheet("QLabel { background-color : red; color : blue; }");
                break;

            case Qt::Key_Right:
                m_right_pressed = false;
                ui->arrow_right->setStyleSheet("QLabel { background-color : red; color : blue; }");
                break;

            case Qt::Key_Left:
                m_left_pressed = false;
                ui->arrow_left->setStyleSheet("QLabel { background-color : red; color : blue; }");
                break;
        }
    }
}


void RemoteControlWindow::timerEvent(QTimerEvent *e)
{
    if (m_remote_control_enabled){
        //Display robot pos
        ui->robot_position_value->setText(("x = " + m_x_pos.substr(0, 4) + " m \ny = " + m_y_pos.substr(0, 4) + " m \nz = " + m_z_pos.substr(0, 4) + " m").c_str());

        // Display costmap + robot direction
        guardian.lock();

        int image_width = 260;
        int image_height = 260;

        QPixmap myPix = QPixmap::fromImage(m_costmap_image).scaled(image_width, image_height);

        QPainter painter(&myPix);
        QTransform trans;
        trans.translate(image_width/2, image_height/2);
        trans.rotate( - m_robot_angles.yaw / (2 * PI) * 360);
        painter.setTransform(trans);

        int arrow_width = 75;
        int arrow_height = 75;
        QRectF target(-arrow_width/2, -arrow_height/2, arrow_width, arrow_height);
        QRectF source(0.0, 0.0, 70.0, 70.0);
        QImage arrow("/home/louis/Pictures/arrows/down_arrow.png");

        painter.drawImage(target, arrow, source);
        ui->costmap_label->setPixmap(myPix);

        guardian.unlock();
    }
}


void RemoteControlWindow::get_robot_info(){
    while(m_remote_control_enabled){
        // Odometry
        std::chrono::steady_clock::time_point begin2 = std::chrono::steady_clock::now();
        // init
        std::string method = "GET";
        std::string telemetry_label = "my_robot_odometry";
        std::string url ="https://cloud-eu-1.stereolabs.com/api/v1/workspaces/" + m_workspace_id + "/telemetry?labels=" + telemetry_label + "&sort=timestamp&sort_direction=DESC&limit=1";
        RestResponse response = m_rest.performRequest(method,url);
        if (response.getOutput() == "null"){
            throw response.getOutput();
        }
        response = m_rest.performRequest(method,url);
        restpp::json output = response.getOutput();
        //std::cerr << output["telemetry"][0]["payload"] << std::endl;
        //display elapsed time
        std::chrono::steady_clock::time_point end2 = std::chrono::steady_clock::now();
        std::cout << "Elapsed time receiving odometry : "
            << std::chrono::duration_cast<chrono::milliseconds>(end2 - begin2).count()
            << " ms" << std::endl;

        // Display odometry
        m_x_pos = to_string(output["telemetry"][0]["payload"]["position"]["x"].get<double>());
        m_y_pos = to_string(output["telemetry"][0]["payload"]["position"]["y"].get<double>());
        m_z_pos = to_string(output["telemetry"][0]["payload"]["position"]["z"].get<double>());

        m_robot_q.w = output["telemetry"][0]["payload"]["orientation"]["w"].get<double>();
        m_robot_q.x = output["telemetry"][0]["payload"]["orientation"]["x"].get<double>();
        m_robot_q.y = output["telemetry"][0]["payload"]["orientation"]["y"].get<double>();
        m_robot_q.z = output["telemetry"][0]["payload"]["orientation"]["z"].get<double>();

        m_robot_angles = ToEulerAngles(m_robot_q);

        ///////////////////////////////////////////////
        //costmap
        telemetry_label = "my_robot_costmap";
        url ="https://cloud-eu-1.stereolabs.com/api/v1/workspaces/" + m_workspace_id + "/telemetry?labels=" + telemetry_label + "&sort=timestamp&sort_direction=DESC&limit=1";


        RestResponse costmap_response = m_rest.performRequest(method,url);
        if (costmap_response.getOutput() == "null"){
            throw costmap_response.getOutput();

        }

        costmap_response = m_rest.performRequest(method,url);
        restpp::json costmap_output = costmap_response.getOutput();

        int height = costmap_output["telemetry"][0]["payload"]["height"].get<int>();
        int width = costmap_output["telemetry"][0]["payload"]["width"].get<int>();

//        method = "POST";
//        std::string function_name = "costmap_topic_function";
//        url ="https://cloud-eu-1.stereolabs.com/api/v1/workspaces/" + m_workspace_id + "/devices/" + m_device_id + "/functions/" + function_name;
//        std::cerr << url<< std::endl;

//        restpp::json message_sent;
//        message_sent["parameters"] = "";
//        RestResponse costmap_response = m_rest.performRequest(method,url, message_sent.dump());
//        if (costmap_response.getOutput() == "null"){
//            throw costmap_response.getOutput();
//        }

//        costmap_response = m_rest.performRequest(method,url, message_sent.dump());
//        restpp::json costmap_output = costmap_response.getOutput();

//        int height = costmap_output["result"]["height"].get<int>();
//        int width = costmap_output["result"]["width"].get<int>();

        my_costmap_image = cv::Mat(height, width, CV_8UC1);

        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                my_costmap_image.at<uchar>(j, i) = 255 - floor(costmap_output["telemetry"][0]["payload"]["data"][i * width + j].get<int>() * 255 /100);
            }
        }
        guardian.lock();
        m_costmap_image = QImage(my_costmap_image.data, my_costmap_image.cols, my_costmap_image.rows, my_costmap_image.step, QImage::Format_Indexed8);
        guardian.unlock();
    }
    //std::terminate();
}

void RemoteControlWindow::publish_arrow_cmd()
{
    while(m_remote_control_enabled){
        qDebug() << "entering while loop";
        std::string arrow_direction;
        bool key_received = false;

        /******************* Send arrow command to robot  *********************************/
        if(m_up_pressed && m_right_pressed && !m_left_pressed && !m_down_pressed){
            arrow_direction = " up_right ";
            key_received = true;
        }

        else if(m_up_pressed && m_left_pressed && !m_right_pressed && !m_down_pressed){
            arrow_direction = " up_left ";
            key_received = true;
        }

        else if(m_down_pressed && m_right_pressed && !m_left_pressed && !m_up_pressed){
            arrow_direction = " down_right ";
            key_received = true;
        }

        else if(m_down_pressed && m_left_pressed && !m_right_pressed && !m_up_pressed){
            arrow_direction = " down_left ";
            key_received = true;
        }

        else if(m_up_pressed && !m_down_pressed && !m_right_pressed && !m_left_pressed){
            arrow_direction = " up ";
            key_received = true;
        }

        else if(m_down_pressed && !m_up_pressed && !m_right_pressed && !m_left_pressed){
            arrow_direction = " down ";
            key_received = true;
        }

        else if(m_right_pressed && !m_up_pressed && !m_down_pressed && !m_left_pressed){
            arrow_direction = " right ";
            key_received = true;
        }

        else if(m_left_pressed && !m_up_pressed && !m_down_pressed && !m_right_pressed){
            arrow_direction = " left ";
            key_received = true;

        }

        cout << arrow_direction << endl;
        std::chrono::steady_clock::time_point begin1 = std::chrono::steady_clock::now();

        // send command to robot
        if (key_received){
            std::string shell_script = "~/People_tracking_with_thread/arrow_direction.sh";
            std::string device_parameters = "https://cloud-eu-1.stereolabs.com/api/v1 " + m_workspace_id +  " " + m_device_id +  " " + m_api_token;
            std::system((shell_script + arrow_direction + device_parameters).c_str());
        }
        else{
            PortableSleep::msleep(500);
        }
        // elapsed time
        std::chrono::steady_clock::time_point end1 = std::chrono::steady_clock::now();
        std::cout << "Elapsed time sending arrow : "
            << std::chrono::duration_cast<chrono::milliseconds>(end1 - begin1).count()
            << " ms" << std::endl;
    }
}

void RemoteControlWindow::display_live_video(){
    sl::VIEW view_mode = sl::VIEW::LEFT;
    sl::Camera zed;
    // Set configuration parameters for the ZED
    sl::InitParameters init_parameters;
    init_parameters.depth_mode = sl::DEPTH_MODE::PERFORMANCE;
    init_parameters.coordinate_system = sl::COORDINATE_SYSTEM::RIGHT_HANDED_Y_UP; // OpenGL's coordinate system is right_handed
    init_parameters.sdk_verbose = true;

    string stream_params;
    stream_params = m_robot_ip;
    init_parameters.input.setFromStream(m_robot_ip);

    // Open the camera
    sl::ERROR_CODE zed_open_state = zed.open(init_parameters);
//    if (zed_open_state != sl::ERROR_CODE::SUCCESS) {
//        print("Camera Open", zed_open_state, "Exit program.");
//        return sl::EXIT_FAILURE;
//    }
    qDebug() << "Open the camera ok";

    // Create a Mat to store images
    sl::Mat zed_image;

    // Initialise camera setting
    //switchCameraSettings();

    // Capture new images until 'q' is pressed

    while (m_remote_control_enabled) {
        // Check that grab() is successful
        auto returned_state = zed.grab();
        if (returned_state == sl::ERROR_CODE::SUCCESS) {
            // Retrieve left image
            zed.retrieveImage(zed_image, view_mode);

            // Convert sl::Mat to cv::Mat (share buffer)
            cv::Mat my_live_video_frame;
            if (zed_image.getChannels() == 1)
                my_live_video_frame = cv::Mat((int) zed_image.getHeight(), (int) zed_image.getWidth(), CV_8UC1, zed_image.getPtr<sl::uchar1>(sl::MEM::CPU));
            else
                my_live_video_frame = cv::Mat((int) zed_image.getHeight(), (int) zed_image.getWidth(), CV_8UC4, zed_image.getPtr<sl::uchar1>(sl::MEM::CPU));

        }
        else {
            std::cout << "Error during capture, break" << std::endl;
            break;
        }
    }

    // Exit
    zed.close();
    std::terminate();
}

EulerAngles RemoteControlWindow::ToEulerAngles(Quaternion q) {

    EulerAngles angles;

    // roll (x-axis rotation)
    double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
    double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
    angles.roll = std::atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    double sinp = 2 * (q.w * q.y - q.z * q.x);
    if (std::abs(sinp) >= 1)
        angles.pitch = std::copysign(M_PI / 2, sinp); // use 90 degrees if out of range
    else
        angles.pitch = std::asin(sinp);

    // yaw (z-axis rotation)
    double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
    double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
    angles.yaw = std::atan2(siny_cosp, cosy_cosp);

    return angles;
}



