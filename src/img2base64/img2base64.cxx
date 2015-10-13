#include "img2base64.h"

using namespace upm;
using namespace std;
using namespace cv;

Cimg2Base64::Cimg2Base64()
{

}

Cimg2Base64::~Cimg2Base64()
{

}

void Cimg2Base64::ptr2String(void* in_ptr, std::string &in_str)
{
    unsigned long ptr2Number;
    std::stringstream number2Stream;

    ptr2Number = (unsigned long)in_ptr;
    number2Stream << ptr2Number;
    in_str = "Camera:" + number2Stream.str();
}

bool Cimg2Base64::string2Ptr(std::string &in_str, void** in_ptr)
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

unsigned char Cimg2Base64::noderedBase64(std::string in_ptr)
{
	unsigned char result;
	if(string2Ptr(in_ptr, (void**)&m_rawImage))
	{
		result = base64(m_rawImage);
	}
	else
		result = -1;

	return result;
}

unsigned char Cimg2Base64::base64(cv::Mat* in_rawImage)
{
	vector<uchar> buffer;
	vector<int> param;

	std::string str = ".";
	str += "png";
	/*
	if(str == ".jpg") {
		param.push_back(IMWRITE_JPEG_QUALITY);
	param.push_back(70);//default(95) 0-100
	imencode(str.c_str(), *src, buffer, param);
	} else {
	*/
	imencode(str.c_str(), *in_rawImage, buffer);

	int bufferLen = buffer.size();

	unsigned char *bufferStr = (uchar*) malloc(bufferLen * 2);

	copy(buffer.begin(), buffer.end(), bufferStr);
	bufferStr[bufferLen] = '\0';

	base64_encodestate es;
	base64_init_encodestate(&es);

	char *headFmt = "data:image/%s;base64,";
	char *str64 = (char*) malloc(bufferLen * 2 + strlen(headFmt) + 3 + 1);
	char *p = str64;

	p += sprintf(p, headFmt, "png");
	p += base64_encode_block((char*)bufferStr, bufferLen, p, &es);
	p += base64_encode_blockend(p, &es);
	*p = '\0';
	m_outputString = str64;
	return 1;
}
