#include "facedetect.h"

using namespace upm;
using namespace std;
using namespace cv;

CfaceDetect::CfaceDetect()
{

}

CfaceDetect::~CfaceDetect()
{

}

bool CfaceDetect::initFaceDetect()
{
	std::string face_cascade_name = "/usr/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml";
	if(!face_cascade.load(face_cascade_name))
    {
    	cout << "can not find face_cascade_file!" <<endl;
    	return false;
    }
    else
    	return true;
}

void CfaceDetect::ptr2String(void* in_ptr, std::string &in_str)
{
    unsigned long ptr2Number;
    std::stringstream number2Stream;

    ptr2Number = (unsigned long)in_ptr;
    number2Stream << ptr2Number;
    in_str = "Camera:" + number2Stream.str();
}

bool CfaceDetect::string2Ptr(std::string &in_str, void** in_ptr)
{
    static std::string head;
    std::string strCut;
    std::istringstream strStream;
    unsigned long number;

    if(in_str.length() <= 7)
    	return false;
    head.assign(in_str, 0, 7);
    if(head != "Camera:")
        return false;

    strCut.assign(in_str, 7, in_str.length());
    strStream.str(strCut);
    strStream >> number;
    *in_ptr = (void*)(number);
    return true;
}

unsigned char CfaceDetect::noderedDetect(std::string in_ptr)
{
	unsigned char result;
	if(string2Ptr(in_ptr, (void**)&m_rawImage))
	{
		result = detect(m_rawImage);
		ptr2String((void*)&m_outputImage, m_outputString);
	}
	else
		result = -1;

	return result;
}

unsigned char CfaceDetect::detect(cv::Mat* in_rawImage)
{
	cv::Mat grayImage;

	unsigned char result = -1;
	in_rawImage->copyTo(m_outputImage);

	cv::resize((*in_rawImage),(*m_rawImage),Size(128,96));

	cvtColor((*m_rawImage), grayImage, COLOR_BGR2GRAY);
	equalizeHist(grayImage, grayImage);

	faces.clear();

face_cascade.detectMultiScale(grayImage, faces, 1.1,
									2, 0|CASCADE_SCALE_IMAGE, Size(30, 30));

	if(faces.empty())
	{
		return 0;
	}
	else
	{
		float wScale = double(m_outputImage.cols) / grayImage.cols;
		float hScale = double(m_outputImage.rows) / grayImage.rows;

		for(unsigned int i = 0; i < faces.size(); i++)
		{
			Point2f center((faces[i].x + faces[i].width*0.5)*wScale, (faces[i].y + faces[i].height*0.5)*hScale);
			ellipse(m_outputImage, center, Size( faces[i].width*0.5*wScale, faces[i].height*0.5*hScale),
					0, 0, 360, Scalar( 255, 0, 255 ), 2, cv::LINE_AA, 0);
		}

		return faces.size();
	}
}
