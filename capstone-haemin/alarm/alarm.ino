const int buttonPin1 = 13;  // 첫 번째 버튼 GPIO 핀
const int buttonPin2 = 12;  // 두 번째 버튼 GPIO 핀
const int ledPin = 14;      // 알람 시 켜질 LED 핀
const int ledPin1 = 15;     // 첫 번째 버튼의 LED 핀
const int ledPin2 = 16;     // 두 번째 버튼의 LED 핀

int lastButtonState1 = HIGH;
int lastButtonState2 = HIGH;
unsigned long buttonPressTime1 = 0;
unsigned long buttonPressTime2 = 0;
int alarmTime = 0;            // 알람 주기 시간 (단위: 초)
unsigned long lastAlarmTime = 0;
bool alarmSet = false;        // 알람이 설정되었는지 여부
bool ledActive = false;       // LED가 켜져 있는지 여부

void setup() {
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  int buttonState1 = digitalRead(buttonPin1);
  int buttonState2 = digitalRead(buttonPin2);

  // 첫 번째 버튼 짧은/긴 누름 감지
  if (buttonState1 == LOW) {
    if (lastButtonState1 == HIGH) {
      buttonPressTime1 = millis();
    } else if (millis() - buttonPressTime1 > 2000) {
      if (!alarmSet) {
        Serial.println("카운트다운 시작");
        lastAlarmTime = millis();
        alarmSet = true;

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
    if (millis() - buttonPressTime1 <= 2000) {
      if (ledActive) {
        // LED를 끄기만 함
        Serial.println("LED 끄기");
        digitalWrite(ledPin, LOW);
        ledActive = false;
      } else {
        // 알람 주기 30초 증가
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

  // 두 번째 버튼 짧은/긴 누름 감지
  if (buttonState2 == LOW) {
    if (lastButtonState2 == HIGH) {
      buttonPressTime2 = millis();
    } else if (millis() - buttonPressTime2 > 2000) {
      // 알람 초기화
      alarmTime = 0;
      alarmSet = false;
      ledActive = false;
      Serial.println("알람 초기화");
      digitalWrite(ledPin, LOW);

      // 두 번째 LED 두 번 깜빡임
      for (int i = 0; i < 2; i++) {
        digitalWrite(ledPin2, HIGH);
        delay(200);
        digitalWrite(ledPin2, LOW);
        delay(200);
      }
    }
  } else if (lastButtonState2 == LOW) {
    if (millis() - buttonPressTime2 <= 2000) {
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

  // 알람 주기적 울림
  if (alarmSet && (millis() - lastAlarmTime >= alarmTime * 1000)) {
    Serial.println("알람 시간 도달!");
    digitalWrite(ledPin, HIGH);  // LED 켜기
    ledActive = true;            // LED 활성화
    lastAlarmTime = millis();    // 마지막 알람 시간을 현재 시간으로 업데이트
  }

  // 현재 버튼 상태를 이전 상태 변수에 저장
  lastButtonState1 = buttonState1;
  lastButtonState2 = buttonState2;

  delay(10);
} 
