#include <WiFi.h>
#include <esp32cam.h>
#include <Arduino.h>


#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "BluetoothSerial.h"

#include "esp_camera.h"


#include "FS.h"
#include "driver/sdmmc_types.h"
#include "SD_MMC.h"
 

//const char* WIFI_SSID = "gamma";
const char* WIFI_SSID = "newesp32cam";
//const char* WIFI_PASS = "8b5bjz4zns";
const char* WIFI_PASSWD = NULL; 

WiFiServer server(80);
 
//internal stuff
#define LED_BUILTIN 14

//bluetooth stuff
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

//bluetooth stuff
BluetoothSerial SerialBT;

void printDeviceAddress() {

  const uint8_t* point = esp_bt_dev_get_address();
  for (int i = 0; i < 6; i++) {
    char str[3];
    sprintf(str, "%02X", (int)point[i]);
    Serial.print(str);
    if (i < 5){
      Serial.print(":");
    }
  }
}

//camera stuff
static auto loRes = esp32cam::Resolution::find(320, 240);
static auto midRes = esp32cam::Resolution::find(350, 530);
static auto hiRes = esp32cam::Resolution::find(800, 600);

//speed calculation

int speed = 10;
/*
void serveJpg()
{
  if (!esp32cam::Camera.changeResolution(loRes)) {
    Serial.println("SET-LO-RES FAIL");
  }
  auto frame = esp32cam::capture();
  if (frame == nullptr) {
    
    Serial.println("CAPTURE FAIL");
    server.send(503, "", "");
    
    return;
  }
  Serial.printf("CAPTURE OK %dx%d %db\n", frame->getWidth(), frame->getHeight(),         static_cast<int>(frame->size()));
 
  server.setContentLength(frame->size());
  server.send(200, "image/jpeg");
  WiFiClient client = server.client();
  frame->writeTo(client);
}


void handleSpeed()
{
  
  server.setContentLength(4);
  server.send(200, "image/jpeg");
  WiFiClient client = server.client();
  client.print(speed);
}

*/

void setup(){
  Serial.begin(115200);

  //gpio begin
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.println();
  {
    using namespace esp32cam;
    Config cfg;
    cfg.setPins(pins::AiThinker);
    cfg.setResolution(hiRes);
    cfg.setBufferCount(2);
    cfg.setJpeg(80);  
 
    bool ok = Camera.begin(cfg);
    Serial.println(ok ? "CAMERA OK" : "CAMERA FAIL");
  }
  /*
  WiFi.persistent(false);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }*/
  
  WiFi.softAP(WIFI_SSID, NULL);

  Serial.print("http://");
  Serial.println(WiFi.localIP());
  Serial.println("  /cam-lo.jpg");
  Serial.println("  /cam-hi.jpg");
  Serial.println("  /cam-mid.jpg");
 
 
  server.begin();

  //bluetooth begin
  SerialBT.begin("ESP32camrobot"); //Bluetooth device name

  //gpio begin
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);

}
 
int blinktime = 0;
int blinkstate = 0;

int lastcheck = 0;



void loop()
{
  if(millis() - blinktime > 500 && blinkstate == 1){
    digitalWrite(LED_BUILTIN, LOW);
    blinkstate = 0;
  }

  WiFiClient client = server.available();

  if (client) { 
    digitalWrite(LED_BUILTIN, HIGH);
    blinktime = millis();
    blinkstate = 1;
    lastcheck = blinktime;
    //1 = camera, alphabet = command
    int video_state = 0;
                                // if you get a client,
    //Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:

            if(video_state){
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:image/jpeg");
              client.println();

              if (!esp32cam::Camera.changeResolution(loRes)) {
                Serial.println("SET-LO-RES FAIL");
              }
              auto frame = esp32cam::capture();
              if (frame == nullptr) {
                client.println();
    
                return;
              }
              Serial.printf("CAPTURE OK %dx%d %db\n", frame->getWidth(), frame->getHeight(),         static_cast<int>(frame->size()));
 
              //server.setContentLength(frame->size());
              frame->writeTo(client);

              // The HTTP response ends with another blank line:
              client.println();
            }else{
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println();

              client.println();
            }
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /Q")) {
          Serial.println("[Q]");              // GET /H turns the LED on
        }else if (currentLine.endsWith("GET /W")) {
          Serial.println("[W]"); 
        }else if (currentLine.endsWith("GET /E")) {
          Serial.println("[E]");              // GET /H turns the LED on
        }else if (currentLine.endsWith("GET /A")) {
          Serial.println("[A]"); 
        }else if (currentLine.endsWith("GET /S")) {
          Serial.println("[S]");              // GET /H turns the LED on
        }else if (currentLine.endsWith("GET /D")) {
          Serial.println("[D]"); 
        }else if (currentLine.endsWith("GET /Z")) {
          Serial.println("[Z]");              // GET /H turns the LED on
        }else if (currentLine.endsWith("GET /X")) {
          Serial.println("[X]"); 
        }else if (currentLine.endsWith("GET /C")) {
          Serial.println("[C]");              // GET /H turns the LED on
        }else if (currentLine.endsWith("GET /V")) {
          Serial.println("[V]"); 
        }else if (currentLine.endsWith("GET /IMG")) {
            video_state = 1;
            /*
            if (!esp32cam::Camera.changeResolution(loRes)) {
              Serial.println("SET-LO-RES FAIL");
            }
            auto frame = esp32cam::capture();
            if (frame == nullptr) {
              
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:image/jpg");
              client.println();
              client.println();
    
              return;
            }
            Serial.printf("CAPTURE OK %dx%d %db\n", frame->getWidth(), frame->getHeight(),         static_cast<int>(frame->size()));
 
            //server.setContentLength(frame->size());
            frame->writeTo(client);

            // The HTTP response ends with another blank line:
            client.println();
            */

                             // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    //Serial.println("Client Disconnected.");
  }

  //  Serial.println(curDir);
  int available = SerialBT.available();
  if (millis() > lastcheck + 50 && available > 0) {
    for (int i = 0; i < available; i++){
      char get_char = SerialBT.read();
      if(get_char != '[' && get_char != ']' && get_char != ' ' && get_char != '\n'){
        char str[4] = "[ ]";
        str[1] = get_char;
        Serial.println(str);
      }
    }
    digitalWrite(LED_BUILTIN, HIGH);
    blinktime = millis();
    blinkstate = 1;
    lastcheck = blinktime;
  }
}
