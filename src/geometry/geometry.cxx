#include "geometry.h"

using namespace upm;
using namespace std;
using namespace cv;

Geometry::Geometry()
{

}

Geometry::~Geometry()
{

}

void Geometry::ptr2String(void* in_ptr, std::string &in_str)
{
    unsigned long ptr2Number;
    std::stringstream number2Stream;

    ptr2Number = (unsigned long)in_ptr;
    number2Stream << ptr2Number;
    in_str = "Camera:" + number2Stream.str();
}

bool Geometry::string2Ptr(std::string &in_str, void** in_ptr)
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

unsigned char Geometry::noderedDetect(std::string in_ptr)
{
    geometryResultType result;
    if(string2Ptr(in_ptr, (void**)&m_rawImage))
    {
        result = detect(m_rawImage);
        ptr2String((void*)&m_outputImage, m_outputString);
    }
    else
        result = GEOMETRY_ERROR;

    return result;
}

geometryResultType Geometry::detect(cv::Mat* in_rawImage)
{
    Mat img_gray, img_thres, img_canny, img_contours;

    vector<Point> poly;
    vector<Vec3f> circles;
    vector<vector<Point> > contours;

    unsigned int i;
    double area;

    geometryResultType result = GEOMETRY_NOTHING;
    cv::Mat outputImage;
    outputImage = cv::Mat::zeros(in_rawImage->rows, in_rawImage->cols, CV_8UC1);

    cvtColor((*in_rawImage), img_gray, COLOR_RGB2GRAY);
    adaptiveThreshold(img_gray, img_thres, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY,201,20);
    Canny(img_thres, img_canny, 150, 200);
    findContours(img_canny, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);

    for (i = 0; i < contours.size(); i++)
    {
        area = fabs(contourArea(contours[i]));
        if (area > 1500)
        {
            drawContours(outputImage, contours, i, Scalar(255), FILLED);
            approxPolyDP(contours[i], poly, 5, 1);
            if (poly.size() == 4)
            {
                result = GEOMETRY_RECTANGLE;
                break;
            }
            else
            {
                HoughCircles(outputImage, circles, HOUGH_GRADIENT, 2, outputImage.rows / 2, 30, 15);
                if (circles.size() != 0)
                {
                    result = GEOMETRY_CIRCLE;
                    break;
                }
            }
        }
    }

    cv::cvtColor(outputImage, m_outputImage, cv::COLOR_GRAY2BGR);
    return result;
}
