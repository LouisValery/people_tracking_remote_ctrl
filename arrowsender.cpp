#include "arrowsender.h"

ArrowSender::ArrowSender()
{

}

void ArrowSender::publish_arrow_cmd()
{
    while(true){
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


        std::chrono::steady_clock::time_point begin1 = std::chrono::steady_clock::now();

        // send command to robot
        if (key_received){
            std::string shell_script = "~/People_tracking/arrow_direction.sh";
            std::string device_parameters = "https://cloud-eu-1.stereolabs.com/api/v1 " + m_workspace_id +  " " + m_device_id +  " " + m_api_token;
            std::system((shell_script + arrow_direction + device_parameters).c_str());
        }

        std::chrono::steady_clock::time_point end1 = std::chrono::steady_clock::now();
        std::cout << "Elapsed time sending arrow : "
            << std::chrono::duration_cast<chrono::milliseconds>(end1 - begin1).count()
            << " ms" << std::endl;
    }
}
