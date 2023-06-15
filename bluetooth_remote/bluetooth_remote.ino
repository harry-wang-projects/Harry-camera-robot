//This example code is in the Public Domain (or CC0 licensed, at your option.)
//By Evandro Copercini - 2018
//
//This example creates a bridge between Serial and Classical Bluetooth (SPP)
//and also demonstrate that SerialBT have the same functionalities of a normal Serial

//Bluetooth stuff
#include "BluetoothSerial.h"
#include "esp32-hal-ledc.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

uint8_t address[6]  = {0xA8, 0x42, 0xE3, 0x4B, 0x79, 0x46};

//Gyro stuff
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

//for bluetooth remote
char angletokeys(float xtilt, float ytilt)
{
  if (xtilt < -15){
    if (ytilt < -15) {
      return 'Q';
    }else if (ytilt > 15) {
      return 'C';
    } else {
      return 'A';
    }
  }else if (xtilt > 15){
    if (ytilt < -15) {
      return 'E';
    }else if (ytilt > 15) {
      return 'Z';
    } else {
      return 'D';
    }
  }else{
    if (ytilt < -15) {
      return 'W';
    }else if (ytilt > 15) {
      return 'X';
    } else {
      return 'S';
    }
  }
}

void mpu6050_init()
{
  /*
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");
  */

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  //Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  //Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    //Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    //Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    //Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    //Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  //Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    //Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    //Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    //Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    //Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  //Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    //Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    //Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    //Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    //Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    //Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    //Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    //Serial.println("5 Hz");
    break;
  }

  //Serial.println("");
  delay(100);        
}

void mpu6050_print()
{
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

   
  float roll = (atan(a.acceleration.y / sqrt(pow(a.acceleration.x, 2) + pow(a.acceleration.z, 2))) * 180 / PI) ;

  float pitch = (atan(-1 * a.acceleration.x / sqrt(pow(a.acceleration.y, 2) + pow(a.acceleration.z, 2))) * 180 / PI); 


  /* Print out the values */
  
  SerialBT.print(" ");
  SerialBT.print(a.acceleration.x);
  SerialBT.print(" ");
  SerialBT.print(a.acceleration.y);
  SerialBT.print(" ");
  SerialBT.print(a.acceleration.z);

  SerialBT.print(" ");
  SerialBT.print(roll);
  SerialBT.print(" ");
  SerialBT.print(pitch);
  SerialBT.print(" ");
  SerialBT.print(digitalRead(19));

  SerialBT.println(angletokeys(roll, pitch));
}

char mpu6050_getchar()
{
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

   
  float roll = (atan(a.acceleration.y / sqrt(pow(a.acceleration.x, 2) + pow(a.acceleration.z, 2))) * 180 / PI) ;

  float pitch = (atan(-1 * a.acceleration.x / sqrt(pow(a.acceleration.y, 2) + pow(a.acceleration.z, 2))) * 180 / PI); 

  return angletokeys(pitch, roll);
}

void setup() {
  //bluetooth
  pinMode(18, OUTPUT);
  digitalWrite(18, HIGH);
  delay(2000);
  digitalWrite(18, LOW);
  //Serial.begin(9600);
  SerialBT.begin("ESP32controller", true); //Bluetooth device name
  //Serial.println("The device started, now you can pair it with bluetooth!");

  
  bool connected = SerialBT.connect(address);
  
  if(connected) {
  //  Serial.println("Connected Succesfully!");
  } else {
    while(!SerialBT.connected(10000)) {
  //    Serial.println("Failed to connect. Make sure remote device is available and in range, then restart app."); 
    }
  }
  
  SerialBT.println("[S]");
  
  digitalWrite(18, HIGH);
  delay(500);
  digitalWrite(18, LOW);
  
  mpu6050_init();

  pinMode(19, INPUT);
}

int prev_read = 1;
char last_char = 'S';

int blinktime = 0;
int blinkstate = 0;

void loop() {
  if(blinkstate == 1 && millis() > blinktime + 500){
    blinkstate = 0;
    digitalWrite(18, LOW);
  }

  int button_press = 0;
  if(digitalRead(19) == 0 && prev_read == 1){
    SerialBT.println("[V]");
    prev_read = 0;
    return;
  }
  prev_read = digitalRead(19);
  
  char current_reading = mpu6050_getchar();
  if(current_reading != last_char){
    char printstr[4] = "[ ]";
    printstr[1] = current_reading;
    SerialBT.println(printstr);
    last_char = current_reading;
    digitalWrite(18, HIGH);
    blinktime = millis();
    blinkstate = 1;
  }
  /*
  if (Serial.available()) {
    digitalWrite(18, HIGH);
    Serial.write(Serial.read());
    SerialBT.write(Serial.read());
  }*/
  if (SerialBT.available()) {
    int get_size = SerialBT.read();
    get_size -= '0';
    ledcWrite(1, get_size * 770 + 1638);
    
    digitalWrite(18, HIGH);
  }
  
  delay(25);
}
