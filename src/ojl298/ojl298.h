#pragma once

#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include "mraa.hpp"

namespace upm
{
    class OJL298
    {
    public:
        unsigned char speed;
        //Counter-Clockwise:0, Clockwise:1, Stop:2
        unsigned char direction;
        double timeout;

    private:
        mraa::Pwm* pwm;
        mraa::Gpio* dir;
        pthread_t m_thread;
        bool running, reset_flag;
        mraa::Result result;
        void mraa_error(mraa::Result error_code);

    public:
        OJL298(int pwm, int dir1);
        ~OJL298();

        void setMotion();
        void stop();
        static void* timeThread(void *cs);
    };
}