#include <WiFi.h>
#include <time.h>
 
const char* ssid     = "GregAir5G";     
const char* password = "Gtheybzhbkj777!";

void setup() {
  Serial.begin(115200);         
  delay(10);
  Serial.println('\n');
  
  WiFi.begin(ssid, password);             // Connect to the network
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(500);
    Serial.print('.');
  }
  Serial.println('\n');
  Serial.println("Connection established");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP()); 

}
 
void loop() { 

}