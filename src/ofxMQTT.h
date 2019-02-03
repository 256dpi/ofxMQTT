#include "ofMain.h"
#include "mosquitto.h"

struct ofxMQTTMessage {
  string topic;
	string payload;
};

class ofxMQTT {
private:
  struct mosquitto *mosq;
  bool alive = false;

  string hostname;
  int port;
  string clientId;
  string username;
  string password;
  string willTopic;
  string willPayload;
  
  int protocol = MQTT_PROTOCOL_V311; // must set to MQTT_PROTOCOL_V31/MQTT_PROTOCOL_V311 before call to connect ()

  int mid = 0;
  int nextMid();
public:
  ofxMQTT();
  ~ofxMQTT();

  void begin(string hostname);
  void begin(string hostname, int port);
  void setWill(string topic);
  void setWill(string topic, string payload);
  bool connect(string clientId);
  bool connect(string clientId, string username, string password);
  void publish(string topic, int qos = 0, bool retain = false);
  void publish(string topic, string payload, int qos = 0, bool retain = false);
  void subscribe(string topic, int qos = 0);
  void unsubscribe(string topic);
  void update();
  bool connected();
  void disconnect();

  void setProtocol(int p);
  int  getProtocol();

  ofEvent<void> onOnline;
  ofEvent<ofxMQTTMessage> onMessage;
  ofEvent<void> onOffline;

  // never call these functions:
  void _on_connect(int rc);
  void _on_disconnect(int rc);
  void _on_message(const struct mosquitto_message *message);
};
