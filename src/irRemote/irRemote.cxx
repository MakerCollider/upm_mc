#include <iostream>
#include "irRemote.h"

using namespace upm;
using namespace std;
using namespace mraa;

IrRemote::IrRemote(int digitalPin)
{
    gpio = new mraa::Gpio(digitalPin);
    if(gpio == NULL)
        mraa_error(mraa::ERROR_UNSPECIFIED);
    
    mraa::Result response = gpio->dir(mraa::DIR_IN);
    if (response != mraa::SUCCESS) {
        mraa_error(response);
    }
    irCode = 0;
    timeDiff = 0;
    pthread_mutex_init(&m_mutexLock, NULL);
    pthread_create(&m_thread, NULL, decodeThread, (void *)this);
    running = 1;
}

IrRemote::~IrRemote()
{
    stopIrRemote();
}

void IrRemote::mraa_error(mraa::Result error_code)
{
    mraa::printError(error_code);
    exit(1);
}

void* IrRemote::decodeThread(void *cs)
{
    IrRemote* irm = (IrRemote*)cs;
    cout<<"decodeThread: Ready to decode..."<<endl;
    while(1)
    {
        if(irm->running == 1)
        {
            irm->decodeIr();
            if(irm->irCode != 0x0000)
            {
                cout<<"decodeThread: irCode = " << hex << irm->irCode <<endl;
            }
        } else {
            cout<<"decodeThread: stop!"<<endl;
            break;
        }
    }
    
}
void IrRemote::decodeIr()
{
    gettimeofday(&m_start,NULL);
    while(gpio->read())//是高就等待
    {
        //cout<<"decodeIr: Get Low"<<endl;
        gettimeofday(&m_end,NULL);
        timeDiff = time_diff(m_start,m_end);
        //cout<<"decodeIr: High timeDiff is "<< timeDiff<<endl;
        if(timeDiff > 100000)  //当高电平持续时间超过100ms，表明此时没有按键按下
        {
            pthread_mutex_lock(&m_mutexLock);
            irCode = 0x0000;
            pthread_mutex_unlock(&m_mutexLock);
            return;
        }  
    }
    //cout<<"decodeIr: dectect button press..."<<endl;

    gettimeofday(&m_start,NULL);
    while(!(gpio->read())); //低等待
    gettimeofday(&m_end,NULL);
    timeDiff = time_diff(m_start,m_end);
    
    //cout<<"decodeIr: 1 timeDiff is "<< timeDiff <<endl;
    if(timeDiff > 8700 && timeDiff < 9300)//9ms
    {
        gettimeofday(&m_start,NULL);
        while(gpio->read());//是高就等待
        gettimeofday(&m_end,NULL);
        timeDiff = time_diff(m_start,m_end);

        //cout<<"decodeIr: 2 timeDiff is "<< timeDiff <<endl;
        if(timeDiff > 4000 && timeDiff < 5000)//4.5ms
        {
            pulse_deal();
            return;
        }
        else if(timeDiff > 1750 && timeDiff < 2750)//2.25ms
        {
            gettimeofday(&m_start,NULL);
            while(!(gpio->read()));//低等待
            gettimeofday(&m_end,NULL);
            timeDiff = time_diff(m_start,m_end);

            if(timeDiff > 160 && timeDiff < 1060)//560us
            {
                return; 
            }
        }
    }
}

char IrRemote::logic_value()
{
    gettimeofday(&m_start,NULL);
    while(!gpio->read()); //低等待
    //cout<<"logic_value: Get High"<<endl;
    gettimeofday(&m_end,NULL);
    timeDiff = time_diff(m_start,m_end);
    //cout<<"logic_value: Low timeDiff is "<< timeDiff<<endl;

    if(timeDiff > 160 && timeDiff < 1060)//低电平560us
    {
        gettimeofday(&m_start,NULL);
        while(gpio->read());//是高就等待
        gettimeofday(&m_end,NULL);
        timeDiff = time_diff(m_start,m_end);
        //cout<<"logic_value: High timeDiff is " << timeDiff<< endl;

        if(timeDiff > 160 && timeDiff < 1060)//接着高电平560us
            return 0;
        else if(timeDiff > 1300 && timeDiff < 2100) //接着高电平1.7ms
            return 1;
    }
    return -1;
}

void IrRemote::pulse_deal()
{
    int i;
    //执行8个0
    for(i=0; i<8; i++)
    {
        int v = logic_value();
        if(v != 0) //不是0
        {
            cout<<"pulse_deal: non zero in 8 zero! index " << i <<endl;    
            return;
        }
    }  
    //执行6个1
    for(i=0; i<6; i++)
    {
        if(logic_value()!= 1) //不是1
        {
            cout<<"pulse_deal: non one in 6 one!"<<endl;    
            return;
        }
    }  
    //执行1个0
    if(logic_value()!= 0) //不是0
    {
        cout<<"pulse_deal: non zero in 1 zero!"<<endl;    
        return;
    } 
    //执行1个1
    if(logic_value()!= 1) //不是1
    {
        cout<<"pulse_deal: non one in 1 one!"<<endl;    
        return;
    } 
    //解析遥控器编码中的command指令
    pthread_mutex_lock(&m_mutexLock);
    irCode=0x00;//清零
    for(i=0; i<16;i++ )
    {
        if(logic_value() == 1)
        {
            irCode |=(1<<i);
        }
    }
    pthread_mutex_unlock(&m_mutexLock);
    return;
}
unsigned int IrRemote::getIrCode()
{
    return irCode;
}

void IrRemote::stopIrRemote()
{
    if(running ==1)
    {
        cout << "exit..." << endl;
        running = 0;
        void* result;
        pthread_join(m_thread, &result);
        pthread_mutex_destroy(&m_mutexLock);
        delete gpio;
    }
}

float IrRemote::time_diff(struct timeval start, struct timeval end)
{
    //retnru in microsecond
    return (end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);
}