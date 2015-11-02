#include <iostream>
#include "ojl298.h"

using namespace upm;
using namespace std;
using namespace mraa;

OJL298::OJL298(int pwmA, int dir1)
{
    pwm = new mraa::Pwm(pwmA);
    if(pwm == NULL)
        mraa_error(mraa::ERROR_UNSPECIFIED);
    pwm->enable(1);

    dir = new mraa::Gpio(dir1);
    if(dir == NULL)
        mraa_error(mraa::ERROR_UNSPECIFIED);
    dir->dir(mraa::DIR_OUT);

    timeout = 3;
    running = true;
    reset_flag = false;
    pthread_create(&m_thread, NULL, timeThread, (void *)this);
}

OJL298::~OJL298()
{
    stop();
    running = false;
    delete pwm;
    delete dir;
}

void OJL298::mraa_error(mraa::Result error_code)
{
    mraa::printError(error_code);
    exit(1);
}

void OJL298::setMotion()
{
    static float percent;

    if(direction == 2)
    {
        stop();
    }
    else
    {
        if (speed < 0)
            speed = 0;

        if (speed > 100)
            speed = 100;

        percent = float(speed) / 100.0;

        result = pwm->write(percent);
        if(result != mraa::SUCCESS)
            mraa_error(result);

        result = dir->write(direction);
        if(result != mraa::SUCCESS)
            mraa_error(result);

        reset_flag = true;
    }
}

void OJL298::stop()
{
    pwm->write(0);
    usleep(100);
}

void* OJL298::timeThread(void *cs)
{
    OJL298* motor = (OJL298*)cs;
//计算每个周期的时长
    struct timeval watch_dog;
    double timeStart, timeRecord;
    bool done = true;

    while(motor->running)
    {
        if(motor->reset_flag)
        {
            gettimeofday(&watch_dog, NULL);
            timeStart = watch_dog.tv_sec + (double)(watch_dog.tv_usec / 1000) / 1000;
            motor->reset_flag = false;
            done = false;
        }
        else if(!done)
        {
            gettimeofday(&watch_dog, NULL);
            timeRecord = watch_dog.tv_sec + (double)(watch_dog.tv_usec / 1000)/1000-timeStart;
            if(timeRecord > motor->timeout)
            {
                motor->stop();
                done = true;
            }
        }
    }
}