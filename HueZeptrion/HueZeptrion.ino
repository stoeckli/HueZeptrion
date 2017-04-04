/*********
  HueZeptrion
*********/

#include <SPI.h>
#include <ESP8266WiFi.h>

// wifi settings
const char* ssid = "<your WLAN ID>";
const char* password = "<your WLAN password>";

// HUE settings
const char* bridge_ip = "<your HUB IP>";
const int port = 80;
String user="<your HUB user>";

// HUE commands
String hue_on="{\"on\":true}";
String hue_off="{\"on\":false}";
String hue_S1="{\"scene\":\"mVr0qQg4P7MbfrI\"}"; //change scene keys
String hue_S2="{\"scene\":\"nQ2GJoMnEWG-a2-\"}";
String hue_S3="{\"scene\":\"biLvQUOGVDF0uQ9\"}";
String hue_S4="{\"scene\":\"2QZHSvEXxdc8m1H\"}";
String hue_up="{\"bri_inc\":254,\"transitiontime\":50}";
String hue_down="{\"bri_inc\":-254,\"transitiontime\":50}";
String hue_stop="{\"bri_inc\":0}";

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
      digitalWrite(2,1);
      WIFI_Connect();
    } else {
      digitalWrite(2,0);
    }

  WiFiClient client;
  
  if (!client.connect(bridge_ip, port)) {
  Serial.println("Connection failed");
  return;
  }
  
  // building string
  client.println("PUT /api/" + user + "/groups/1/action");
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
  digitalWrite(2,1);
  WiFi.disconnect();
  Serial.println("Booting Sketch...");
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
    // Wait for connection
  for (int i = 0; i < 25; i++)
  {
    if ( WiFi.status() != WL_CONNECTED ) {
      delay ( 250 );
      digitalWrite(2,0);
      Serial.print ( "." );
      delay ( 250 );
      digitalWrite(2,1);
    }
  }
  digitalWrite(2,0);
}

void setup() {
  //set serial port
  Serial.begin(115200);
  Serial.println("Booting");

  pinMode(2, OUTPUT);
  WIFI_Connect();
  
  //start SPI
  pinMode(MOSI, OUTPUT);
  pinMode(_ss_pin, OUTPUT);
  digitalWrite(_ss_pin, HIGH);
  SPI.begin();
  SPI.setFrequency(10000);
  SPI.setDataMode(SPI_MODE3);
  
  delay(1000);
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {

  // initiate SPI read/write
  digitalWrite(_ss_pin, LOW);
  spiRXdata = SPI.transfer(spiTXdata);
  digitalWrite(_ss_pin, HIGH);
  delay(50);

  spiTimer ++;
  
  // check for pressed key
  switch (spiRXdata)
  {
    case 0xC0: //no key
      // this means the key was released
      // check which key was released
      switch (spiRXold)
      {
        case 0xC0: //no key
        break;

        case 0xC1: //+
          // short press of "+" switches light on
          if (spiTimer < spiLong)
          {
            hue_control(hue_S3);
            spiScene = 3;
            spiTXdata = 0x01;
          }
          else
          {
            hue_control(hue_stop);
          }
        break;
        case 0xC2: //-
          // short press of "-" switches light off
          if (spiTimer < spiLong)
            {
              if (spiScene == 0)
              {
                hue_control(hue_S4);
                spiScene = 4;
                spiTXdata = 0x01;                
              }
              else
              {
                hue_control(hue_off);
                spiTXdata = 0x00;
                spiScene = 0;
              }
            }
          else
          {
            hue_control(hue_stop);
          }
        break;
        
        case 0xC4: //S1
        break;
        
        case 0xC5: //S2  
        break;
        
        default:
        break;
      }
      
      // reset timer
      spiTimer = 0;
      break;
      
      case 0xC1: //+
          if (spiTimer == spiLong)
          {
            if (spiScene == 0)
            {
              hue_control(hue_S3);
              spiScene = 3;
              spiTXdata = 0x01;
            }
            hue_control(hue_up);
            spiTimer = spiLong + 1;
          }
      break;
      case 0xC2: //-
        if (spiTimer == spiLong)
          {
            if (spiScene == 0)
            {
              hue_control(hue_S4);
              spiScene = 4;
              spiTXdata = 0x01;
            }

            hue_control(hue_down);
            spiTimer = spiLong + 1;
          }
      break;
      case 0xC4: //S1
        hue_control(hue_S1);
        hue_control(hue_S1);
        spiScene = 1;

        spiTXdata = 0x05;
      break;
      case 0xC5: //S2
        hue_control(hue_S2);
        hue_control(hue_S2);
        spiScene = 2;
        spiTXdata = 0x06;
      break;
      default:
        spiTimer = 0;
      break;
    }    

  spiRXold = spiRXdata;
  if (spiTimer == 100)
  {
    spiTimer = 99;
  }
  
}
