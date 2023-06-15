
#include "BluetoothSerial.h"

#define LED_BUILTIN 23

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;


void setup() {
        Serial.begin(9600);
        SerialBT.begin("ESP32receiver"); //Bluetooth device name // <------- set this to be the same as the name you chose above!!!!! 
        
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(1000);
        digitalWrite(LED_BUILTIN, LOW);
}

int blinktime = 0;
int willblink = 0;

void loop() {
  if(millis() > blinktime + 200 && willblink == 1){
    digitalWrite(23, LOW);
    willblink = 0;
  }
  /*
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }*/
  if (SerialBT.available()) {
    while (SerialBT.available()){
        char get_char = SerialBT.read();
        char str[4] = "[ ]";
        str[1] = get_char;
        Serial.println(str);
    }
    willblink = 1;
    blinktime = millis();
    digitalWrite(23, HIGH);
  }
  delay(20);
}
