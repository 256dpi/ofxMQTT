#ifndef OFXMQTT_H
#define OFXMQTT_H

#include "mosquitto.h"
#include "ofMain.h"

#//define ofxMQTT_THREADED 1


struct ofxMQTTMessage {
  string topic;
  string payload;
    bool retain;
    int qos;
};

class ofxMQTT {
 private:
  struct mosquitto *mosq;
  std::atomic<bool> alive = false;

  string hostname;
  int port;
  string clientId;
  string username;
  string password;
  string willTopic;
  string willPayload;
	
	std::unique_ptr<ofThreadChannel<ofxMQTTMessage>> messagesChannel = std::make_unique<ofThreadChannel<ofxMQTTMessage>>();
	ofxMQTTMessage buffer_message;

  std::atomic<int> mid = 0;
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
  auto publish(string topic, int qos = 0, bool retain = false) -> int;
  auto publish(string topic, string payload, int qos = 0, bool retain = false) -> int;
  bool subscribe(string topic, int qos = 0);
  void unsubscribe(string topic);
  void update();
  bool connected();
  void disconnect();

  ofEvent<void> onOnline;
  ofEvent<ofxMQTTMessage> onMessage;
  ofEvent<void> onOffline;

	std::optional<ofxMQTTMessage> getMessage() {
		if (messagesChannel->tryReceive(buffer_message)) {
			return buffer_message;
		} else {
			return {};
		}
	}

  // never call these functions:
  void _on_connect(int rc);
  void _on_disconnect(int rc);
  void _on_message(const struct mosquitto_message *message);
};

#endif
