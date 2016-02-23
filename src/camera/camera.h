#pragma once

#include <dirent.h>

#include "opencv2/opencv.hpp"

namespace upm
{
    class Camera
    {
    public:
        cv::VideoCapture m_camera;
        cv::Mat m_rawImage;
        unsigned char m_cameraId;
        double m_width;
        double m_height;

        bool m_running;
        pthread_t m_grabThread;
        pthread_mutex_t m_mutexLock;

    public:
        Camera(unsigned char in_cameraId, double in_width=0, double in_height=0);
        ~Camera();

        void ptr2String(void* in_ptr, std::string &in_str);
        bool string2Ptr(std::string &in_str, void** in_ptr);

        std::string read();
        std::string shoot();
        bool startCamera();
        void stopCamera();
        bool checkCamera(int in_videoID);
        static void* grabFunc(void* in_data);
    };
}
