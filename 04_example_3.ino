#define PIN_LED 13
unsigned int count, toggle;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200);
  
  Serial.println("Hello World!");
  count = toggle = 0;
  digitalWrite(PIN_LED, toggle); //LED 끄기
}

void loop() {
  Serial.println(++count);
  toggle = toggle_state(toggle); //toggle LED값
  digitalWrite(PIN_LED, toggle); //LED 상태 업데이트
  delay(1000);
}

int toggle_state(int toggle){
  return toggle;
}
