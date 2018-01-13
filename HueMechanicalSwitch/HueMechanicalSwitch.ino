/*********
  HueZeptrion
*********/

#include <ESP8266WiFi.h>

const int button = 2;

// wifi settings

// HUE settings
const char* bridge_ip = "10.0.0.165";
const int port = 80;

// HUE commands
String hue_toggle="{\"state\":{\"flag\":true}}";

// SPI variables
uint8_t spiTXdata = 0;
uint8_t spiRXdata;
uint8_t spiRXold;
uint8_t spiTimer;
uint8_t spiScene = 0;
const int spiLong = 10;
uint8_t _ss_pin = SS;


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
  client.println("PUT /api/" + user + "/sensors/14");
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

  // Set up the button pin for input
  pinMode( button, INPUT_PULLUP );

  // since GPIO 0 and 2 must be high for booting, pulldown od witch is done by GPIO 0
  pinMode( 0, OUTPUT );
  digitalWrite(0, LOW);

 
}

void loop() {
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
