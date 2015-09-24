#pragma once

namespace upm
{
    class Geometry
    {
    private:
    	bool running;
		pthread_t m_thread;
		static void* timeThread(void *cs);

    public:
        Geometry();
        ~Geometry();

        void startDetect();
	void stopDetect();

        unsigned char detect;
        bool isDetect;
    };
}
