#include "numberdetect.h"

using namespace upm;
using namespace std;
using namespace cv;
using namespace cv::ml;

CNumberDetect::CNumberDetect():mFeatureLen(64)
{
}

CNumberDetect::~CNumberDetect()
{

}

void CNumberDetect::swapBuffer(char* buf)   
{   
    char temp;   
    temp = *(buf);   
    *buf = *(buf+3);   
    *(buf+3) = temp;   
   
    temp = *(buf+1);   
    *(buf+1) = *(buf+2);   
    *(buf+2) = temp;   
}   

int CNumberDetect::readTrainData(int maxCount)   
{   
    //Open image and label file    
    const char fileName[] = "/home/root/train-images.idx3-ubyte";   
    const char labelFileName[] = "/home/root/train-labels.idx1-ubyte";   
   
    ifstream lab_ifs(labelFileName, ios_base::binary);   
    ifstream ifs(fileName, ios_base::binary);   
   
    if( ifs.fail() == true )   
        return -1;   
   
    if( lab_ifs.fail() == true )   
        return -1;   
   
    //Read train data number and image rows / cols    
    char magicNum[4], ccount[4], crows[4], ccols[4];   
    ifs.read(magicNum, sizeof(magicNum));   
    ifs.read(ccount, sizeof(ccount));   
    ifs.read(crows, sizeof(crows));   
    ifs.read(ccols, sizeof(ccols));   
   
    int count, rows, cols;   
    swapBuffer(ccount);   
    swapBuffer(crows);   
    swapBuffer(ccols);   
   
    memcpy(&count, ccount, sizeof(count));   
    memcpy(&rows, crows, sizeof(rows));   
    memcpy(&cols, ccols, sizeof(cols));   
   
    //Just skip label header    
    lab_ifs.read(magicNum, sizeof(magicNum));   
    lab_ifs.read(ccount, sizeof(ccount));   
   
    //Create source and show image matrix    
    Mat src = Mat::zeros(rows, cols, CV_8UC1);   
    Mat temp = Mat::zeros(8, 8, CV_8UC1);   
    Mat dst;   
   
    char label = 0;   
   
    NumTrainData rtd;   
   
    int total = 0;   
   
    while(!ifs.eof())   
    {   
        if(total >= count)   
            break;   
           
        total++;   
           
        //Read label    
        lab_ifs.read(&label, 1);   
        label = label + '0';   
   
        //Read source data    
        ifs.read((char*)src.data, rows * cols);   
        getROI(src, dst);   
   
        rtd.result = label;   
        resize(dst, temp, temp.size());   
        //threshold(temp, temp, 10, 1, CV_THRESH_BINARY);    
   
        for(int i = 0; i<8; i++)   
        {   
            for(int j = 0; j<8; j++)   
            {   
                    rtd.data[ i*8 + j] = temp.at<uchar>(i, j);   
            }   
        }   
   
        mBuffer.push_back(rtd);   
   
        maxCount--;   
           
        if(maxCount == 0)   
            break;   
    }   
   
    ifs.close();   
    lab_ifs.close();   
   
    return 0;   
}   

void CNumberDetect::getROI(Mat& src, Mat& dst){
    int left, right, top, bottom;
    left = src.cols;
    right = 0;
    top = src.rows;
    bottom = 0;

    //得到区域
    for(int i=0; i<src.rows; i++)
    {
        for(int j=0; j<src.cols; j++)
        {
            if(src.at<uchar>(i, j) > 0)
            {
                if(j<left) left = j;
                if(j>right) right = j;
                if(i<top) top = i;
                if(i>bottom) bottom = i;
            }
        }
    }
    int width = right - left;   
    int height = bottom - top;   
    int len = (width < height) ? height : width;   

    //Create a squre    
    dst = Mat::zeros(len, len, CV_8UC1);   

    //Copy valid data to squre center    
    Rect dstRect((len - width)/2, (len - height)/2, width, height);   
    Rect srcRect(left, top, width, height);   
    Mat dstROI = dst(dstRect);   
    Mat srcROI = src(srcRect);   
    srcROI.copyTo(dstROI);   
}

void CNumberDetect::ptr2String(void* in_ptr, std::string &in_str)
{
    unsigned long ptr2Number;
    std::stringstream number2Stream;

    ptr2Number = (unsigned long)in_ptr;
    number2Stream << ptr2Number;
    in_str = "Camera:" + number2Stream.str();
}


bool CNumberDetect::string2Ptr(std::string &in_str, void** in_ptr)
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

unsigned char CNumberDetect::numberDetect(std::string in_ptr)
{
	unsigned char result;
	if(string2Ptr(in_ptr, (void**)&m_rawImage))
	{
		result = detect(m_rawImage);
        ptr2String((void *)(&m_outputImage), m_outputString);
	}
	else
		result = -1;

	return result;
}

bool CNumberDetect::initNumberDetect()
{
    int maxCount = 60000;   
    std::string numberDetectName = "/home/root/SNDaily/node-red/node_modules/node-red-contrib-smartnode/imageProcessing/numberDetect/SVM_DATA.xml";
    mSvm = StatModel::load<SVM>(numberDetectName);
    if(!mSvm)
    {
        cout << "Can not find SVM_DATA.xml." << endl;
        return false;
    }

#if(ON_STUDY)
    readTrainData(maxCount);   
    newSvmStudy(mbuffer);   
#else
 //   newSvmPredict();
#endif
    return true;
}

void CNumberDetect::newSvmStudy(vector<NumTrainData>& trainData)   
{   
    int testCount = trainData.size();   
   
    Mat m = Mat::zeros(1, mFeatureLen, CV_32FC1);   
    Mat data = Mat::zeros(testCount, mFeatureLen, CV_32FC1);   
    Mat res = Mat::zeros(testCount, 1, CV_32SC1);   
   
    for (int i= 0; i< testCount; i++)    
    {    
        NumTrainData td = trainData.at(i);   
        memcpy(m.data, td.data, mFeatureLen*sizeof(float));   
        normalize(m, m);   
        memcpy(data.data + i*mFeatureLen*sizeof(float), m.data, mFeatureLen*sizeof(float));   
   
        res.at<unsigned int>(i, 0) = td.result;   
    }   
   
    /////////////START SVM TRAINNING//////////////////    
    cout << "traing..." << endl;
    SVM::Params params;
    params.svmType = SVM::C_SVC;
    params.kernelType = SVM::RBF;
    params.termCrit   = TermCriteria(TermCriteria::EPS, 1000, 1e-6);
    Ptr<SVM> svm = StatModel::train<SVM>(data, ml::ROW_SAMPLE, res, params);
    svm->save( "/home/root/SVM_DATA.xml" );  
    cout << "training finish..." << endl;
}   

int CNumberDetect::newSvmPredict()   
{   
    Ptr<SVM> svm = StatModel::load<SVM>( "/home/root/SVM_DATA.xml" );
   
    const char fileName[] = "/home/root/t10k-images.idx3-ubyte";   
    const char labelFileName[] = "/home/root/t10k-labels.idx1-ubyte";   
   
    ifstream lab_ifs(labelFileName, ios_base::binary);   
    ifstream ifs(fileName, ios_base::binary);   
   
    if( ifs.fail() == true )   
        return -1;   
   
    if( lab_ifs.fail() == true )   
        return -1;   
   
    char magicNum[4], ccount[4], crows[4], ccols[4];   
    ifs.read(magicNum, sizeof(magicNum));   
    ifs.read(ccount, sizeof(ccount));   
    ifs.read(crows, sizeof(crows));   
    ifs.read(ccols, sizeof(ccols));   
   
    int count, rows, cols;   
    swapBuffer(ccount);   
    swapBuffer(crows);   
    swapBuffer(ccols);   
   
    memcpy(&count, ccount, sizeof(count));   
    memcpy(&rows, crows, sizeof(rows));   
    memcpy(&cols, ccols, sizeof(cols));   
   
    Mat src = Mat::zeros(rows, cols, CV_8UC1);   
    Mat temp = Mat::zeros(8, 8, CV_8UC1);   
    Mat m = Mat::zeros(1, mFeatureLen, CV_32FC1);   
    Mat dst;   
   
    //Just skip label header    
    lab_ifs.read(magicNum, sizeof(magicNum));   
    lab_ifs.read(ccount, sizeof(ccount));   
   
    char label = 0;   
   
    NumTrainData rtd;   
   
    int right = 0, error = 0, total = 0;   
    int right_1 = 0, error_1 = 0, right_2 = 0, error_2 = 0;   
    while(ifs.good())   
    {   
        //Read label    
        lab_ifs.read(&label, 1);   
        label = label + '0';   
   
        //Read data    
        ifs.read((char*)src.data, rows * cols);   
        getROI(src, dst);   
   
        rtd.result = label;   
        resize(dst, temp, temp.size());   
        //threshold(temp, temp, 10, 1, CV_THRESH_BINARY);    
        for(int i = 0; i<8; i++)   
        {   
            for(int j = 0; j<8; j++)   
            {   
                    m.at<float>(0,j + i*8) = temp.at<uchar>(i, j);   
            }   
        }   
   
        if(total >= count)   
            break;   
   
        normalize(m, m);   
        char ret = (char)svm->predict(m);    
   
        if(ret == label)   
        {   
            right++;   
            if(total <= 5000)   
                right_1++;   
            else   
                right_2++;   
        }   
        else   
        {   
            error++;   
            if(total <= 5000)   
                error_1++;   
            else   
                error_2++;   
        }   
   
        total++;   
   
#if(SHOW_PROCESS)    
        cout << "Number " << label << ", predict " << ret << endl;   
#endif    
    }   
   
    ifs.close();   
    lab_ifs.close();   
   
    cout << "Total " << total << ", right " << right <<", error " << error << endl;   
   
    return 0;   
}   


int CNumberDetect::detect(cv::Mat *m_rawImage)
{
 //   std::string sDir = "/home/root/SNDaily/test1.png";
    Mat grayImg, sobelImg, validImg;
    int scaler = 1;
    //detect the resolution of image
    //cout << "resolution:" <<m_rawImage->cols << endl;
    switch(m_rawImage->cols){
        case 160:
            scaler = 1;
            break;
        case 320:
            scaler = 2;
            break;
        case 640:
            scaler = 4;
            break;
        default:
            return -1;
    }
    Mat temp = Mat::zeros(8, 8, CV_8UC1);   
    Mat m = Mat::zeros(1, mFeatureLen, CV_32FC1);   
 //   imwrite("/home/root/SNDaily/test1.png", *m_rawImage);
//	grayImg = imread(sDir, IMREAD_GRAYSCALE);
//    if(grayImg.empty())
//    {
//        cout << "Can not load image." << endl;
//        return -1;
//    }
    // covert the color picture to gray picture
    cvtColor(*m_rawImage, grayImg, COLOR_RGB2GRAY);
    
    //detect vetical edge 
    grayImg.copyTo(sobelImg);
  //  Canny(sobelImg, sobelImg, 1, 3, 3);
    Sobel(grayImg, sobelImg, CV_16S, 1, 0, 5);
    convertScaleAbs(sobelImg, sobelImg);

    //process into two value picture
	threshold(sobelImg, sobelImg, 130, 255, THRESH_BINARY);
    Mat element = getStructuringElement(MORPH_RECT, Size(3,1), Point(1,0));
    erode(sobelImg, sobelImg, element, Point(1, 0), 2);
    dilate(sobelImg, sobelImg, element, Point(1, 0), 4);
    erode(sobelImg, sobelImg, element, Point(1, 0), 2);
    element = getStructuringElement(MORPH_RECT, Size(1,3), Point(0,1));
    erode(sobelImg, sobelImg, element, Point(0, 1), 1);
    dilate(sobelImg, sobelImg, element, Point(0, 1), 2);
    erode(sobelImg, sobelImg, element, Point(0, 1), 1);
    
    //filtrate the valid area of number
    vector<vector<Point> > contours0;
    size_t k;
    bool bFindNumber = false;
    Mat grayROI;
    int minWidth = scaler*5;
    int maxWidth = scaler*50;
    int minHeight = scaler*30;
    int maxHeight = scaler*100;
    findContours( sobelImg, contours0, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
    for( k = 0; k < contours0.size(); k++ ) //   return 0;
    {
        Rect rect = boundingRect(contours0[k]);
        if((rect.width >= minWidth && rect.width <= maxWidth) && (rect.height >= minHeight && rect.height <= maxHeight))
        {
            bFindNumber = true;
            grayROI = grayImg(rect);
            break;
        }
    }

    // predict the number according to the training database
    if(bFindNumber)
    {
        //imwrite("/home/root/before.png", grayROI);
        threshold(grayROI, grayROI, 100, 255, THRESH_BINARY_INV);
        //imwrite("/home/root/after.png", grayROI);
        getROI(grayROI, validImg);
        resize(validImg, temp, temp.size());
        for(int i = 0; i<8; i++)   
        {   
            for(int j = 0; j<8; j++)   
            {   
                m.at<float>(0,j + i*8) = temp.at<uchar>(i, j);   
            }   
        }   
        normalize(m, m);   
        char ret = (char)mSvm->predict(m);    
 //       cout << "the predict number is :" << ret << endl;
        //Too small to watch    
        Mat img = Mat::zeros(grayROI.rows*3, grayROI.cols*3, CV_8UC3);   
        resize(grayROI, img, img.size()); 
        img.copyTo(m_outputImage);
        return (ret - '0');
    }
    else
    {
        cout << "Can not find number." << endl;
        return -1;
    }
}
