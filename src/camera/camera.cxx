#include <iostream>
#include <sstream>
#include <pthread.h>

#include "camera.h"

using namespace upm;

Camera::Camera(unsigned char in_cameraId, double in_width, double in_height)
{
    m_running = false;
    m_cameraId = in_cameraId;
    m_width = in_width;
    m_height = in_height;
}

Camera::~Camera()
{
    stopCamera();
}

bool Camera::checkCamera(int in_videoID)
{
    bool result = false;

    DIR* videoDirPtr;
    const char videoDir[] = "/sys/class/video4linux";
    struct dirent* element = NULL;

    std::stringstream fileStream;
    fileStream << "video" << in_videoID;
    std::string fileString = fileStream.str();

    videoDirPtr = opendir(videoDir);
    if(videoDirPtr == NULL)
    {
        std::cout << "videoDirPtr NULL" << std::endl;
    }

    while( (element=readdir(videoDirPtr)) != NULL )
    {
        if(strcmp(fileString.c_str(), element->d_name) == 0)
        {
            result = true;
        }
    }

    closedir(videoDirPtr);
    return result;
}

void* Camera::grabFunc(void* in_data)
{
    Camera* in_class = (Camera*)(in_data);

    while(in_class->m_running)
    {
        if (in_class->checkCamera(in_class->m_cameraId))
        {
            pthread_mutex_lock(&in_class->m_mutexLock);
            in_class->m_camera.grab();
            pthread_mutex_unlock(&in_class->m_mutexLock);
        }
        else
        {
            std::cout << "camera error....." << std::endl;
            in_class->m_running = false;
            pthread_mutex_lock(&in_class->m_mutexLock);
            in_class->m_camera.release();
            pthread_mutex_lock(&in_class->m_mutexLock);
        }
    }
}

void Camera::ptr2String(void* in_ptr, std::string &in_str)
{
    static unsigned long ptr2Number;
    std::stringstream number2Stream;

    ptr2Number = (unsigned long)in_ptr;
    number2Stream << ptr2Number;
    in_str = "Camera:" + number2Stream.str();
}

bool Camera::string2Ptr(std::string &in_str, void** in_ptr)
{
    static std::string head;
    static std::string strCut;
    static std::istringstream strStream;
    static unsigned long number;

    head.assign(in_str, 0, 7);
    if(head != "Camera:")
        return false;

    strCut.assign(in_str, 7, in_str.length());
    strStream.str(strCut);
    strStream >> number;
    *in_ptr = (void*)(number);
    return true;
}

bool Camera::startCamera()
{
    if(m_running != true)
    {
        m_camera.open(m_cameraId);
        if(m_camera.isOpened())
        {
            m_camera.set(cv::CAP_PROP_FRAME_WIDTH, m_width);
            m_camera.set(cv::CAP_PROP_FRAME_HEIGHT, m_height);
            m_running = true;
            pthread_mutex_init(&m_mutexLock, NULL);
            pthread_create(&m_grabThread, NULL, grabFunc, this);
        }
        else
            return false;
    }

    return true;
}

void Camera::stopCamera()
{
    if(m_running)
    {
        m_running = false;
        void* result;
        pthread_join(m_grabThread, &result);
        pthread_mutex_destroy(&m_mutexLock);
        m_camera.release();
    }
}

std::string Camera::read()
{
    std::string ptrString;
    if(m_running)
    {
        m_camera.retrieve(m_rawImage);
        ptr2String((void*)&m_rawImage, ptrString);
        return ptrString;
    }
    
    return "";
}