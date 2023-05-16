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
  if (key=='s') {
    client.self_loop = !client.self_loop;
    ofLogNotice("Client is now self-looping:") << client.self_loop;
  } else if (key=='2') {
    ofLogNotice("Setting frame rate") << 2;
    ofSetFrameRate(2);
  } else if (key=='6') {
    ofLogNotice("Setting frame rate") << 60;
    ofSetFrameRate(60);
  } else if (key=='b') {
    for (size_t i=0; i<5; i++)
      client.publish("hello", "burst");
  } else {
    client.publish("hello", "world");
  }
}
