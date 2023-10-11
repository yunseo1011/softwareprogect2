// 아두이노 핀 할당
#define PIN_LED  9
#define PIN_TRIG 12   // 초음파 센서 트리거 핀
#define PIN_ECHO 13   // 초음파 센서 에코 핀

// 설정 가능한 매개변수
#define SND_VEL 346.0     // 24도에서의 음속 (단위: m/초)
#define INTERVAL 25       // 샘플링 간격 (단위: 밀리초)
#define PULSE_DURATION 10 // 초음파 펄스 지속 시간 (단위: 마이크로초)
#define _DIST_MIN 100.0   // 측정할 최소 거리 (단위: mm)
#define _DIST_MAX 300.0   // 측정할 최대 거리 (단위: mm)
#define LED_MAX_DISTANCE 200.0  // 최대 거리에 대응하는 LED 밝기 (0% duty cycle)
#define LED_MIN_DISTANCE 100.0  // 최소 거리에 대응하는 LED 밝기 (50% duty cycle)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // 최대 에코 대기 시간 (단위: 마이크로초)
#define SCALE (0.001 * 0.5 * SND_VEL) // 지속 시간을 거리로 변환하는 계수

unsigned long last_sampling_time;   // 단위: 밀리초

void setup() {
  // GPIO 핀 초기화
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);  // 초음파 트리거
  pinMode(PIN_ECHO, INPUT);   // 초음파 에코
  digitalWrite(PIN_TRIG, LOW);  // 초음파 끄기 
  
  // 시리얼 포트 초기화
  Serial.begin(57600);
  last_sampling_time = millis();  // 마지막 샘플링 시간 초기화
}

void loop() {
  float distance;
  int led_brightness;

  // 다음 샘플링 시간이 되기를 기다림
  if (millis() - last_sampling_time >= INTERVAL) {
    distance = USS_measure(PIN_TRIG, PIN_ECHO); // 거리 측정

    // LED 밝기 계산
    if (distance <= LED_MAX_DISTANCE && distance >= LED_MIN_DISTANCE) {
      // 거리에 따라 LED 밝기 계산 (LED_MAX_DISTANCE에서 최대, LED_MIN_DISTANCE에서 최소, 중간에서 50% duty cycle)
      led_brightness = map(distance, LED_MAX_DISTANCE, LED_MIN_DISTANCE, 0, 255);
      analogWrite(PIN_LED, led_brightness);
    } else if (distance > LED_MAX_DISTANCE) {
      analogWrite(PIN_LED, 0);  // 거리가 최대일 때 LED 꺼짐
    } else {
      analogWrite(PIN_LED, 255);  // 거리가 최소일 때 LED 가장 밝음
    }

    // 마지막 샘플링 시간 업데이트
    last_sampling_time = millis();
  }
}

// 초음파 센서로부터 거리를 측정하여 반환 (단위: 밀리미터)
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // 단위: mm
}
