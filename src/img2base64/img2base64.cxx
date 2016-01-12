#include "img2base64.h"

using namespace upm;
using namespace std;
using namespace cv;

Cimg2Base64::Cimg2Base64()
{
    m_base64Chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";
}

Cimg2Base64::~Cimg2Base64()
{

}

std::string Cimg2Base64::base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len)
{
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (in_len--)
    {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3)
        {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for(i = 0; (i <4) ; i++)
            {
                ret += m_base64Chars[char_array_4[i]];
            }
            i = 0;
        }
    }

    if (i)
    {
        for(j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++)
        {
            ret += m_base64Chars[char_array_4[j]];
        }

        while((i++ < 3))
        {
            ret += '=';
        }
    }

    return ret;
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

bool Cimg2Base64::noderedBase64(std::string in_ptr)
{
    unsigned char result;
    if(string2Ptr(in_ptr, (void**)&m_rawImage))
    {
        result = base64(m_rawImage);
    }
    else
        result = false;

    return result;
}

bool Cimg2Base64::base64(cv::Mat* in_rawImage)
{
    vector<uchar> imageVector;
    cv::imencode(".png", *in_rawImage, imageVector);
    uint32_t imageVecLength = imageVector.size();
    uchar *imageBuffer = new uchar[imageVecLength];
    for(uint32_t i=0; i<imageVecLength; i++)
    {
        imageBuffer[i] = imageVector[i];
    }

    std::string imageString = base64_encode(imageBuffer, imageVecLength);
    std::string headString = "data:image/png;base64,";
    m_outputString = headString + imageString;
    delete imageBuffer;
    return true;
}