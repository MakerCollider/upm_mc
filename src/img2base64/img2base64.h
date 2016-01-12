#pragma once

#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>

namespace upm
{
    class Cimg2Base64
    {
    public:
        cv::Mat* m_rawImage;
        cv::Mat m_outputImage;
        std::string m_outputString;
        std::string m_base64Chars;
    public:
        Cimg2Base64();
        ~Cimg2Base64();

        std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
        void ptr2String(void* in_ptr, std::string &in_str);
        bool string2Ptr(std::string &in_str, void** in_ptr);
        bool base64(cv::Mat* in_rawImage);
        bool noderedBase64(std::string in_ptr);
    };
}