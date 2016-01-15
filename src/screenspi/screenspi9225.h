/*
 * ILI9225G.h
 *
 *  Created on: Jun 24, 2013
 *      Author: blitzter
 */

#pragma once

#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <map>
#include "opencv2/core/core.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "mraa.hpp"


#define WHITE          0xFFFF
#define BLACK          0x0000
#define BLUE           0x001F
#define BLUE2          0x051F
#define RED            0xF800
#define MAGENTA        0xF81F
#define GREEN          0x07E0
#define CYAN           0x7FFF
#define YELLOW         0xFFE0
#define ORANGE		   0xF900

namespace upm
{
    class ScreenSpi9225
    {
    public:
        enum ScreenError
        {
            SUCCESS,
            IMAGE_PATH_ERROR,
            IMAGE_EXIST
        };

        struct ImageData
        {
            uint16_t topLeftX;
            uint16_t topLeftY;
            uint16_t height;
            uint16_t width;
            uint16_t* data;
            uint32_t length;
        };
    public:
        void ILI9225GclearScreen(unsigned short color);
        void ILI9225GfillRect(int in_index);
        void ILI9225GfillRectA(std::string in_string);
        void ILI9225GflashBuffer(ImageData in_imageData);
        void ILI9225GflashBuffer(int16_t x, int16_t y, 
                                 int16_t w, int16_t h,
                                 uint16_t* rectbuffer,
                                 int length);
        ScreenError setImage(std::string in_string, int in_index);

    private:
        mraa::Gpio* gCS;
        mraa::Gpio* gRS;
        mraa::Gpio* gRST;
        mraa::Spi* gSPI;
        uint16_t *gFB;
        std::map<int, ImageData> imageMap;
        unsigned long gFBSize;
        void mraa_error(mraa_result_t error_code);
        void writeCommand(unsigned short wr_cmd_a, unsigned short wr_cmd_b);
        void writeData(unsigned short wr_cmd_a);
        void writeDataFast(unsigned short wr_cmd_a);
        void writeRegister(unsigned short wr_cmd_a);
        void initializeLCD();
        void delay(unsigned long t);
        bool string2Ptr(std::string &in_str, void** in_ptr);
        void image2flow(cv::Mat& in_image, uint16_t* in_str);
        ImageData mat2ImageData(cv::InputArray in_mat);
        cv::Mat smileImg;
        cv::Mat angryImg;
        cv::Mat normalImg;
    public:
        ScreenSpi9225(int cs, int rs, int rst);
        ~ScreenSpi9225();
    };
}


