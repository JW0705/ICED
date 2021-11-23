#include <Servo.h>

// Arduino pin assignment
#define PIN_IR A0
#define PIN_LED 9
#define PIN_SERVO 10

// configurable parameters
#define _DUTY_MIN 553 // servo full clockwise position (0 degree)
#define _DUTY_NEU 1476 // servo neutral position (90 degree)
#define _DUTY_MAX 2399 // servo full counterclockwise position (180 degree)

#define _POS_START (_DUTY_MIN + 100)
#define _POS_END (_DUTY_MAX - 100)

#define _SERVO_SPEED 60 // servo speed limit (unit: degree/second)
#define SND_VEL 346.0 // sound velocity at 24 celsius degree (unit: m/s)
#define INTERVAL 25 // sampling interval (unit: ms)
#define _DIST_MIN 180 // minimum distance to be measured (unit: mm)
#define _DIST_MAX 360 // maximum distance to be measured (unit: mm)
#define Alpha 0.5 // ema의 기준 값


// global variables
unsigned long last_sampling_time; // unit: ms
int duty_chg_per_interval; // maximum duty difference per interval
int toggle_interval, toggle_interval_cnt;
float pause_time; // unit: sec
Servo myservo;

float timeout; // unit: us
float dist_min, dist_max, dist_raw, dist_prev, dist_ema, last_ema, dist_target; // unit: mm
float alpha; // ema의 기준을 정하는 데 사용
float _DUTY_ANGLE; // 거리에 비례하여 변하는 서보모터의 각도를 위한 값
int duty_target, duty_curr;
float scale; // used for pulse duration to distance conversion

void setup() {
// initialize GPIO pins
  pinMode(PIN_LED,OUTPUT);
  digitalWrite(PIN_LED, 1);
  
  myservo.attach(PIN_SERVO); 
  myservo.writeMicroseconds(_DUTY_NEU);
 
// initialize serial port
  Serial.begin(57600);

// convert angle speed into duty change per interval.
  duty_chg_per_interval = (float)(_DUTY_MAX - _DUTY_MIN) * _SERVO_SPEED / 180 * INTERVAL / 1000;

// remove next three lines after finding answers
//  Serial.print("duty_chg_per_interval:");
//  Serial.println(duty_chg_per_interval);
//  while(1) {}

// initialize USS related variables
  dist_min = _DIST_MIN; 
  dist_max = _DIST_MAX;
  dist_ema = 0;
  timeout = (INTERVAL / 2) * 1000.0; // precalculate pulseIn() timeout value. (unit: us)
  dist_raw = dist_prev = 0.0; // raw distance output from USS (unit: mm)
  scale = 0.001 * 0.5 * SND_VEL;
  alpha = Alpha;
  dist_target = 0.0;
// initialize last sampling time
  last_sampling_time = 0;
}

float ir_distance(void){ // return value unit: mm
  float val;
  float volt = float(analogRead(PIN_IR));
  val = ((6762.0/(volt-9.0))-4.0) * 10.0;
  return val;
}

void loop() {
  dist_raw = ir_distance();
  
  float dist_cali = 100+300/(350-85) * ( dist_raw - 85);
  Serial.print("min:0,max:500,dist:");
  Serial.println(dist_cali);
  
  dist_ema = alpha * dist_cali + (1 - alpha)* last_ema;
  last_ema = dist_ema;
  
  if(dist_cali >= 200.0){
    
    myservo.writeMicroseconds(1400);
    delay(100);
  }
  else{
    
    myservo.writeMicroseconds(1800);
    delay(100);
  }
 
}
