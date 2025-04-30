#include "wifi_config.h"

// WiFi-innstillinger
const char* ssid = "Elsys11";
const char* password = "12345678";

// Variabel
int tryes = 0;


void setupWifi(){
  WiFi.disconnect(true);
  delay(100);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Kobler til WiFi...");
      tryes += 1;
      if (tryes > 10) {
        Serial.println("Kunne ikke koble til WiFi, restarter ESP-en...");
        delay(200);
        ESP.restart();
      }
  }
  tryes = 0;
  Serial.println("WiFi tilkoblet!");
  Serial.print("IP-adresse: ");
  Serial.println(WiFi.localIP());
}