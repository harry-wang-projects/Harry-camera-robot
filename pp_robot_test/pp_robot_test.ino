/****************************************************************************8
   亚博智能科技有限公司
   程序：TB6612FNG测试程序
   程序直接烧录到平衡车上就可使用测试电机
*/
int STBY = 8; //使能端

//Motor A
int PWMA = 9; //左电机PWM输出控制脚
int AIN1 = 7; //左电机正极
int AIN2 = 6; //左电机负极

//Motor B
int PWMB = 10; //右电机PWM输出控制脚
int BIN1 = 13; //右电机正极
int BIN2 = 12; //右电机负极

int trig = 3;
int echo = 2;


void setup() {
  pinMode(STBY, OUTPUT);

  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  pinMode(11, OUTPUT);


  runset(1, 150, 0); //左电机全速向前转
  runset(2, 150, 0); //右电机全速向前转
}


void runset(int motor, int speed, int direction) {

  digitalWrite(STBY, HIGH); //使能驱动模块脚

  boolean Pin1 = LOW;
  boolean Pin2 = HIGH;

  if (direction == 1) {
    Pin1 = HIGH;
    Pin2 = LOW;
  }

  if (motor == 1) {
    digitalWrite(AIN1, Pin1);
    digitalWrite(AIN2, Pin2);
    analogWrite(PWMA, speed);
  } else {
    digitalWrite(BIN1, Pin1);
    digitalWrite(BIN2, Pin2);
    analogWrite(PWMB, speed);
  }
}

float get_distance()
{
  // Clears the trigPin condition
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  int duration = pulseIn(echo, HIGH);
  // Calculating the distance
  float distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
  return distance;
}

void loop() {
  delay(500);

  if(get_distance() <= 30){
    digitalWrite(11, HIGH);
    stop();
    delay(200);
    runset(1, 100, 1); //左电机全速向前转
    runset(2, 100, 1); //右电机全速向前转
    delay(1500);
    stop();
    digitalWrite(11, LOW);
    delay(200);
    runset(1, 100, 0); //左电机全速向前转
    runset(2, 100, 1); //右电机全速向前转
    delay(500);
    stop();
    delay(200);
    runset(1, 150, 0); //左电机全速向前转
    runset(2, 150, 0);
  }
}
  

void stop() {
  digitalWrite(STBY, LOW);
}
