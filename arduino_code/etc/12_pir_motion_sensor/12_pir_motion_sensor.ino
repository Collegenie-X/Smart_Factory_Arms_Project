/*
 * ========================================
 * PIR 인체 감지 센서 예제
 * ========================================
 * 
 * 기능:
 * - PIR 센서로 인체 동작 감지
 * - LED 자동 점등 (절전 모드)
 * - 감지 카운터 및 시간 기록
 * - Serial 모니터로 로그 출력
 * 
 * 하드웨어:
 * - PIR 센서 (HC-SR501)
 * - LED (자동 조명)
 * - 부저 (알림음)
 * 
 * 작성일: 2026-01-30
 */

// ========================================
// 핀 정의
// ========================================
#define PIR_PIN         2     // PIR 센서 디지털 입력
#define LED_PIN         3     // 자동 조명 LED
#define BUZZER_PIN      4     // 부저

// ========================================
// 상수 정의
// ========================================
#define LED_ON_DURATION   10000   // LED 켜짐 시간 (10초)
#define BUZZER_DURATION   200     // 부저 울림 시간 (ms)

// ========================================
// 전역 변수
// ========================================
bool motionDetected = false;
bool ledState = false;
unsigned long ledOnTime = 0;
int detectionCount = 0;

// ========================================
// 초기화 함수
// ========================================
void setup() {
  Serial.begin(9600);
  Serial.println("========================================");
  Serial.println("  PIR 인체 감지 센서 시스템");
  Serial.println("========================================");
  
  // 핀 모드 설정
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // 초기 상태
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  
  Serial.println("[초기화 완료]");
  Serial.println("\n⚠️ PIR 센서 안정화 중... (약 30초)");
  Serial.println("센서가 안정화될 때까지 움직이지 마세요.");
  Serial.println("========================================");
  
  // PIR 센서 안정화 대기 (30초)
  for (int i = 30; i > 0; i--) {
    Serial.print("남은 시간: ");
    Serial.print(i);
    Serial.println("초");
    delay(1000);
  }
  
  Serial.println("\n✅ 센서 준비 완료!");
  Serial.println("========================================");
  Serial.println();
}

// ========================================
// 메인 루프
// ========================================
void loop() {
  // 1. PIR 센서 읽기
  motionDetected = digitalRead(PIR_PIN);
  
  // 2. 동작 감지 처리
  if (motionDetected) {
    handleMotionDetected();
  }
  
  // 3. LED 자동 꺼짐 처리
  handleLEDAutoOff();
  
  delay(100);
}

// ========================================
// 동작 감지 처리 함수
// ========================================
void handleMotionDetected() {
  // LED가 이미 켜져 있으면 타이머만 갱신
  if (ledState) {
    ledOnTime = millis();
    return;
  }
  
  // LED 켜기
  ledState = true;
  digitalWrite(LED_PIN, HIGH);
  ledOnTime = millis();
  detectionCount++;
  
  // 알림음
  tone(BUZZER_PIN, 1500, BUZZER_DURATION);
  
  // 감지 로그
  printDetectionLog();
}

// ========================================
// LED 자동 꺼짐 처리 함수
// ========================================
void handleLEDAutoOff() {
  // LED가 꺼져 있으면 리턴
  if (!ledState) {
    return;
  }
  
  // 10초 경과 체크
  if (millis() - ledOnTime >= LED_ON_DURATION) {
    ledState = false;
    digitalWrite(LED_PIN, LOW);
    
    Serial.println("[자동 조명] LED 자동 꺼짐 (10초 경과)");
    Serial.println();
  }
}

// ========================================
// 감지 로그 출력 함수
// ========================================
void printDetectionLog() {
  Serial.println("========================================");
  Serial.print("[동작 감지 #");
  Serial.print(detectionCount);
  Serial.println("]");
  
  // 현재 시간 (millis를 초로 변환)
  unsigned long currentTime = millis() / 1000;
  Serial.print("시간: ");
  printFormattedTime(currentTime);
  
  Serial.println("상태: 자동 조명 켜짐 (10초간)");
  Serial.println("========================================");
  Serial.println();
}

// ========================================
// 시간 포맷 출력 함수
// ========================================
void printFormattedTime(unsigned long seconds) {
  int hours = seconds / 3600;
  int minutes = (seconds % 3600) / 60;
  int secs = seconds % 60;
  
  if (hours < 10) Serial.print("0");
  Serial.print(hours);
  Serial.print(":");
  
  if (minutes < 10) Serial.print("0");
  Serial.print(minutes);
  Serial.print(":");
  
  if (secs < 10) Serial.print("0");
  Serial.println(secs);
}

// ========================================
// 통계 출력 함수
// ========================================
void printStatistics() {
  Serial.println("\n========================================");
  Serial.println("  감지 통계");
  Serial.println("========================================");
  
  Serial.print("총 감지 횟수: ");
  Serial.println(detectionCount);
  
  Serial.print("가동 시간: ");
  printFormattedTime(millis() / 1000);
  
  Serial.println("========================================");
  Serial.println();
}

// ========================================
// 연속 감지 모니터링 함수
// ========================================
void monitorContinuousDetection(unsigned long duration) {
  Serial.print("[연속 감지 모니터링] ");
  Serial.print(duration / 1000);
  Serial.println("초간 측정...");
  
  int count = 0;
  unsigned long startTime = millis();
  bool lastState = false;
  
  while (millis() - startTime < duration) {
    bool currentState = digitalRead(PIR_PIN);
    
    // 상태 변화 감지 (LOW → HIGH)
    if (currentState && !lastState) {
      count++;
      Serial.print("[");
      Serial.print(count);
      Serial.print("] 동작 감지 - ");
      printFormattedTime((millis() - startTime) / 1000);
    }
    
    lastState = currentState;
    delay(100);
  }
  
  Serial.println("\n[결과]");
  Serial.print("총 감지 횟수: ");
  Serial.println(count);
}

// ========================================
// 절전 모드 함수
// ========================================
void powerSavingMode() {
  Serial.println("[절전 모드] 동작 감지 대기 중...");
  
  // 동작 감지될 때까지 대기
  while (digitalRead(PIR_PIN) == LOW) {
    delay(100);
  }
  
  Serial.println("[절전 모드 해제] 동작 감지됨!");
  digitalWrite(LED_PIN, HIGH);
  delay(LED_ON_DURATION);
  digitalWrite(LED_PIN, LOW);
}

// ========================================
// 민감도 테스트 함수
// ========================================
void testSensitivity() {
  Serial.println("\n[민감도 테스트]");
  Serial.println("30초간 센서 반응을 관찰합니다.");
  Serial.println("센서 앞에서 천천히 움직여 보세요.\n");
  
  unsigned long startTime = millis();
  int triggerCount = 0;
  
  while (millis() - startTime < 30000) {
    if (digitalRead(PIR_PIN) == HIGH) {
      triggerCount++;
      Serial.print(".");
      delay(500);  // 디바운싱
    }
    delay(100);
  }
  
  Serial.println("\n\n[결과]");
  Serial.print("30초간 트리거 횟수: ");
  Serial.println(triggerCount);
  
  if (triggerCount < 5) {
    Serial.println("→ 민감도가 낮습니다. 센서 조정 필요");
  } else if (triggerCount > 20) {
    Serial.println("→ 민감도가 높습니다. 오감지 가능");
  } else {
    Serial.println("→ 민감도가 적절합니다.");
  }
}
