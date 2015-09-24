#include <iostream>
#include "geometry.h"
using namespace std;
using namespace upm;

int main()
{
	Geometry geo;
	geo.startDetect();
	while(1)
	{
		if(geo.isDetect)
		{
			cout << int(geo.detect) << endl;
			geo.isDetect = false;
		}
	}
	return 0;
}
