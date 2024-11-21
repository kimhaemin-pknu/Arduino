#include <Wire.h>
#include <RTClib.h>

const int buttonPin1 = 13;  // 첫 번째 버튼 GPIO 핀
const int buttonPin2 = 12;  // 두 번째 버튼 GPIO 핀
const int ledPin = 14;      // 알람 시 켜질 LED 핀
const int ledPin1 = 15;     // 첫 번째 버튼의 LED 핀
const int ledPin2 = 16;     // 두 번째 버튼의 LED 핀

RTC_DS3231 rtc;

int lastButtonState1 = HIGH;
int lastButtonState2 = HIGH;
unsigned long buttonPressTime1 = 0;
unsigned long buttonPressTime2 = 0;
int alarmTime = 0;            // 알람 주기 시간 (단위: 초)
bool alarmSet = false;        // 알람이 설정되었는지 여부
bool ledOn = false;           // LED가 켜져 있는지 여부

void setup() {
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  Serial.begin(115200);

  // RTC 초기화
  if (!rtc.begin()) {
    Serial.println("RTC를 찾을 수 없습니다!");
    while (1); // 무한 대기
  }

  // 전원이 꺼졌다면 RTC 초기화
  if (rtc.lostPower()) {
    Serial.println("RTC 전원이 꺼져있었습니다. 시간을 초기화합니다.");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // 현재 컴파일 시간으로 설정
  }
}

void loop() {
  int buttonState1 = digitalRead(buttonPin1);
  int buttonState2 = digitalRead(buttonPin2);

  // LED가 켜져 있는 상태에서 1번 버튼을 눌렀을 경우 LED 끄기
  if (ledOn && buttonState1 == LOW && lastButtonState1 == HIGH) {
    digitalWrite(ledPin, LOW);
    ledOn = false;  // LED 상태 업데이트
    Serial.println("LED가 꺼졌습니다.");
  }

  // 1번 버튼 짧은/긴 누름 감지
  if (buttonState1 == LOW) {
    if (lastButtonState1 == HIGH) {
      buttonPressTime1 = millis();
    } else if (millis() - buttonPressTime1 > 2000) {  // 2초 이상 눌렀을 때
      if (!alarmSet) {
        Serial.println("카운트다운 시작");
        alarmSet = true;
        setRTCAlarm(); // RTC 알람 설정

        // 첫 번째 LED 두 번 깜빡임
        for (int i = 0; i < 2; i++) {
          digitalWrite(ledPin1, HIGH);
          delay(200);
          digitalWrite(ledPin1, LOW);
          delay(200);
        }
      }
    }
  } else if (lastButtonState1 == LOW) {
    if (millis() - buttonPressTime1 <= 2000) {  // 2초 이하 눌렀을 때
      // 알람 주기 30초 증가
      if (!ledOn) { // 알람 LED가 켜져 있을 때 30초 추가 안됨
        alarmTime += 30;
        Serial.print("30초 증가: ");
        Serial.print(alarmTime);
        Serial.println("초");

        // 첫 번째 버튼 LED 한 번 깜빡임
        digitalWrite(ledPin1, HIGH);
        delay(500);
        digitalWrite(ledPin1, LOW);
      }
    }
  }

  // 2번 버튼 짧은/긴 누름 감지
  if (buttonState2 == LOW) {
    if (lastButtonState2 == HIGH) {
      buttonPressTime2 = millis();
    } else if (millis() - buttonPressTime2 > 2000) {  // 2초 이상 눌렀을 때
      // 모든 알람 초기화
      alarmTime = 0;
      alarmSet = false;

      // RTC의 모든 알람 플래그 초기화
      rtc.clearAlarm(1);
      rtc.clearAlarm(2);
      rtc.writeSqwPinMode(DS3231_OFF); // 알람 발생 비활성화
      Serial.println("모든 알람이 삭제되었습니다.");
      digitalWrite(ledPin, LOW);
      ledOn = false;

      // 두 번째 LED 두 번 깜빡임
      for (int i = 0; i < 2; i++) {
        digitalWrite(ledPin2, HIGH);
        delay(200);
        digitalWrite(ledPin2, LOW);
        delay(200);
      }
    }
  } else if (lastButtonState2 == LOW) {
    if (millis() - buttonPressTime2 <= 2000) {  // 2초 이하 눌렀을 때
      // 알람 주기 30초 감소
      alarmTime -= 30;
      if (alarmTime < 0) alarmTime = 0;
      Serial.print("30초 감소: ");
      Serial.print(alarmTime);
      Serial.println("초");

      // 두 번째 버튼 LED 한 번 깜빡임
      digitalWrite(ledPin2, HIGH);
      delay(500);
      digitalWrite(ledPin2, LOW);
    }
  }

  // 알람 확인
  if (rtc.alarmFired(1)) { // RTC에서 알람 발생 여부 확인
    if (alarmSet) { // 알람이 설정되어 있을 때만 실행
      Serial.println("알람 발생!");
      digitalWrite(ledPin, HIGH);  // 알람 LED 켜기
      ledOn = true;               // LED 상태 업데이트
      rtc.clearAlarm(1);           // 알람 해제
      setRTCAlarm();               // 다음 알람 설정
    }
  }

  // 버튼 상태 저장
  lastButtonState1 = buttonState1;
  lastButtonState2 = buttonState2;

  delay(10); // 짧은 딜레이
}

void setRTCAlarm() {
  // 현재 시간 가져오기
  DateTime now = rtc.now();
  int alarmSeconds = now.second() + alarmTime; // 현재 시간 기준으로 설정된 알람 시간 추가

  int alarmMinutes = now.minute();
  int alarmHours = now.hour();

  if (alarmSeconds >= 60) {
    alarmSeconds -= 60;
    alarmMinutes++;
  }
  if (alarmMinutes >= 60) {
    alarmMinutes -= 60;
    alarmHours++;
  }

  // RTC 알람 설정
  rtc.clearAlarm(1); // 기존 알람 제거
  rtc.setAlarm1(
    DateTime(now.year(), now.month(), now.day(), alarmHours, alarmMinutes, alarmSeconds),
    DS3231_A1_Second
  );
  Serial.println("RTC 알람이 설정되었습니다.");
}
