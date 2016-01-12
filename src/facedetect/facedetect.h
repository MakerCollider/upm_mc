#pragma once

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

namespace upm
{
    class CfaceDetect
    {
    public:
        cv::Mat* m_rawImage;
        cv::Mat m_outputImage;

        cv::CascadeClassifier face_cascade;
		std::vector<cv::Rect> faces;
		
		std::string m_outputString;
    public:
        CfaceDetect();
        ~CfaceDetect();

		bool initFaceDetect();	
		void ptr2String(void* in_ptr, std::string &in_str);
		bool string2Ptr(std::string &in_str, void** in_ptr);
		unsigned char detect(cv::Mat* in_rawImage);
		unsigned char noderedDetect(std::string in_ptr);
    };
}
