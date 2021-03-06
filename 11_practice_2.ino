#include <Servo.h>

// Arduino pin assignment
#define PIN_LED 9
#define PIN_SERVO 10
#define PIN_TRIG 12
#define PIN_ECHO 13

// configurable parameters
#define SND_VEL 346.0 // sound velocity at 24 celsius degree (unit: m/s)
#define INTERVAL 25 // sampling interval (unit: ms)
#define _DIST_MIN 180 // minimum distance to be measured (unit: mm)
#define _DIST_MAX 360 // maximum distance to be measured (unit: mm)
#define Alpha 0.5 // ema의 기준 값

#define _DUTY_MIN 550 // servo full clockwise position (0 degree)
#define _DUTY_NEU 1475 // servo neutral position (90 degree)
#define _DUTY_MAX 2400 // servo full counterclockwise position (180 degree)

// global variables
float timeout; // unit: us
float dist_min, dist_max, dist_raw, dist_prev, dist_ema, last_ema; // unit: mm
float alpha; // ema의 기준을 정하는 데 사용
float _DUTY_ANGLE; // 거리에 비례하여 변하는 서보모터의 각도를 위한 값
unsigned long last_sampling_time; // unit: ms
float scale; // used for pulse duration to distance conversion
Servo myservo;

void setup() {
// initialize GPIO pins
  pinMode(PIN_LED,OUTPUT);
  pinMode(PIN_TRIG,OUTPUT);
  digitalWrite(PIN_TRIG, LOW); 
  pinMode(PIN_ECHO,INPUT);

  myservo.attach(PIN_SERVO); 
  myservo.writeMicroseconds(_DUTY_NEU);

// initialize USS related variables
  dist_min = _DIST_MIN; 
  dist_max = _DIST_MAX;
  dist_ema = 0;
  timeout = (INTERVAL / 2) * 1000.0; // precalculate pulseIn() timeout value. (unit: us)
  dist_raw = dist_prev = 0.0; // raw distance output from USS (unit: mm)
  scale = 0.001 * 0.5 * SND_VEL;
  alpha = Alpha;

// initialize serial port
  Serial.begin(57600);

// initialize last sampling time
  last_sampling_time = 0;
}

void loop() {
// wait until next sampling time. 
// millis() returns the number of milliseconds since the program started. Will overflow after 50 days.
  if(millis() < last_sampling_time + INTERVAL) return;

// get a distance reading from the USS
  dist_raw = USS_measure(PIN_TRIG,PIN_ECHO);
  dist_ema = alpha * dist_raw + (1 - alpha)* last_ema;
  last_ema = dist_ema;
// output the read value to the serial port
  Serial.print("Min:180, raw:");
  Serial.print(dist_raw);
  Serial.print(", ema:");
  Serial.print(dist_ema);
  Serial.print(", servo:");
  Serial.print(myservo.read());  
  Serial.println(", Max:360");

// adjust servo position according to the USS read value

  // add your code here!

if(dist_raw < dist_min) {
    pinMode(PIN_LED, LOW);
    myservo.writeMicroseconds(_DUTY_MIN); // 18cm 미만일 시 LED OFF and servo 0도
  }
  else if(dist_raw < dist_max){
    pinMode(PIN_LED, HIGH);
    _DUTY_ANGLE = map(dist_ema, dist_min, dist_max, _DUTY_MIN, _DUTY_MAX); // 거리 값의 변화에 따라 angle 값 변화
    myservo.writeMicroseconds(_DUTY_ANGLE);
  }
  else {
    pinMode(PIN_LED, LOW);
    myservo.writeMicroseconds(_DUTY_MAX); // 36cm 이상일 시 LED OFF and servo 180도
  }
   
// update last sampling time
  last_sampling_time += INTERVAL;
}


// 전자의 범위를 후자의 범위로 변경하여 변경된 범위에 맞게 val값을 변경하여 val 값에 저장한다.
double map(double val, double in_min, double in_max, int out_min, int out_max){
  return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// get a distance reading from USS. return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  float reading;
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  reading = pulseIn(ECHO, HIGH, timeout) * scale; // unit: mm
  if(reading < dist_min){
    reading = dist_min - 1; // 18cm 미만일 경우 17.9cm 고정
  }
  else if(reading > dist_max){
    reading = dist_max + 1; // 36cm 초과일 경우 36.1cm 고정
  }
  else dist_prev = reading;
  
  
  return reading;
}