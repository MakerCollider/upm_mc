#pragma once

#include <iostream>
#include <string>
#include <b64/cencode.h>
#include <opencv2/opencv.hpp>

namespace upm
{
    class Cimg2Base64
    {
    public:
        cv::Mat* m_rawImage;
        cv::Mat m_outputImage;
        std::string m_outputString;
    public:
        Cimg2Base64();
        ~Cimg2Base64();
	
	void ptr2String(void* in_ptr, std::string &in_str);
	bool string2Ptr(std::string &in_str, void** in_ptr);
    unsigned char  base64(cv::Mat* in_rawImage);
    unsigned char noderedBase64(std::string in_ptr);
    };
}
