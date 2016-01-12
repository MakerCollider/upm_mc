/*
   RGBLED.cpp - Library for LEDs to display RGB/HSV color
   March 19th, 2011
   Version 0.0
   */
#include <iostream>
#include <string>
#include <stdexcept>
#include "rgbled.h"
using namespace upm;
using namespace std;


RGBLED::RGBLED(uint8_t  redPin, uint8_t  greenPin, uint8_t  bluePin)  //Initialization for the three pins of to show RGB/HSV color
{
    mContextPwmRed = mraa_pwm_init(redPin);
    mContextPwmGreen = mraa_pwm_init(greenPin);
    mContextPwmBlue = mraa_pwm_init(bluePin);
    if(!mContextPwmRed || !mContextPwmGreen || !mContextPwmBlue)
    {
        throw std::invalid_argument(std::string(__FUNCTION__) + 
              //  " red: " + static_cast<const void *>(mContextPwmRed) + 
              //  " green: " + static_cast<const void *>(mContextPwmGreen) + 
              //  " blue: " + static_cast<const void *>(mContextPwmBlue) + 
                ": mraa_pwm_init() failed, invalid pin?");
        return;
    }
    mName = "RGBLED";
    mraa_pwm_enable(mContextPwmRed, 1);
    mraa_pwm_period_ms(mContextPwmRed, 1);

    mraa_pwm_enable(mContextPwmGreen, 1);
    mraa_pwm_period_ms(mContextPwmGreen, 1);

    mraa_pwm_enable(mContextPwmBlue, 1);
    mraa_pwm_period_ms(mContextPwmBlue, 1);
    RGBLED::InitPins();
}

RGBLED::~RGBLED()  
{
    mraa_pwm_enable(mContextPwmRed, 0);
    mraa_pwm_enable(mContextPwmGreen, 0);
    mraa_pwm_enable(mContextPwmBlue, 0);
    mraa_pwm_close(mContextPwmRed);
    mraa_pwm_close(mContextPwmGreen);
    mraa_pwm_close(mContextPwmBlue);
}


void RGBLED::InitPins(){
    mraa_pwm_write(mContextPwmRed, 0.99);
    mraa_pwm_write(mContextPwmGreen, 0.99);
    mraa_pwm_write(mContextPwmBlue, 0.99);
}

//Set RGB color to the LED
void RGBLED::colorRGB(int red, int green, int blue){
    mraa_pwm_write(mContextPwmRed, constrain(red,0,255)/(float)255);
    mraa_pwm_write(mContextPwmGreen, constrain(green,0,255)/(float)255);
    mraa_pwm_write(mContextPwmBlue, constrain(blue,0,255)/(float)255);
}


//Set HSV color to the LED
//range of H:0~360
//range of S:0.0~1.0
//range of V:0.0(BLACK)~1.0(WHITE)
void RGBLED::colorHSV(int hue, int Value, int Saturation){
    int __max_brightness = 255;

    uint8_t sector = hue / 60;
    uint8_t rel_pos = hue - (sector * 60);
    uint16_t const mmd = 255 * 255; 	/* maximum modulation depth */
    uint16_t top;
    top = Value * 255;
    uint16_t bottom = Value * (255 - Saturation); 	/* (Value*255) - (Value*255)*(Saturation/255) */
    uint16_t slope = (uint16_t)(Value) * (uint16_t)(Saturation) / 120; 	/* dy/dx = (top-bottom)/(2*60) -- Value*Saturation: modulation_depth dy */
    uint16_t a = bottom + slope * rel_pos;
    uint16_t b = bottom + (uint16_t)(Value) * (uint16_t)(Saturation) / 2 + slope * rel_pos;
    uint16_t c = top - slope * rel_pos;
    uint16_t d = top - (uint16_t)(Value) * (uint16_t)(Saturation) / 2 - slope * rel_pos;

    uint16_t R, G, B;

    if (sector == 0)    {
        R = c;
        G = a;
        B = bottom;
    }
    else if (sector == 1)	{
        R = d;
        G = b;
        B = bottom;
    }
    else if (sector == 2)	{
        R = bottom;
        G = c;
        B = a;
    }
    else if (sector == 3)	{
        R = bottom;
        G = d;
        B = b;
    }
    else if (sector == 4)	{
        R = a;
        G = bottom;
        B = c;
    }
    else	{
        R = b;
        G = bottom;
        B = d;
    }

    uint16_t scale_factor = mmd / __max_brightness;

    R = (uint8_t) (R / scale_factor);
    G = (uint8_t) (G / scale_factor);
    B = (uint8_t) (B / scale_factor);

    this->colorRGB(R,G,B);
}
