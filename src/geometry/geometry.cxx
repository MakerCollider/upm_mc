#include <math.h>
#include <unistd.h>
#include <iostream>
#include "opencv2/opencv.hpp"
#include "geometry.h"

using namespace upm;
using namespace std;
using namespace cv;

Geometry::Geometry()
{
	detect = 0;
	isDetect = false;
	running = false;
}

Geometry::~Geometry()
{
	running = false;
	usleep(500);
}

void Geometry::startDetect()
{
	running = true;
	pthread_create(&m_thread, NULL, timeThread, (void *)this);
}

void Geometry::stopDetect()
{
	running = false;
	usleep(500);
}

void* Geometry::timeThread(void *cs)
{
    Geometry* geometry = (Geometry*)cs;

	Mat img_cam, img_out;
	Mat img_gray, img_thres, img_canny, img_contours;

	vector<Point> poly;
	vector<Vec3f> circles;
	vector<vector<Point> > contours;

	//³õÊ¼»¯ÉãÏñÍ·£¬²¢¼õÐ¡·Ö±æÂÊ
	VideoCapture capture(0);
	capture.set(CAP_PROP_FRAME_WIDTH, 320);
	capture.set(CAP_PROP_FRAME_HEIGHT, 240);

	img_out = Mat::zeros(Size(320,240), CV_8UC1);

	unsigned int i;
	double area;

	geometry->running = 1;

	while (geometry->running)
	{
		//³õÊ¼»¯img_outºÍ¶ÁÈ¡ÉãÏñÍ·
		geometry->detect = 0;
		img_out.setTo(0);
		capture.read(img_cam);

		//Ç°ÆÚ´¦Àí£¨»Ò¶È¡¢×ÔÊÊÓ¦¶þÖµ»¯¡¢±ßÔµ¼ì²â£©
		cvtColor(img_cam, img_gray, COLOR_RGB2GRAY);
		adaptiveThreshold(img_gray, img_thres, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY,201,20);
		Canny(img_thres, img_canny, 150, 200);
		
		//¼ì²âÂÖÀª
		findContours(img_canny, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
		for (i = 0; i < contours.size(); i++)
		{
			//¸ù¾ÝÂÖÀªÃæ»ý³õ²½É¸Ñ¡
			area = fabs(contourArea(contours[i]));
			if (area > 1500)
			{
				//»æÖÆÂÖÀª²¢Ìî³ä
				drawContours(img_out, contours, i, Scalar(255, 255, 255), FILLED);
				//ÏÈÅÐ¶ÏÊÇ·ñÎª¾ØÐÎ£¨¶à±ßÐÎ±Æ½ü£©
				approxPolyDP(contours[i], poly, 5, 1);
				if (poly.size() == 4)
				{
					geometry->detect = 1;
					geometry->isDetect = true;
					capture.release();
					sleep(3);
					capture.open(0);
					capture.set(CAP_PROP_FRAME_WIDTH, 320);
					capture.set(CAP_PROP_FRAME_HEIGHT, 240);
					break;
				}
				else
				{
					//ÔÙ¼ì²âÊÇ·ñÓÐÔ²ÐÎ
					HoughCircles(img_out, circles, HOUGH_GRADIENT, 2, img_out.rows / 2, 30, 15);
					if (circles.size() != 0)
					{
						geometry->detect = 2;
						geometry->isDetect = true;
						capture.release();
						sleep(3);
						capture.open(0);
						capture.set(CAP_PROP_FRAME_WIDTH, 320);
						capture.set(CAP_PROP_FRAME_HEIGHT, 240);
						break;
					}
				}
			}
		}
	}
	capture.release();
}
