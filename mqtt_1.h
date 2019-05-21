//  KN 12.1.2019


//
//   MQTT client setup
//



#include <mosquittopp.h>
#include <math.h>


        extern long count1;




class Mqtt_in : public mosqpp::mosquittopp
{

	char buf[251]; 

	public:
		Mqtt_in(const char *id, const char *host, int port);
		~Mqtt_in(){};

		void on_connect(int rc);
		void on_message(const struct mosquitto_message *message);
		void on_subscribe(int mid, int qos_count, const int *granted_qos);
		void mq_send2(const char mess[]);
		void IoTTi(void);


};


class Mqtt_out : public mosqpp::mosquittopp
{

	public:
		Mqtt_out(const char *id, const char *host, int port);
		~Mqtt_out(){};

		void on_connect(int rc);

		void mq_send3(const char mess[]);
			


};


