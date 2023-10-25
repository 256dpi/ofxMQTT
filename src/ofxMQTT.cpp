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
    
#ifdef ofxMQTT_THREADED
    mosquitto_threaded_set(mosq, true);
#endif
    
  mosquitto_connect_callback_set(mosq, on_connect_wrapper);
  mosquitto_disconnect_callback_set(mosq, on_disconnect_wrapper);
  mosquitto_message_callback_set(mosq, on_message_wrapper);
}

ofxMQTT::~ofxMQTT() {
  if (alive) {
    mosquitto_disconnect(mosq);
  }

#ifdef ofxMQTT_THREADED

mosquitto_loop_stop(mosq, true);
#endif

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
    
#ifdef ofxMQTT_THREADED
    mosquitto_threaded_set(mosq, false);
#else
    mosquitto_threaded_set(mosq, true);
#endif


  if (!this->username.empty() && !this->password.empty()) {
    mosquitto_username_pw_set(mosq, this->username.c_str(), this->password.c_str());
  }

  if (!willTopic.empty() && !willPayload.empty()) {
    mosquitto_will_set(mosq, willTopic.c_str(), (int)willPayload.length(), willPayload.c_str(), 0, false);
  }
    
    int rc = mosquitto_connect(mosq, hostname.c_str(), port, 60);
#ifdef ofxMQTT_THREADED
//    mosquitto_loop(mosq, 1, 0);
    int rr =   mosquitto_loop_start(mosq);
    ofLogNotice("ofxMQTT::connect() thread res:") <<  mosquitto_strerror(rr);
#else
    
    mosquitto_loop(mosq, 1, 0);
#endif
    
    if (rc != MOSQ_ERR_SUCCESS) {
        ofLogError("ofxMQTT") << "Connect error: " << mosquitto_strerror(rc);
        return false;
  } else {
  }

  return true;
}

auto ofxMQTT::publish(string topic, int qos, bool retain) -> int { return publish(topic, "", qos, retain); }

auto ofxMQTT::publish(string topic, string payload, int qos, bool retain) -> int {
  int mid = nextMid();
  mosquitto_publish(mosq, &mid, topic.c_str(), (int)payload.length(), payload.c_str(), qos, retain);
    return mid;
}

bool ofxMQTT::subscribe(string topic, int qos) {
  int mid = nextMid();
  return mosquitto_subscribe(mosq, &mid, topic.c_str(), qos) == MOSQ_ERR_SUCCESS;
}

void ofxMQTT::unsubscribe(string topic) {
  int mid = nextMid();
  mosquitto_unsubscribe(mosq, &mid, topic.c_str());
}


size_t got_ = 0;

void ofxMQTT::update() {
#ifdef    ofxMQTT_THREADED
    
#else
    
    got_ = 0;
  int rc1 = mosquitto_loop(mosq, 0, 1);
  if (rc1 != MOSQ_ERR_SUCCESS) {
    ofLogError("ofxMQTT") << "Loop error: " << mosquitto_strerror(rc1);

    int rc2 = mosquitto_reconnect(mosq);
    if (rc2 != MOSQ_ERR_SUCCESS) {
      ofLogError("ofxMQTT") << "Reconnect error: " << mosquitto_strerror(rc2);
    }
  }
    
    if (got_>0) update();
#endif
    
}

bool ofxMQTT::connected() { return alive; }

void ofxMQTT::disconnect() {
    mosquitto_disconnect(mosq);
    
}

void ofxMQTT::_on_connect(int rc) {
    ofLogNotice("ofxMQTT::_on_connect()") << rc;
  alive = (rc == MOSQ_ERR_SUCCESS);

  if (alive) {
#ifdef ofxMQTT_THREADED
      
      mosquitto_loop_stop(mosq,false);
      int rr =   mosquitto_loop_start(mosq);
        ofLogNotice("ofxMQTT::_on_connect() thread res:") <<  mosquitto_strerror(rr);
#endif

      
    ofNotifyEvent(onOnline, this);

  } else {
    ofNotifyEvent(onOffline, this);
  }
}

void ofxMQTT::_on_disconnect(int /*rc*/) {
  alive = false;
#ifdef ofxMQTT_THREADED

    mosquitto_loop_stop(mosq, true);
#endif
  ofNotifyEvent(onOffline, this);
}

void ofxMQTT::_on_message(const struct mosquitto_message *message) {
  string payload((char *)message->payload, (uint)message->payloadlen);

  ofxMQTTMessage msg; 
  msg.topic = message->topic;
  msg.payload = payload;
    msg.retain = message->retain;
    msg.qos = message->qos;

  ofNotifyEvent(onMessage, msg, this);
    got_++;
}
