#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv2/ml.hpp>
#define ON_STUDY    0
#define SHOW_PROCESS 0

namespace upm
{
    class NumTrainData   
    {   
        public:   
            NumTrainData()   
            {   
                memset(data, 0, sizeof(data));   
                result = -1;   
            }   
        public:   
            float data[64];   
            int result;   
    };   

    class CNumberDetect
    {
    public:
        cv::Ptr<cv::ml::SVM> mSvm;
        cv::Mat* m_rawImage;
        cv::Mat m_outputImage;
        std::string m_outputString;
        std::vector<NumTrainData> mBuffer;
        int mFeatureLen;
    public:
        CNumberDetect();
        ~CNumberDetect();

	    bool  initNumberDetect();	
        unsigned char numberDetect(std::string in_ptr);
        
    private:
        int readTrainData(int maxCount);
        void newSvmStudy(std::vector<NumTrainData>& trainData);
        int newSvmPredict(); 
        void swapBuffer(char* buf);
        void getROI(cv::Mat& src, cv::Mat& dst);
        bool string2Ptr(std::string &in_str, void** in_ptr);
        void ptr2String(void* in_ptr, std::string &in_str);
        int detect(cv::Mat *m_rawImage);
    };
}
