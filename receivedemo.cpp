#include <stdio.h>
#include "rc-switch/RCSwitch.h"
#include <time.h>
#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {

    printf("Starting receivedemo\n");
    printf("\n");
    uint32_t Data = 0;
    int T1,T2,T3;
    int PIN = 2; // this is pin 13, aka GPIO22 on the PI3, see https://www.element14.com/community/servlet/JiveServlet/previewBody/73950-102-10-339300/pi3_gpio.png
    int PINT = 0;

    if (wiringPiSetup () == -1) {
        printf("ERROR\n");
        return 1;
    }

    pullUpDnControl(PIN, PUD_OFF);

    RCSwitch mySwitch = RCSwitch();
    RCSwitch mySwitchT = RCSwitch();

    mySwitch.enableReceive(PIN);
    mySwitchT.enableTransmit(PINT);
    mySwitchT.setProtocol(1);

    printf("Listening\n");
    time_t tOld;
    int flag=0;
    uint32_t command;
    while(true) {

        if (mySwitch.available())
        {
	//cout << "avail" <<endl;
		time_t t = time(NULL);
	    struct tm* aTm = localtime(&t);
         if (t != tOld)
         {
            FILE* hFILE = fopen("/var/www/html/Termo/server/Data", "w");
            Data = mySwitch.getReceivedValue();
	    if(Data/1000000000==0)
	    {
              T1 = Data/1000000;
              T2 = Data/1000%1000;
              T3 = Data%1000;
              printf("%02d:%02d:%02d \n %.1f %.1f %.1f \n", aTm->tm_hour, aTm->tm_min, aTm->tm_sec, T1/10.0, T2/10.0, T3/10.0);
              fprintf(hFILE,"%02d:%02d:%02d %.1f %.1f %.1f", aTm->tm_hour, aTm->tm_min, aTm->tm_sec, T1/10.0, T2/10.0, T3/10.0);
	    }
	    else if(Data/1000000000==1)
	    {
	      T1 = Data/1000000%1000;
              T2 = Data/1000%1000;
              T3 = Data%1000;
              printf("%02d:%02d:%02d \n %.1f %.1f %.1f ALARM \n", aTm->tm_hour, aTm->tm_min, aTm->tm_sec, T1/10.0, T2/10.0, T3/10.0);
              fprintf(hFILE,"%02d:%02d:%02d %.1f %.1f %.1f ALARM", aTm->tm_hour, aTm->tm_min, aTm->tm_sec, T1/10.0, T2/10.0, T3/10.0);
	    }
	    else
	    {
		printf("%u \n", Data);
 		fprintf(hFILE,"%u", Data);
	    }
            mySwitch.resetAvailable();
            fclose(hFILE);
            tOld = t;
         }
        }
        //________________________________________Чтение и отправка
        ifstream fin("/var/www/html/Termo/server/Send");
        if( fin.is_open()  )
        {
            fin >> command;
            cout << command << endl;
            mySwitchT.send(command, 32);
	    //cout << "close" << endl;
            fin.close();
	    //cout << "remove" << endl;
            remove("/var/www/html/Termo/server/Send");
	}
	//cout << "delay" << endl;
        delay(100);
    }
}
