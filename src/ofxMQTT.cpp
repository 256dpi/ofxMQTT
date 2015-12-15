#include "ofxMQTT.h"

using namespace mosqpp;

ofxMQTT::ofxMQTT() : mosquittopp() {
  lib_init();
  bConnected = false;
  bAutoReconnect = true;
}

ofxMQTT::ofxMQTT(string clientID, string host, int port, bool cleanSession) : mosquittopp(clientID.c_str(), cleanSession) {
  lib_init();
  this->clientID = clientID;
  this->host = host;
  this->port = port;
  this->keepAlive = 60;
}

ofxMQTT::~ofxMQTT() {
  lock();

  if (isThreadRunning()) {
      stopThread();
  }

  lib_cleanup();
  unlock();
}

void ofxMQTT::reinitialise(string clientID, bool cleanSession) {
  lock();
  this->clientID = clientID;
  check_error(mosquittopp::reinitialise(clientID.c_str(), cleanSession));
  unlock();
}

void ofxMQTT::setup(string host, int port, int keepAlive) {
  lock();
  this->host = host;
  this->port = port;
  this->keepAlive = keepAlive;
  unlock();
}

void ofxMQTT::connect() {
  check_error(mosquittopp::connect(host.c_str(), port, keepAlive));
  start();
}

void ofxMQTT::connect(string bindAddress) {
  check_error(mosquittopp::connect(host.c_str(), port, keepAlive, bindAddress.c_str()));
  start();
}

void ofxMQTT::reconnect() {
  check_error(mosquittopp::reconnect());
}

void ofxMQTT::disconnect() {
  stop();
  check_error(mosquittopp::disconnect());
}

void ofxMQTT::publish(int mid, string topic, string payload, int qos, bool retain) {
  check_error(mosquittopp::publish(&mid, topic.c_str(), payload.size(), payload.c_str(), qos, retain));
}

void ofxMQTT::subscribe(int mid, string sub, int qos) {
  check_error(mosquittopp::subscribe(&mid, sub.c_str(), qos));
}

void ofxMQTT::unsubscribe(int mid, string sub) {
  check_error(mosquittopp::unsubscribe(&mid, sub.c_str()));
}

void ofxMQTT::start() {
  lock();
  startThread(true);
  unlock();
}

void ofxMQTT::stop() {
  lock();
  stopThread();
  unlock();
}

void ofxMQTT::setUsernameAndPassword(string username, string password) {
  lock();
  this->username = username;
  this->password = password;
  check_error(username_pw_set(username.c_str(), password.c_str()));
  unlock();
}

void ofxMQTT::setKeepAlive(int keepAlive) {
  lock();
  this->keepAlive = keepAlive;
  unlock();
}

void ofxMQTT::setAutoReconnect(bool reconnect) {
  lock();
  this->bAutoReconnect = reconnect;
  unlock();
}

void ofxMQTT::setUserdata(void *userdata) {
  lock();
  this->userdata = userdata;
  unlock();
}

void ofxMQTT::threadedFunction() {
  while (isThreadRunning()) {
    if (lock()) {
      int rc = loop();
      if (MOSQ_ERR_SUCCESS != rc && bAutoReconnect) {
        ofLogError("ofxMQTT") << mosqpp::strerror(rc);
        reconnect();
        ofSleepMillis(20);
      }
      unlock();
    }
  }
}

void ofxMQTT::on_connect(int rc) {
  if (MOSQ_ERR_SUCCESS == rc) {
      bConnected = true;
  } else {
    ofLogError("ofxMQTT") << mosqpp::strerror(rc);
  }

  ofNotifyEvent(onConnect, rc, this);
}

void ofxMQTT::on_disconnect(int rc) {
  if (MOSQ_ERR_SUCCESS == rc) {
      bConnected = false;
  } else {
    ofLogError("ofxMQTT") << mosqpp::strerror(rc);
  }

  ofNotifyEvent(onDisconnect, rc, this);
}

void ofxMQTT::on_message(const struct mosquitto_message *message) {
  ofxMQTTMessage msg;
  msg.mid = message->mid;
  msg.topic = message->topic;
  msg.payload = message->payload;
  msg.payloadlen = message->payloadlen;
  msg.qos = message->qos;
  msg.retain = message->retain;

  ofNotifyEvent(onMessage, msg, this);
}

void ofxMQTT::on_publish(int rc) {
  ofNotifyEvent(onPublish, rc, this);
}

void ofxMQTT::on_subscribe(int mid, int qos_count, const int *granted_qos) {
  ofNotifyEvent(onSubscribe, mid, this);
}

void ofxMQTT::on_unsubscribe(int mid) {
  ofNotifyEvent(onUnsubscribe, mid, this);
}

void ofxMQTT::on_log(int level, const char *str) {
  ofLogVerbose("ofxMQTT") << "on_log : level = " << level << ", str = " << ofToString(str);
}

void ofxMQTT::on_error() {
  ofLogError("ofxMQTT") << "error";
}

void ofxMQTT::check_error(int ret) {
  if (0 < ret) ofLogError("ofxMQTT") << mosqpp::strerror(ret);
}
