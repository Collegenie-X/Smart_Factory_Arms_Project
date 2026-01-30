/*
 * ========================================
 * delay (동기) vs millis (비동기) 비교 예제
 * ========================================
 * 
 * 기능:
 * - delay() 방식과 millis() 방식의 차이 비교
 * - LED 3개를 서로 다른 주기로 깜빡임
 * - 버튼 입력 반응 속도 비교
 * - Serial 명령으로 모드 전환
 * 
 * 하드웨어:
 * - LED 3개 (서로 다른 깜빡임 주기)
 * - 버튼 1개 (반응 속도 테스트)
 * 
 * 작성일: 2026-01-30
 */

// ========================================
// 핀 정의
// ========================================
#define LED1_PIN      3     // LED 1 (500ms 주기)
#define LED2_PIN      4     // LED 2 (1000ms 주기)
#define LED3_PIN      5     // LED 3 (1500ms 주기)
#define BUTTON_PIN    2     // 버튼 입력

// ========================================
// 상수 정의
// ========================================
#define LED1_INTERVAL   500   // LED 1 깜빡임 주기 (ms)
#define LED2_INTERVAL   1000  // LED 2 깜빡임 주기 (ms)
#define LED3_INTERVAL   1500  // LED 3 깜빡임 주기 (ms)

// ========================================
// 전역 변수
// ========================================
// 모드 선택 (true: millis 방식, false: delay 방식)
bool useMillisMode = true;

// millis() 방식용 변수
unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
unsigned long previousMillis3 = 0;

bool led1State = false;
bool led2State = false;
bool led3State = false;

// 버튼 카운터
int buttonPressCount = 0;
int lastButtonState = HIGH;

// ========================================
// 초기화 함수
// ========================================
void setup() {
  Serial.begin(9600);
  Serial.println("========================================");
  Serial.println("  delay vs millis 비교 시스템");
  Serial.println("========================================");
  
  // 핀 모드 설정
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // 초기 상태
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  digitalWrite(LED3_PIN, LOW);
  
  Serial.println("[초기화 완료]");
  Serial.println("\n명령어:");
  Serial.println("  millis - millis() 방식 (비동기, 권장)");
  Serial.println("  delay  - delay() 방식 (동기, 비교용)");
  Serial.println("\n현재 모드: millis() 방식");
  Serial.println("\n[테스트 방법]");
  Serial.println("1. LED 3개가 서로 다른 주기로 깜빡임");
  Serial.println("2. 버튼을 눌러서 반응 속도 확인");
  Serial.println("3. delay 모드에서는 버튼 반응이 느림!");
  Serial.println("========================================");
  Serial.println();
}

// ========================================
// 메인 루프
// ========================================
void loop() {
  // Serial 명령 처리
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    handleCommand(command);
  }
  
  // 버튼 입력 처리 (항상 체크)
  handleButton();
  
  // 선택된 모드에 따라 실행
  if (useMillisMode) {
    runMillisMode();
  } else {
    runDelayMode();
  }
}

// ========================================
// 명령 처리 함수
// ========================================
void handleCommand(String cmd) {
  Serial.print("[명령 수신] ");
  Serial.println(cmd);
  
  if (cmd == "millis") {
    useMillisMode = true;
    Serial.println("========================================");
    Serial.println("[모드 변경] millis() 방식 (비동기)");
    Serial.println("→ 버튼 입력이 즉시 반응합니다");
    Serial.println("→ 여러 작업을 동시에 처리 가능");
    Serial.println("========================================");
    
  } else if (cmd == "delay") {
    useMillisMode = false;
    Serial.println("========================================");
    Serial.println("[모드 변경] delay() 방식 (동기)");
    Serial.println("→ 버튼 입력이 느리게 반응합니다");
    Serial.println("→ delay 중에는 다른 작업 불가");
    Serial.println("========================================");
    
  } else {
    Serial.println("[에러] 알 수 없는 명령어");
  }
}

// ========================================
// millis() 방식 (비동기, 권장)
// ========================================
void runMillisMode() {
  unsigned long currentMillis = millis();
  
  // LED 1 제어 (500ms 주기)
  if (currentMillis - previousMillis1 >= LED1_INTERVAL) {
    previousMillis1 = currentMillis;
    led1State = !led1State;
    digitalWrite(LED1_PIN, led1State);
  }
  
  // LED 2 제어 (1000ms 주기)
  if (currentMillis - previousMillis2 >= LED2_INTERVAL) {
    previousMillis2 = currentMillis;
    led2State = !led2State;
    digitalWrite(LED2_PIN, led2State);
  }
  
  // LED 3 제어 (1500ms 주기)
  if (currentMillis - previousMillis3 >= LED3_INTERVAL) {
    previousMillis3 = currentMillis;
    led3State = !led3State;
    digitalWrite(LED3_PIN, led3State);
  }
  
  // 버튼은 loop()에서 항상 체크되므로 즉시 반응!
}

// ========================================
// delay() 방식 (동기, 비교용)
// ========================================
void runDelayMode() {
  // LED 1 켜기
  digitalWrite(LED1_PIN, HIGH);
  delay(LED1_INTERVAL);  // ← 이 시간 동안 버튼 입력 무시!
  digitalWrite(LED1_PIN, LOW);
  delay(LED1_INTERVAL);
  
  // LED 2 켜기
  digitalWrite(LED2_PIN, HIGH);
  delay(LED2_INTERVAL);  // ← 이 시간 동안 버튼 입력 무시!
  digitalWrite(LED2_PIN, LOW);
  delay(LED2_INTERVAL);
  
  // LED 3 켜기
  digitalWrite(LED3_PIN, HIGH);
  delay(LED3_INTERVAL);  // ← 이 시간 동안 버튼 입력 무시!
  digitalWrite(LED3_PIN, LOW);
  delay(LED3_INTERVAL);
  
  // 문제점: delay 중에는 버튼 입력을 체크할 수 없음!
  // → 버튼 반응이 매우 느림
}

// ========================================
// 버튼 입력 처리 함수
// ========================================
void handleButton() {
  int buttonState = digitalRead(BUTTON_PIN);
  
  // 버튼이 눌렸을 때 (HIGH → LOW)
  if (lastButtonState == HIGH && buttonState == LOW) {
    buttonPressCount++;
    
    Serial.println("========================================");
    Serial.print("[버튼 입력 감지] 누른 횟수: ");
    Serial.println(buttonPressCount);
    
    if (useMillisMode) {
      Serial.println("→ millis 모드: 즉시 반응!");
    } else {
      Serial.println("→ delay 모드: 느린 반응 (delay 중 무시됨)");
    }
    Serial.println("========================================");
    
    // 버튼 누를 때마다 모든 LED 한 번 깜빡임
    blinkAllLEDs();
  }
  
  lastButtonState = buttonState;
}

// ========================================
// 모든 LED 깜빡임 함수
// ========================================
void blinkAllLEDs() {
  // 모든 LED 켜기
  digitalWrite(LED1_PIN, HIGH);
  digitalWrite(LED2_PIN, HIGH);
  digitalWrite(LED3_PIN, HIGH);
  delay(200);
  
  // 모든 LED 끄기
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  digitalWrite(LED3_PIN, LOW);
  delay(200);
}

// ========================================
// 성능 비교 함수 (선택 사항)
// ========================================
void comparePerformance() {
  Serial.println("\n========================================");
  Serial.println("  성능 비교 결과");
  Serial.println("========================================");
  
  Serial.println("\n[delay() 방식의 문제점]");
  Serial.println("1. 동기 처리: delay 중에는 아무것도 할 수 없음");
  Serial.println("2. 버튼 입력 무시: 반응 속도 느림");
  Serial.println("3. 여러 작업 동시 처리 불가");
  Serial.println("4. 코드 확장성 낮음");
  
  Serial.println("\n[millis() 방식의 장점]");
  Serial.println("1. 비동기 처리: 여러 작업을 동시에 처리");
  Serial.println("2. 즉시 반응: 버튼 입력을 놓치지 않음");
  Serial.println("3. 정밀한 타이밍 제어 가능");
  Serial.println("4. 코드 확장성 높음 (권장!)");
  
  Serial.println("\n========================================");
}
