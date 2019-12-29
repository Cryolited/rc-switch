#include <stdio.h>
#include "rc-switch/RCSwitch.h"
#include <time.h>

int main(int argc, char *argv[]) {

    printf("Starting receivedemo\n");
    printf("\n");
    uint32_t Data = 0;
    int T1,T2,T3;
    int PIN = 2; // this is pin 13, aka GPIO22 on the PI3, see https://www.element14.com/community/servlet/JiveServlet/previewBody/73950-102-10-339300/pi3_gpio.png

    if (wiringPiSetup () == -1) {
        printf("ERROR\n");
        return 1;
    }

    // put the PIN into no-pull/up down state:
    // see https://github.com/ninjablocks/433Utils/issues/21
    pullUpDnControl(PIN, PUD_OFF);

    RCSwitch mySwitch = RCSwitch();

    mySwitch.enableReceive(PIN);

    printf("Listening\n");
    time_t tOld;
    int flag=0;
    while(true) {

        if (mySwitch.available())
        {
		time_t t = time(NULL);
	    struct tm* aTm = localtime(&t);
         if (t != tOld)
         {
            FILE* hFILE = fopen("/var/www/html/Termo/server/Data", "w");
            Data = mySwitch.getReceivedValue();
            T1 = Data/1000000;
            T2 = Data/1000%1000;
            T3 = Data%1000;
            printf("%02d:%02d:%02d \n %.1f %.1f %.1f \n", aTm->tm_hour, aTm->tm_min, aTm->tm_sec, T1/10.0, T2/10.0, T3/10.0);
            fprintf(hFILE,"%02d:%02d:%02d %.1f %.1f %.1f", aTm->tm_hour, aTm->tm_min, aTm->tm_sec, T1/10.0, T2/10.0, T3/10.0);
            mySwitch.resetAvailable();
            fclose(hFILE);
            tOld = t;
         }
        }
        //________________________________________Чтение и отправка
        FILE* Send = fopen("/var/www/html/Termo/server/Send","r");
        if( Send != NULL && flag != 1 )
        {
            int64_t command;
            fscanf(Send, "%i", &command);
            printf("Sended : %i", command);
            fclose(Send);
            flag = 1;
            //remove("/var/www/html/Termo/server/Send");
        }
        else
            printf("Cannot open directory file.");
        delay(100);
    }
}
