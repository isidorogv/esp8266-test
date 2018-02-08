// ---------------
// Wifi scanner example
// ---------------

// This little program looks for available wifis
// around and displays y serial console the results.
// Waits for 50 seconds and restarts the scannig.

 
#include "ESP8266WiFi.h"

void setup() {
  Serial.begin(115200);
delay(1500);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("Seting up completed!!");
}

void loop() {
  Serial.println("Searching WIFIs");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("Ready");
  if (n == 0)
    Serial.println("Wifis not available");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID y RSSI for every one
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      delay(10);
    }
  }
  Serial.println("");

  // Wait a bit before scanning again
  delay(50000);
}
