/*********
  HueZeptrion

  Disable eindows firewall for OTA
  Engage switch to enable OTA
*********/

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <UserSettings.h>

const int button = 5; //Shelly

/* uncomment and change.  remove UserSettings include
// wifi settings
const char* ssid = "<change>";
const char* password = "<change>";

// HUE settings
const char* bridge_ip = "<change>";
const int port = 80;
String user="<change>";
*/

// HUE commands
String hue_toggle="{\"state\":{\"flag\":true}}";

// send command to hue
void hue_control(String command) {
    
    //wifi still alive?
    if (WiFi.status() != WL_CONNECTED)
    {
      WIFI_Connect();
    }
    
  WiFiClient client;
  
  if (!client.connect(bridge_ip, port)) {
  Serial.println("Connection failed");
  return;
  }
  
  // building string
  client.println("PUT /api/" + user + "/sensors/30  HTTP/1.1");
  client.println("Host: " + String(bridge_ip) + ":" + String(port));
  client.println("User-Agent: ESP8266/1.0");
  client.println("Connection: close");
  client.println("Content-type: text/xml; charset=\"utf-8\"");
  client.print("Content-Length: ");
  client.println(command.length());
  client.println();
  client.println(command);
  
  delay(10);
  
  // read all lines of the reply from server
  while(client.available()){
    String line = client.readStringUntil('\r');
    //Serial.print(line);
  }
}

// connecting to wifi
void WIFI_Connect()
{
  WiFi.disconnect();
  Serial.println("Booting Sketch...");
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
    // Wait for connection
  for (int i = 0; i < 25; i++)
  {
    if ( WiFi.status() != WL_CONNECTED ) {
      delay ( 250 );
      Serial.print ( "." );
      delay ( 250 );
    }
  }
}

void setup() {
  //set serial port
  Serial.begin(115200);
  Serial.println("Booting");

  WIFI_Connect();
    
  delay(1000);
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Set up the button pin for input, no pullup for shelly
  pinMode( button, INPUT );

  // when using GPIO 2 , GPIO 0 and 2 must be high for booting, pulldown od witch is done by GPIO 0
  //pinMode( 0, OUTPUT );
  //digitalWrite(0, LOW);

 // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("Shelly1-001");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}

void loop() {
  ArduinoOTA.handle();
  hue_control(hue_toggle);
  delay (100);
  while(digitalRead(button) == HIGH) {
    delay (10);  
  }
  hue_control(hue_toggle);
  while(digitalRead(button) == LOW) {
    delay (10);
    }
  delay (100);
}
