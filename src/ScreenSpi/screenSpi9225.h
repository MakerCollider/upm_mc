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
        void ILI9225GclearScreen(unsigned short color);
        void ILI9225GfillRect(int faceId);
        void ILI9225GflashBuffer(int16_t x, int16_t y, 
                                 int16_t w, int16_t h,
                                 unsigned short * rectbuffer,
                                 int length);

    private:
        mraa::Gpio* gCS;
        mraa::Gpio* gRS;
        mraa::Gpio* gRST;
        mraa::Spi* gSPI;
        uint16_t *gFB;
        unsigned long gFBSize;
        void mraa_error(mraa_result_t error_code);
        void writeCommand(unsigned short wr_cmd_a, unsigned short wr_cmd_b);
        void writeData(unsigned short wr_cmd_a);
        void writeDataFast(unsigned short wr_cmd_a);
        void writeRegister(unsigned short wr_cmd_a);
        void initializeLCD();
        void delay(unsigned long t);
        cv::Mat smileImg;
        cv::Mat angryImg;
        cv::Mat normalImg;
    public:
        ScreenSpi9225(int cs, int rs, int rst);
        ~ScreenSpi9225();
    };
}


