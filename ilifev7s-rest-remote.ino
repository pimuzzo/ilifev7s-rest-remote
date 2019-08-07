#include <stdio.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

#define HTTP_REST_PORT 80
#define WIFI_RETRY_DELAY 500
#define MAX_WIFI_INIT_RETRY 50

const char* wifi_ssid = ""; // edit me
const char* wifi_passwd = ""; // edit me

const char* previous_action = "";

ESP8266WebServer http_rest_server(HTTP_REST_PORT);

int init_wifi() {
  int retries = 0;
  Serial.print("Connecting to WiFi AP");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_passwd);
  // check the status of WiFi connection to be WL_CONNECTED
  while ((WiFi.status() != WL_CONNECTED) && (retries < MAX_WIFI_INIT_RETRY)) {
    retries++;
    delay(WIFI_RETRY_DELAY);
    Serial.print(".");
  }
  Serial.println("");
  return WiFi.status();
}

void manage_get() {
  StaticJsonDocument<200> json_doc;
  char JSONmessageBuffer[200];
  json_doc["action"] = previous_action;
  serializeJson(json_doc, JSONmessageBuffer);
  http_rest_server.send(200, "application/json", JSONmessageBuffer);
}

void manage_post() {
  StaticJsonDocument<500> json_doc;
  String post_body = http_rest_server.arg("plain");
  Serial.println(post_body);

  deserializeJson(json_doc, post_body);
  String action = previous_action = json_doc["action"];
  Serial.print("Executing action ");
  Serial.println(action);
  http_rest_server.send(204);
}

void config_rest_server_routing() {
  http_rest_server.on("/", HTTP_GET, []() {
    http_rest_server.send(200, "text/html", "Welcome to the ilifev7s-rest-remote server");
  });
  http_rest_server.on("/ilifev7s", HTTP_GET, manage_get);
  http_rest_server.on("/ilifev7s", HTTP_POST, manage_post);
}

void setup(void) {
  Serial.begin(115200);

  if (init_wifi() == WL_CONNECTED) {
    Serial.print("Connected to ");
    Serial.print(wifi_ssid);
    Serial.print(" with IP ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.print("Error connecting to: ");
    Serial.println(wifi_ssid);
  }

  config_rest_server_routing();
  http_rest_server.begin();
  Serial.println("HTTP REST Server Started");
}

void loop(void) {
  http_rest_server.handleClient();
}
