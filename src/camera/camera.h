#pragma once

#include <iostream>
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
	public:
		Camera(unsigned char in_cameraId, double in_width=0, double in_height=0);
		~Camera();

		void ptr2String(void* in_ptr, std::string &in_str);
		bool string2Ptr(std::string &in_str, void** in_ptr);

		int* test();
		std::string read();
		bool startCamera();
		void stopCamera();
	};
}
