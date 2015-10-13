#include "camera.h"

using namespace upm;

Camera::Camera(unsigned char in_cameraId, double in_width, double in_height)
{
	m_cameraId = in_cameraId;
	m_width = in_width;
	m_height = in_height;
}

Camera::~Camera()
{
	stopCamera();
}

int* Camera::test()
{
	static int a;
	return &a;
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
	m_camera.open(m_cameraId);
	if(m_camera.isOpened())
	{
		m_camera.set(cv::CAP_PROP_FRAME_WIDTH, m_width);
		m_camera.set(cv::CAP_PROP_FRAME_HEIGHT, m_height);
		return true;
	}
	else
		return false;
}

void Camera::stopCamera()
{
	m_camera.release();
}

std::string Camera::read()
{
	std::string ptrString;
	m_camera >> m_rawImage;
	ptr2String((void*)&m_rawImage, ptrString);
	return ptrString;
}