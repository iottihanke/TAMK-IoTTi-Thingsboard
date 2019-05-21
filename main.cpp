

//////////////////////////////////////  12.1.2019 //////////////////////////////
///
///	g++ main.cpp mqtt_arduino1.cpp -std=c++0x -pthread -lmosquitto -lmosquittopp -lcurl -ljsoncpp
///
///////////////////////////////////////////////////////////////////////////////

#include <signal.h>
#include <sys/time.h>

#include <stdio.h>
#include <string.h>
#include <atomic>
#include <iostream>

#include <fstream>
#include <jsoncpp/json/json.h>

#include <mutex>
#include <condition_variable>

#include <thread>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>

#include "mqtt_main.h"
#include "mqtt_1.h"
#include "mqtt_config.h"

long  count =0;


std::atomic < long>  time_i;

std::atomic <long> time_b;
std::atomic <unsigned char> tmr_flag;


using namespace std;

mutex xmut,mut2;
std::condition_variable cv;

///////////////////////////////////////////////////////////////
/////
/////		MQTT thread 
/////
///////////////////////////////////////////////////////////////

void mqtt_loop()
{
        int in_th,out_th;

	Mqtt_in *mqttHdl_in = new Mqtt_in(in_topic, in_host_addr, in_port_number);

	Mqtt_out *mqttHdl_out = new Mqtt_out(out_topic, out_host_addr, out_port_number);


	mosqpp::lib_init();

	while(1)
	{

		in_th = mqttHdl_in->loop();
		if(in_th)	{
				mqttHdl_in->reconnect();
				usleep(1e5);
			}

		out_th = mqttHdl_out->loop();
		if(out_th)	{
				mqttHdl_out->reconnect();
				usleep(1e5);
			}


	}

	mosqpp::lib_cleanup();
}

//////////////////////////////////// mqtt thread end /////////////




//////////////////////////////////////////////////////////////////
/////
/////		Timer thread
/////
/////////////////////////////////////////////////////////////////

/////////////////////////////////// timer routine ///////////////


void timer_routine (int signum)
{
	long a;
    	time_b++;


    if (time_b>1000){ time_b=0; tmr_flag=1;} ;   // 1000 x 10mS = sending rate  = 10 sec 

	a=time_b;
	time_i=a;	


}


void timer_start()
{
	struct sigaction sa;
 	struct itimerval timer;

 	/* Install timer_handler as the signal handler for SIGVTALRM. */
 	memset (&sa, 0, sizeof (sa));
 	sa.sa_handler = &timer_routine;
 	sigaction (SIGALRM, &sa, NULL);

 	/* Configure the timer to expire after 250 msec... */
 	timer.it_value.tv_sec = 0;
 	timer.it_value.tv_usec = 100000;

 	/* ... and every 3 000 msec after that. */
 	timer.it_interval.tv_sec = 0;	    // Timer time set sec	
 	timer.it_interval.tv_usec = 10000;  // Timer time set usec  //  10mS   //

 	/* Start a virtual timer. It counts down whenever this process is executing. */
	setitimer (ITIMER_REAL, &timer, NULL);

	while(1);
}

//////////////////////////////////// timer thread end /////////////////


///////////////////////////////////////////////////////////////////////////
////////
///////			Send URL
///////
//////////////////////////////////////////////////////////////////////////


void Send_url(void)

{
  while(1)
	{ 
	
	if(tmr_flag==1)
		{ 
		 
		printf ("Counting: %ld count\n",count);
 		 		
		count++; 
		
		if (count>10)
			{ count=0; };
		 
		 tmr_flag=0;
		}	 	
		
	}

}


//////////////////////////////////// send URL thread end /////////////////


int main(int argc, char *argv[])
{
	int i;
	tmr_flag=0;


//
//Mqtt_in mqtt_in;



	thread mqtt_th (mqtt_loop);
        thread send_th(Send_url);
       	thread tmr_th(timer_start);


	printf ("\n   Subscribed to listen MQTT messages \n");
	printf   ("   Sending to IoT ticket when message arrived \n\n");


	///
        /// 		start threads
	///	

	mqtt_th.join();
        send_th.join();
	tmr_th.join();	

	return 0;
}