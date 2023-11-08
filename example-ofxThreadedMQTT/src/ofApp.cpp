#include "ofApp.h"

void ofApp::setup(){
  client.begin("public.cloud.shiftr.io", 1883);
  client.connect("openframeworks", "public", "public");

  ofAddListener(client.onOnline, this, &ofApp::onOnline);
  ofAddListener(client.onOffline, this, &ofApp::onOffline);
}

void ofApp::update() {
  while (auto m = client.getNextMessage()) {
    ofLogNotice(m->topic) << m->payload;
  }
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

void ofApp::keyPressed(int key){
  if (key=='2') {
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
