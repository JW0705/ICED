#define PIN_LED 13
bool toggle = false;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200);
  
  digitalWrite(PIN_LED, toggle); //LED 끄기
}

void loop() {
  toggle = !toggle; //toggle LED값
  digitalWrite(PIN_LED, toggle); //LED 상태 업데이트
  delay(1000);
}

