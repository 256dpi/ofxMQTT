# ofxMQTT

**MQTT addon for openframeworks based on libmosquitto**

This addon bundles the [libmosquitto](http://mosquitto.org/man/libmosquitto-3.html) library and adds a thin wrapper to get an openframeworks like API.

The first release of the addon only supports QoS0 and the basic features to get going. In the next releases more of the features will be available. Please create an issue if you need a specific functionality.

This addon is an alternative to [ofxMosquitto](https://github.com/hideyukisaito/ofxMosquitto) by @hideyukisaito which didn't get much attention lately.

[Download the latest version of the addon.](https://github.com/256dpi/ofxMQTT/releases/latest)

## Example

The following example connects to the public shiftr.io instance. You can check on your app after a successful connection here: <https://www.shiftr.io/try>.

```c++
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
```

The corresponding header file can be found [here](https://github.com/256dpi/ofxMQTT/blob/master/example-ofxMQTT/src/ofApp.h).

## API

Initialize the object using the hostname of the broker and the brokers port (default: `1883`):

```c++
bool begin(string hostname);
bool begin(string hostname, int port;
```

Set the will message that gets registered on a connect:

```c++
void setWill(string topic);
void setWill(string topic, string payload);
```

Connect to broker using the supplied client id and an optional username and password:

```c++
bool connect(string clientId);
bool connect(string clientId, string username, string password);
```

_This functions returns a value that indicates if the connection has been established successfully._

Publishes a message to the broker with an optional payload:

```c++
void publish(string topic, int qos = 0, retain = false);
void publish(string topic, string payload, int qos = 0, retain = false);
```

Subscribe to a topic:

```c++
void subscribe(string topic, int qos = 0);
```

Unsubscribe from a topic:

```c++
void unsubscribe(string topic);
```

Sends and receives packets:

```c++
void update();
```

_This function should be called in every `update` loop._

Check if the client is currently connected:

```c++
bool connected();
```

Disconnects from the broker:

```c++
void disconnect();
```

You can add event listeners for the following events to receive status updates and incoming messages:

```c++
ofEvent<void> onOnline;
ofEvent<ofxMQTTMessage> onMessage;
ofEvent<void> onOffline;
```
