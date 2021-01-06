#include "ofApp.h"

void ofApp::setup(){
  client.begin("public.cloud.shiftr.io", 1883);
  client.connect("openframeworks", "public", "public");

  ofAddListener(client.onOnline, this, &ofApp::onOnline);
  ofAddListener(client.onOffline, this, &ofApp::onOffline);
  ofAddListener(client.onMessage, this, &ofApp::onMessage);
}

void ofApp::update() {
  client.update();
}

void ofApp::exit(){
  client.disconnect();
}

void ofApp::onOnline(){
  ofLog() << "online";

  client.subscribe("hello");
}

void ofApp::onOffline(){
  ofLog() << "offline";
}

void ofApp::onMessage(ofxMQTTMessage &msg){
  ofLog() << "message: " << msg.topic << " - " << msg.payload;
}

void ofApp::keyPressed(int key){
  client.publish("hello", "world");
}
