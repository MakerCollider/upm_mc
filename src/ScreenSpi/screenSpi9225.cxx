/*
 * ILI9225G.cpp
 *
 *  Created on: Jun 24, 2013
 *      Author: blitzter
 */


#include "screenSpi9225.h"
using namespace cv;
using namespace mraa;
using namespace upm;
using namespace std;

#define HIGH 1
#define LOW 0

#define FACE_WIDTH 100
#define FACE_HEIGHT 100

#define WIDTH 176
#define HEIGHT 220
#define MAXFRAME 32

/* For Arduino, this is assumed to be 4MHz, using SPI_CLOCK_DEV4 
  * Sticking to this for backward compatibility */
#define SPI_CLK_DEFAULT_HZ 4000000

ScreenSpi9225::ScreenSpi9225(int cs, int rs, int rst){
	gCS = new mraa::Gpio(cs);
	gRS = new mraa::Gpio(rs);
	gRST = new mraa::Gpio(rst);
	gSPI  = new mraa::Spi(0);

	//gpio direction
        mraa_result_t response;
	response = gCS->dir(mraa::DIR_OUT);
        if (response != MRAA_SUCCESS) {
            mraa::printError(response);
        }
	gRS->dir(mraa::DIR_OUT);
	gRST->dir(mraa::DIR_OUT);

	//set frequency
	//mraa_spi_frequency(gSPI, (SPI_CLK_DEFAULT_HZ));
	gSPI->lsbmode(0);
	gSPI->mode(mraa::SPI_MODE0);
	
	//init lcd
	initializeLCD();
        gFBSize = WIDTH*HEIGHT*sizeof(uint16_t);
	gFB = new uint16_t[WIDTH*HEIGHT];
	if(gFB==NULL){
        delete gCS;
        delete gRS;
        delete gRST;
        delete gSPI;
	}
	memset(gFB,0,sizeof(uint16_t)*WIDTH*HEIGHT);
    cv::Mat image;
    image = imread("/usr/share/SmartNode/resource/angry.bmp");
    imagePtr[0] = new uint16_t[image.rows*image.cols];
    image2flow(image, imagePtr[0]);
    image = imread("/usr/share/SmartNode/resource/smile.bmp");                          
    imagePtr[1] = new uint16_t[image.rows*image.cols];
    image2flow(image, imagePtr[1]);
    image = imread("/usr/share/SmartNode/resource/normal.bmp");                          
    imagePtr[2] = new uint16_t[image.rows*image.cols]; 
    image2flow(image, imagePtr[2]);

    cout << "init finished!" << endl;
}
ScreenSpi9225::~ScreenSpi9225(){
    delete gCS;
    delete gRS;
    delete gRST;
    delete gSPI;
    delete []gFB;
}

void ScreenSpi9225::image2flow(cv::Mat& in_image, uint16_t* in_str)
{
	cv::Mat bgr565Image;
	cv::cvtColor(in_image, bgr565Image,cv::COLOR_BGR2BGR565);

	uint16_t length;
	//length = bgr565Image.rows * bgr565Image.cols;
	//in_str = new uint16_t[length];
	uint8_t* p_str = (uint8_t*)in_str;
	for(uint16_t i=0; i<bgr565Image.rows; i++)
		for(uint16_t j=0; j<bgr565Image.cols; j++)
		{
			*p_str = bgr565Image.at<cv::Vec2b>(i, j)[1];
			p_str++;
			*p_str = bgr565Image.at<cv::Vec2b>(i, j)[0];
			p_str++;
		}
}

void ScreenSpi9225::writeCommand(unsigned short wr_cmd_a, unsigned short wr_cmd_b) {
	writeRegister(wr_cmd_a);
	writeData(wr_cmd_b);
}

void ScreenSpi9225::writeData(unsigned short wr_cmd_a) {
	gRS->write(HIGH);
	gCS->write(LOW);
	//transfer(wr_cmd_a >> 8);
	//transfer(wr_cmd_a & 0xFF);
	writeDataFast(wr_cmd_a);
	gCS->write(HIGH);
}

void ScreenSpi9225::writeDataFast(unsigned short wr_cmd_a) {
	//digitalWrite(cs,LOW);
	//transfer(wr_cmd_a >> 8);
	//transfer(wr_cmd_a & 0xFF);
	//digitalWrite(cs,HIGH);
	uint16_t cmd;
	uint8_t *p = (uint8_t*)(&cmd);
	p[0] = wr_cmd_a >> 8;
	p[1] = wr_cmd_a & 0xff;
	gSPI->write_word(&cmd, 2);

}

void ScreenSpi9225::writeRegister(unsigned short wr_cmd_a) {
	gRS->write(LOW);
	gCS->write(LOW);
	//transfer(wr_cmd_a >> 8);
	//transfer(wr_cmd_a & 0xFF);
	writeDataFast(wr_cmd_a);
	gCS->write(HIGH);
}
void ScreenSpi9225::delay(unsigned long t){
	usleep(t*1000);
}
void ScreenSpi9225::initializeLCD() {
	gCS->write(LOW);
	gRST->write(LOW);

	delay(50);
	gRST->write(HIGH);

	writeCommand(0x28, 0x00CE);
	writeCommand(0x01, 0x011C);

	writeCommand(0x03, 0x1030);  //1018

	writeCommand(0x07, 0x0017);
	delay(50);

	writeCommand(0x11, 0x1000);   //0808
	writeCommand(0x20, 0x0000);
	writeCommand(0x21, 0x0000);
	delay(50);
	writeCommand(0x30, 0x0000);  // set_windows_x_y()
	writeCommand(0x31, 0x00DB);  //DB
	writeCommand(0x32, 0x0000);
	writeCommand(0x33, 0x0000);
	writeCommand(0x34, 0x00DB);  //DB
	writeCommand(0x35, 0x0000);

	writeCommand(0x36, 0x00AF);
	writeCommand(0x37, 0x0000);
	writeCommand(0x38, 0x00DB);  //DB
	writeCommand(0x39, 0x0000);
	delay(50);
	writeCommand(0x02, 0x0000); // Gamma Command
	delay(50);
//	writeCommand(0x0d, 0x2100); // Gamma Command

	writeCommand(0x61, 0x0103);
	writeCommand(0xE8, 0x1000);
	writeCommand(0xB0, 0x0812);
	writeCommand(0x0B, 0x0000);
	writeCommand(0x0D, 0x0000);
	writeCommand(0xB1, 0x0404);
	writeCommand(0x62, 0x0019);
	delay(50);
	writeCommand(0xE8, 0x0100);

	writeCommand(0x50, 0x0007);
	writeCommand(0x51, 0x0708);
	writeCommand(0x52, 0x0D0A);
	writeCommand(0x53, 0x0404);
	writeCommand(0x54, 0x0007);
	writeCommand(0x55, 0x0706);
	writeCommand(0x56, 0x0D0A);
	writeCommand(0x57, 0x0404);
	writeCommand(0x58, 0x0000);
	writeCommand(0x59, 0x0000);

	writeCommand(0x20, 0x0000);
	writeCommand(0x21, 0x0000);
	writeCommand(0x07, 0x1017);
	delay(80);
        cout<<"init LCD parameter finished!" << endl;
}

void ScreenSpi9225::ILI9225GclearScreen(unsigned short color) {
	std::cout << "ClearScreen" << std::endl;
	int i, j;

	int index=0;	
	int len = WIDTH*HEIGHT;
	int size  = len*sizeof(unsigned short);	
	int numblock = size /  MAXFRAME;
	int remain = size % MAXFRAME;
	
	for(i=0;i<len;i++)
	{
		uint8_t * p = (uint8_t*)(&gFB[i]);
		p[0] = color>>8;
		p[1] = color & 0xff;
	
        }
	
	writeCommand(0x36, 0x00AF); 								 //x start point
	writeCommand(0x37, 0x0000); 								//y start point
	writeCommand(0x38, 0x00DB); 								   //x end point
	writeCommand(0x39, 0x0000); 								//y end point

	// Set the initial value of address Counter
	writeCommand(0x20, 0x0000);
	writeCommand(0x21, 0x0000);

	writeRegister(0x22);									//write data to GRAM
	gRS->write(HIGH);
	gCS->write(LOW);

	uint8_t *  p  = (uint8_t*)gFB;
	for(i=0;i<numblock;i++){
		gSPI->transfer_word((uint16_t*)(p+i*MAXFRAME), NULL, MAXFRAME);
	}
	if(remain){
	    gSPI->transfer_word((uint16_t*)((uint8_t*)gFB+numblock*MAXFRAME), NULL, remain);
	}
	gCS->write(HIGH);

}

void ScreenSpi9225::ILI9225GfillRect(int faceId)
{
	int x = (WIDTH-FACE_WIDTH)/2;
	int y = (HEIGHT-FACE_HEIGHT)/2;
    int w = FACE_WIDTH;
    int h = FACE_HEIGHT;
    int len = FACE_WIDTH*FACE_HEIGHT;
	ILI9225GflashBuffer(x,y, w,h,imagePtr[faceId],len*sizeof(uint16_t));
}
void ScreenSpi9225::ILI9225GflashBuffer(int16_t x, int16_t y, 
                                        int16_t w, int16_t h,
                                        uint16_t* rectbuffer,
                                        int length)
{
	uint16_t i, j;
	int size  = w*h*sizeof(uint16_t);
	int numblock = size /  MAXFRAME;
	int remain = size % MAXFRAME;
	if(rectbuffer==NULL){
		return;
	}
	if(size>length){
		return;
	}
	
	writeCommand(0x36, x + w - 1);					//x end point
	writeCommand(0x37, x);							//x start point
	writeCommand(0x38, y + h);						//y end point
	writeCommand(0x39, y);							//y start point

	// Set the initial value of address Counter
	writeCommand(0x20, x);
	writeCommand(0x21, y);

	writeRegister(0x22);									//write data to GRAM
	gRS->write(HIGH);
	gCS->write(LOW);

	uint8_t *  p  = (uint8_t*)rectbuffer;
	for(i=0;i<numblock;i++){
	    gSPI->transfer_word((uint16_t*)(p+i*MAXFRAME), NULL, MAXFRAME);
	}
	
	if(remain){
	    gSPI->transfer_word((uint16_t*)((uint8_t*)rectbuffer+numblock*MAXFRAME), NULL,
                             remain);
	}	
	gCS->write(HIGH);
}

