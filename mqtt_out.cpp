//////////////////////////  12.1.2019  KN /////////////////////////////



#include <cstdio>
#include <cstring>
#include <string>
#include <functional>     	// c++11

#include <iostream>        	// due to: cout

#include <fstream>
#include <jsoncpp/json/json.h>

#include <mutex>
#include <condition_variable>

using std::cout;
using std::endl;
 


#include "mqtt.h"

#include "mqtt_1.h"
#include "mqtt_config.h"

using namespace std;

extern mutex xmut;
extern std::condition_variable cv;



/////////////////////////////////////////////////////////////////////////
//////////
//////////    MQTT OUT
//////////
/////////////////////////////////////////////////////////////////////////



Mqtt_out::Mqtt_out(const char *id, const char *host, int port) : mosquittopp(id)

{

	
	int con_stat=-100;

	cout << " \n+++ MQTT OUT START +++" << endl;

	int keepalive_out = keep_alive_out;


	//con_stat=connect(host, port, (int) keepalive_out);

	//cout << " \n+++ OUT MQTT connection status="<< con_stat <<  endl;

	//     username and passwd defined in  \\ mqtt_1.h \\

	if(username_pw_set(user_name,pass_wd) != MOSQ_ERR_SUCCESS)

	{ cout << " \n   +++ Username failed  " << endl;
	} 
	else
	{ cout << " \n+++ Username OK +++  " << endl;
	};

	

};

//////////////////////////////// IoTTi IOT course format //////////////////////


////////// ON CONNECT ///////////////////////////////////////////////////





void Mqtt_out::on_connect(int rc)

{

	printf("+++ OUT MQTT Connected with code %d.  +++\n", rc);

	if(rc == 0)
	{

		/* Only attempt to subscribe on a successful connect. */

		subscribe(NULL, in_topic);

		printf("+++ IN MQTT subscription to topic = ");printf(out_topic);

		printf("    +++\n");

	}

}




