#include "ofxMQTT.h"

/* mosquitto */

static void on_connect_wrapper(struct mosquitto *, void *userData, int rc) {
  auto *m = (class ofxMQTT *)userData;
  m->_on_connect(rc);
}

static void on_disconnect_wrapper(struct mosquitto *, void *userData, int rc) {
  auto *m = (class ofxMQTT *)userData;
  m->_on_disconnect(rc);
}

static void on_message_wrapper(struct mosquitto *, void *userData, const struct mosquitto_message *message) {
  auto *m = (class ofxMQTT *)userData;
  m->_on_message(message);
}

/* ofxMQTT */

ofxMQTT::ofxMQTT() {
  mosquitto_lib_init();

  mosq = mosquitto_new("ofxMQTT", true, this);
  mosquitto_connect_callback_set(mosq, on_connect_wrapper);
  mosquitto_disconnect_callback_set(mosq, on_disconnect_wrapper);
  mosquitto_message_callback_set(mosq, on_message_wrapper);
}

ofxMQTT::~ofxMQTT() {
  if (alive) {
    mosquitto_disconnect(mosq);
  }

  mosquitto_destroy(mosq);
  mosquitto_lib_cleanup();
}

int ofxMQTT::nextMid() {
  mid++;

  if (mid > 65536) {
    mid = 0;
  }

  return mid;
}

void ofxMQTT::begin(string hostname) { begin(hostname, 1883); }

void ofxMQTT::begin(string hostname, int port) {
  this->hostname = hostname;
  this->port = port;
}

void ofxMQTT::setWill(string topic) { setWill(topic, ""); }

void ofxMQTT::setWill(string topic, string payload) {
  this->willTopic = topic;
  this->willPayload = payload;
}

bool ofxMQTT::connect(string clientId) { return connect(clientId, "", ""); }

bool ofxMQTT::connect(string clientId, string username, string password) {
  this->clientId = clientId;
  this->username = username;
  this->password = password;

  mosquitto_reinitialise(mosq, this->clientId.c_str(), true, this);
  mosquitto_connect_callback_set(mosq, on_connect_wrapper);
  mosquitto_disconnect_callback_set(mosq, on_disconnect_wrapper);
  mosquitto_message_callback_set(mosq, on_message_wrapper);

  if (!this->username.empty() && !this->password.empty()) {
    mosquitto_username_pw_set(mosq, this->username.c_str(), this->password.c_str());
  }

  if (!willTopic.empty() && !willPayload.empty()) {
    mosquitto_will_set(mosq, willTopic.c_str(), (int)willPayload.length(), willPayload.c_str(), 0, false);
  }

  int rc = mosquitto_connect(mosq, hostname.c_str(), port, 60);

  if (rc != MOSQ_ERR_SUCCESS) {
    ofLogError("ofxMQTT") << "Connect error: " << mosquitto_strerror(rc);
    return false;
  }

  return true;
}

void ofxMQTT::publish(string topic, int qos, bool retain) { publish(topic, "", qos, retain); }

void ofxMQTT::publish(string topic, string payload, int qos, bool retain) {
  int mid = nextMid();
  mosquitto_publish(mosq, &mid, topic.c_str(), (int)payload.length(), payload.c_str(), qos, retain);
}

void ofxMQTT::subscribe(string topic, int qos) {
  int mid = nextMid();
  mosquitto_subscribe(mosq, &mid, topic.c_str(), qos);
}

void ofxMQTT::unsubscribe(string topic) {
  int mid = nextMid();
  mosquitto_unsubscribe(mosq, &mid, topic.c_str());
}

void ofxMQTT::update() {
  int rc1 = mosquitto_loop(mosq, 0, 1);
  if (rc1 != MOSQ_ERR_SUCCESS) {
    ofLogError("ofxMQTT") << "Loop error: " << mosquitto_strerror(rc1);

    int rc2 = mosquitto_reconnect(mosq);
    if (rc2 != MOSQ_ERR_SUCCESS) {
      ofLogError("ofxMQTT") << "Reconnect error: " << mosquitto_strerror(rc2);
    }
  }
}

bool ofxMQTT::connected() { return alive; }

void ofxMQTT::disconnect() { mosquitto_disconnect(mosq); }

void ofxMQTT::_on_connect(int rc) {
  alive = (rc == MOSQ_ERR_SUCCESS);

  if (alive) {
    ofNotifyEvent(onOnline, this);
  } else {
    ofNotifyEvent(onOffline, this);
  }
}

void ofxMQTT::_on_disconnect(int /*rc*/) {
  alive = false;
  ofNotifyEvent(onOffline, this);
}

void ofxMQTT::_on_message(const struct mosquitto_message *message) {
  string payload((char *)message->payload, (uint)message->payloadlen);

  ofxMQTTMessage msg; 
  msg.topic = message->topic;
  msg.payload = payload;

  ofNotifyEvent(onMessage, msg, this);
}
