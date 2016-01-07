#pragma once

#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include "mraa.hpp"

namespace upm
{
    class IrRemote
    {
    private:
        mraa::Gpio* gpio;
        pthread_t m_thread;
        int irCode, running;
        struct timeval m_start, m_end;
        float timeDiff;
        mraa::Result remote_decode();
        char logic_value();
        void pulse_deal();
        float time_diff(struct timeval start, struct timeval end);
        void mraa_error(mraa::Result error_code);
        void decodeIr();
        pthread_mutex_t m_mutexLock;

    public:
        IrRemote(int digitalPin);
        ~IrRemote();

        void stopIrRemote();
        unsigned int getIrCode();
        static void* decodeThread(void* cs);
    };
}