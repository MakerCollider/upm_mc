#pragma once
#include <string>
#include <mraa/pwm.h>

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

namespace upm{
    class RGBLED 	{
        public:
            RGBLED(uint8_t  redPin, uint8_t  greenPin, uint8_t  bluePin);
            ~RGBLED();
            void InitPins();
            //void colorRGB(int red, int green, int blue);
            void colorRGB(int, int, int);
            void colorHSV(int hue, int Value, int Saturation);

        std::string name()
        {
            return mName;
        }

        protected:
            std::string mName;

        private:
            mraa_pwm_context  mContextPwmRed;
            mraa_pwm_context  mContextPwmGreen;
            mraa_pwm_context  mContextPwmBlue;
    };

}


