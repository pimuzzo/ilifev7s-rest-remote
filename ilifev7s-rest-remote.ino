#include <stdio.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

#define HTTP_REST_PORT 80
#define WIFI_RETRY_DELAY 500
#define MAX_WIFI_INIT_RETRY 50
#define DOUBLE_PRESS_BUTTON_DELAY 1000

// edit start
const char* wifi_ssid = "";
const char* wifi_passwd = "";
const uint16_t ir_led = 4; // D2
// edit end

const char* previous_action = "";

uint16_t goHomeRawData[67] = {8762, 4542, 484, 628, 484, 626, 486, 602, 510, 602,
                              456, 654, 486, 628, 484, 1720, 506, 602, 508, 1718,
                              486, 628, 484, 1720, 506, 602, 506, 1716, 486, 628,
                              484, 1718, 508, 600, 482, 1744, 484, 628, 484, 626,
                              486, 604, 508, 1714, 482, 630, 484, 628, 484, 628,
                              484, 602, 510, 1716, 482, 1742, 484, 1716, 510, 600,
                              482, 1744, 484, 1740, 486, 1714, 508}; // UNKNOWN 5F9BBF19

ESP8266WebServer http_rest_server(HTTP_REST_PORT);
IRsend irsend(ir_led);

int init_wifi() {
  int retries = 0;
  Serial.println("");
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

  if(action == "clean" || action == "stop") {
    irsend.sendNEC(0x02aa22dd);
  } else if(action == "wake_up") {
    irsend.sendNEC(0x02aa22dd);
    delay(DOUBLE_PRESS_BUTTON_DELAY);
    irsend.sendNEC(0x02aa22dd);
  } else if(action == "home") {
    irsend.sendRaw(goHomeRawData, 67, 38);
  } else if(action == "spot") {
    irsend.sendNEC(0x02aa7788);
  } else if(action == "edge") {
    irsend.sendNEC(0x02aa9966);
  } else {
      Serial.println("Action not found");
      http_rest_server.send(400);
      return;
  }

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
  irsend.begin();

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
