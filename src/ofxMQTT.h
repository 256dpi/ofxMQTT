#include "ofMain.h"
#include "mosquittopp.h"

struct ofxMQTTMessage {
  int mid;
  string topic;
	void *payload;
	int payloadlen;
	int qos;
	bool retain;

  const string payloadAsString() {
      return static_cast<string>((const char *)payload);
  }
};

class ofxMQTT : public mosqpp::mosquittopp, public ofThread {
public:
  ofxMQTT();
  ofxMQTT(string clientID, string host, int port, bool cleanSession=true);
  ~ofxMQTT();

  void setup(string host, int port, int keepAlive=60);
  void reinitialise(string clientID, bool cleanSession);
  void connect();
  void connect(string bindAddress);
  void reconnect();
  void disconnect();
  void publish(int mid, string topic, string payload, int qos=0, bool retain=false);
  void subscribe(int mid, string sub, int qos = 0);
  void unsubscribe(int mid, string sub);

  void threadedFunction();
  void start();
  void stop();

  string getClientID() { return clientID; }
  string getHost()     { return host; }
  int    getPort()     { return port; }
  string getUsername() { return username; }
  string getPassword() { return password; }

  bool isConnected() { return bConnected; };

  void setUsernameAndPassword(string username, string password);
  void setKeepAlive(int keepAlive);
  void setAutoReconnect(bool reconnect);
  void setUserdata(void *userdata);

  ofEvent<int> onConnect;
  ofEvent<ofxMQTTMessage> onMessage;
  ofEvent<int> onDisconnect;
  ofEvent<int> onPublish;
  ofEvent<int> onSubscribe;
  ofEvent<int> onUnsubscribe;

private:
  string clientID;
  string host;
  int port;
  string username;
  string password;
  int keepAlive;
  bool bConnected;
  bool bAutoReconnect;
  void *userdata;
  static string keyfilePath;

  void on_connect(int rc);
  void on_disconnect(int rc);
  void on_message(const struct mosquitto_message *message);
  void on_publish(int rc);
  void on_subscribe(int mid, int qos_count, const int *granted_qos);
  void on_unsubscribe(int mid);
  void on_log(int level, const char *str);
  void on_error();

  void check_error(int ret);

  static int pw_callback(char *buf, int size, int rwflag, void *userdata) {
      strcpy(buf, keyfilePath.c_str());
      return (int)keyfilePath.size();
  }
};
