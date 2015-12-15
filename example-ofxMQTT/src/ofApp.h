#pragma once

#include "ofMain.h"
#include "ofxMQTT.h"

class ofApp : public ofBaseApp{
public:
  ofxMQTT client;
  void setup();
  void exit();

  void update();
  void draw();

  void onOnline();
  void onOffline();
  void onMessage(ofxMQTTMessage &msg);

  void keyPressed(int key);
};
