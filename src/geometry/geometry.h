#pragma once

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

namespace upm
{
    typedef enum{
        GEOMETRY_ERROR = -1,
        GEOMETRY_NOTHING = 0,
        GEOMETRY_RECTANGLE = 1,
        GEOMETRY_CIRCLE = 2,
    }geometryResultType;

    class Geometry
    {
    public:
        cv::Mat* m_rawImage;
        cv::Mat m_outputImage;
        std::string m_outputString;
    public:
        Geometry();
        ~Geometry();
	
	void ptr2String(void* in_ptr, std::string &in_str);
	bool string2Ptr(std::string &in_str, void** in_ptr);
        geometryResultType detect(cv::Mat* in_rawImage);
        unsigned char noderedDetect(std::string in_ptr);
    };
}
