#include <WiFi.h>
#include <WiFiClient.h>

#define LED       2
#define MAX_NUM   4096

uint16_t num, dst;
uint8_t buffer[MAX_NUM];

const char *ssid = "ErykM";
const char *password = "kczewo143";

WiFiClient client;

void setup() {
  pinMode(LED, OUTPUT);

  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  const uint8_t deviceCount = WiFi.scanNetworks();
  for(uint8_t i=0; i<deviceCount; i++) {
    IPAddress ip = WiFi.softAPIP();
    ip[3] = i + 1;

    if(client.connect(ip, 23)) {
      break;
    }
  }
}

void loop() {

  digitalWrite(LED, LOW);

  num = client.available();
  dst = Serial.availableForWrite();
  num = num<dst ? num : dst;
  num = num<MAX_NUM ? num : MAX_NUM;
  client.read(buffer, num);
  Serial.write(buffer, num);

  digitalWrite(LED, HIGH);

  num = Serial.available();
  num = num<MAX_NUM ? num : MAX_NUM;
  Serial.read(buffer, num);
  client.write(buffer, num);
}
