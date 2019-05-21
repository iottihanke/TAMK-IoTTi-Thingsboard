
///////////////////////////  12.1.2019  KN /////////////////////////////



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


//////////////////////////////////////////////////////////////////////////
//////////
//////////    MQTT
//////////
/////////////////////////////////////////////////////////////////////////

Mqtt_out mqt_out(out_topic, out_host_addr, out_port_number);


Mqtt_in::Mqtt_in(const char *id, const char *host, int port) : mosquittopp(id)
{

	///////////////////////////////////////////////////////////
	//	MQTT IN
	///////////////////////////////////////////////////////////

	int keepalive_in = keep_alive_in;

	/* Connect immediately. This could also be done by calling
	 * mqtt_tempconv->connect(). */

	connect(host, port, (int) keepalive_in);


	///////////////////////////////////////////////////////////
	//	MQTT OUT object
	///////////////////////////////////////////////////////////

	int con_stat=-100;

	cout << " \n+-+ MQTT IN/OUT CONNECTION +-+" << endl;

	int keepalive_out = keep_alive_out;

	// On connect OUT	

	con_stat=mqt_out.connect(out_host_addr, out_port_number, keepalive_out);

	cout << " \n+-+ IN/OUT MQTT connection status="<< con_stat <<  endl;


};


////////// ON CONNECT IN ///////////////////////////////////////////////////


void Mqtt_in::on_connect(int rc)
{
	printf("--- IN MQTT Connected with code %d.  ---\n", rc);
	if(rc == 0){
		/* Only attempt to subscribe on a successful connect. */
		subscribe(NULL, in_topic);
	printf("--- IN MQTT subscription to topic = ");printf(in_topic);
	printf("    ---\n");

	}
}

//////////////////////////  ON SUBSCRIBE ///////////////////////////////////////

void Mqtt_in::on_subscribe(int mid, int qos_count, const int *granted_qos)
{

	printf("--- IN  MQTT Topic subscription succeeded.   ---\n");

	printf("    ---\n");
	printf("--- ThingsBoard user name:  ");
	;printf( user_name);
	printf("    ---\n");
	printf("--- ThingsBoard out topic:  ");
	;printf( out_topic);
	printf("    ---\n");
	printf("--- ThingsBoard host address:  ");
	;printf( out_host_addr);
	printf("    ---\n");


}


//////////////////////////// Message received //////////////////////////////////

void Mqtt_in::on_message(const struct mosquitto_message *message)
{
	


	printf("\n");
	//cout << "IN Msg received - " << endl;
        printf(" IN Msg received "); 

	if(!strcmp(message->topic, in_topic))
	{
		memset(buf, 0, 151*sizeof(char));
		/* Copy N-1 bytes to ensure always 0 terminated. */
		memcpy(buf, message->payload, 150*sizeof(char));

		/// IoTTi format
		if((buf[0]=='I') && (buf[1]=='O') && (buf[2]=='T') && (buf[3]=='J') && (buf[4]=='S'))  /// ICT course

          		{  IoTTi();  };


	}  // IoTTi topic
}

//////////////////////////////// IoTTi IOT course format //////////////////////



////////////////////////////////////////////////////////////////////////////////
///
///		MQTT IOTTI
///
////////////////////////////////////////////////////////////////////////////////


								// save data to global table 

void Mqtt_in::IoTTi( void)
{

    Json::Reader reader;
    Json::Value obj;


	std::string js = " ";
	
        char st1[250];

	char sensor_name1[80];
	char sensor_path1[80];
	char sensor_unit1[10];


	//cout << " * " << endl ;
	//cout << "buf = " << buf << endl ;


	int i=0;
	while(buf[i]!=0)
	{
          st1[i]=buf[i+6];	// remove "IOTJS="  from beging of MQTT message
 	//  cout << st1[i] ; 
	  i++;
	}
	cout << " I = " << i << endl ;

	st1[i]='\0';


	//cout << "+ " << endl ;
	//cout << "ST1 = " << st1 << endl ;

	
	std::string hp;
	std::string hp2;

	hp=st1;
	
	cout << "hp = " << hp << endl ;

	sprintf(sensor_name1,"-");
	sprintf(sensor_path1,"dummy");
	float sensor_value1=-2000;
	sprintf(sensor_unit1,"-");
	sprintf(sensor_data,"?");


 	if(reader.parse(hp, obj)==true)
	{     // Reader can also read strings
  

		//   Check mqtt message content


		if(obj["S_name"].isString())
		{
			hp2=obj["S_name"].asString();
			strncpy(sensor_name1,hp2.c_str(),hp2.length());
			sensor_name1[hp2.length()]='\0';
 			cout << "  Sensor name: " << sensor_name1 ;
		};

		if(obj["S_value"].isDouble())
		{
			sensor_value1=obj["S_value"].asFloat();
			cout << "       Sensor value Float: " << sensor_value1 ;
		};

		if(obj["S_value"].isInt())
		{
			sensor_value1=obj["S_value"].asInt();
			cout << " Sensor value Int: " << sensor_value1 ;
		};


		if(obj["S_path"].isString())
		{
			hp2=obj["S_path"].asString();
			strncpy(sensor_path1,hp2.c_str(),hp2.length());
			sensor_path1[hp2.length()]='\0';
 			cout << " Sensor path: " << sensor_path1 ;
		};

		if(obj["S_unit"].isString())
		{
			hp2=obj["S_unit"].asString();
			strncpy(sensor_unit1,hp2.c_str(),hp2.length());
			sensor_unit1[hp2.length()]='\0';
 			cout << " Sensor unit: " << sensor_unit1 ;
		};

		if(obj["S_data"].isString())
		{
			hp2=obj["S_data"].asString();
			strncpy(sensor_data,hp2.c_str(),hp2.length());
			sensor_data[hp2.length()]='\0';
 			cout << " Sensor data: " << sensor_data ;
		};

		cout << " done " << endl;



		char sb_buf[150];
			       //  0000000000111111111122222222223333333333
                               //  0123456789012345678901234567890123456789
		//sprintf(sb_buf,"{\"temperature2\":19.9,\"humidity2\":3.2}");

		sprintf(sb_buf,"{\"%s\":%4.2f}",sensor_name1,sensor_value1);

		cout << " sb_buf "<< strlen(sb_buf) << endl;

		if(mqt_out.publish(NULL,out_topic,strlen(sb_buf),sb_buf,0,0)==MOSQ_ERR_NO_CONN)
		{
	
			cout << "  ++++ RE CONNECT +++ "<< endl; 
			mqt_out.reconnect();
		} 
		else
		{

			cout << "+++ Publish >" ; 

			cout << "Topic = "<< out_topic <<"   Tb message = " << sb_buf << endl;
		};

		

	} else   ///       Parsing error handling 
	{
		cout << " --- Error in JSON parsing ---- " << endl;
	};

	;				

}




