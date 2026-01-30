/*
 * ========================================
 * 7 Segment 디스플레이 제어 예제
 * ========================================
 * 
 * 기능:
 * - 단일 7 segment 숫자 표시 (0~9)
 * - TM1637 4자리 7 segment 제어
 * - 시계 표시 (HH:MM)
 * - 온도 표시
 * - 밝기 조절
 * 
 * 하드웨어:
 * - TM1637 4자리 7 Segment 모듈
 * - 또는 단일 7 Segment + 저항
 * 
 * 라이브러리:
 * - TM1637Display (4자리 모듈용)
 * 
 * 작성일: 2026-01-30
 */

#include <TM1637Display.h>

// ========================================
// 핀 정의 (TM1637)
// ========================================
#define CLK_PIN     2     // 클럭 핀
#define DIO_PIN     3     // 데이터 핀

// ========================================
// 단일 7 Segment 핀 정의 (옵션)
// ========================================
// 공통 캐소드 (Common Cathode) 기준
#define SEG_A       4
#define SEG_B       5
#define SEG_C       6
#define SEG_D       7
#define SEG_E       8
#define SEG_F       9
#define SEG_G       10
#define SEG_DP      11    // 소수점

// ========================================
// 전역 변수
// ========================================
TM1637Display display(CLK_PIN, DIO_PIN);

// 7 segment 숫자 패턴 (공통 캐소드)
const byte DIGIT_PATTERNS[10] = {
  0b00111111,  // 0
  0b00000110,  // 1
  0b01011011,  // 2
  0b01001111,  // 3
  0b01100110,  // 4
  0b01101101,  // 5
  0b01111101,  // 6
  0b00000111,  // 7
  0b01111111,  // 8
  0b01101111   // 9
};

int currentNumber = 0;
int brightness = 2;  // 0~7
unsigned long previousMillis = 0;

// ========================================
// 초기화 함수
// ========================================
void setup() {
  Serial.begin(9600);
  Serial.println("========================================");
  Serial.println("  7 Segment 디스플레이 제어 시스템");
  Serial.println("========================================");
  
  // TM1637 초기화
  display.setBrightness(brightness);
  display.clear();
  
  // 단일 7 segment 핀 모드 설정 (옵션)
  pinMode(SEG_A, OUTPUT);
  pinMode(SEG_B, OUTPUT);
  pinMode(SEG_C, OUTPUT);
  pinMode(SEG_D, OUTPUT);
  pinMode(SEG_E, OUTPUT);
  pinMode(SEG_F, OUTPUT);
  pinMode(SEG_G, OUTPUT);
  pinMode(SEG_DP, OUTPUT);
  
  Serial.println("[초기화 완료]");
  Serial.println("\n명령어:");
  Serial.println("  num:숫자 - 숫자 표시 (0~9999)");
  Serial.println("  time:HH:MM - 시간 표시");
  Serial.println("  temp:온도 - 온도 표시 (°C)");
  Serial.println("  bright:레벨 - 밝기 조절 (0~7)");
  Serial.println("  count - 카운트 업 (0~9999)");
  Serial.println("  clock - 시계 모드");
  Serial.println("  clear - 화면 지우기");
  Serial.println("\n예시:");
  Serial.println("  num:1234");
  Serial.println("  time:12:34");
  Serial.println("  temp:25");
  Serial.println("========================================");
  Serial.println();
  
  // 시작 애니메이션
  startupAnimation();
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
}

// ========================================
// 명령 처리 함수
// ========================================
void handleCommand(String cmd) {
  Serial.print("[명령 수신] ");
  Serial.println(cmd);
  
  // 숫자 표시: num:1234
  if (cmd.startsWith("num:")) {
    int number = cmd.substring(4).toInt();
    displayNumber(number);
    
  // 시간 표시: time:12:34
  } else if (cmd.startsWith("time:")) {
    String timeStr = cmd.substring(5);
    displayTime(timeStr);
    
  // 온도 표시: temp:25
  } else if (cmd.startsWith("temp:")) {
    int temp = cmd.substring(5).toInt();
    displayTemperature(temp);
    
  // 밝기 조절: bright:5
  } else if (cmd.startsWith("bright:")) {
    int level = cmd.substring(7).toInt();
    setBrightness(level);
    
  // 카운트 업
  } else if (cmd == "count") {
    countUp();
    
  // 시계 모드
  } else if (cmd == "clock") {
    clockMode();
    
  // 화면 지우기
  } else if (cmd == "clear") {
    display.clear();
    Serial.println("[실행] 화면 지우기");
    
  } else {
    Serial.println("[에러] 알 수 없는 명령어");
  }
}

// ========================================
// 숫자 표시 함수 (4자리)
// ========================================
void displayNumber(int number) {
  // 범위 제한 (0~9999)
  number = constrain(number, 0, 9999);
  
  display.showNumberDec(number, false);  // 선행 0 제거
  
  Serial.print("[표시] 숫자: ");
  Serial.println(number);
}

// ========================================
// 시간 표시 함수 (HH:MM)
// ========================================
void displayTime(String timeStr) {
  // "12:34" 형식 파싱
  int colonIndex = timeStr.indexOf(':');
  
  if (colonIndex == -1) {
    Serial.println("[에러] 잘못된 시간 형식 (HH:MM)");
    return;
  }
  
  int hours = timeStr.substring(0, colonIndex).toInt();
  int minutes = timeStr.substring(colonIndex + 1).toInt();
  
  // 범위 체크
  if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59) {
    Serial.println("[에러] 시간 범위 오류");
    return;
  }
  
  // 시간 표시 (콜론 포함)
  int timeValue = hours * 100 + minutes;
  display.showNumberDecEx(timeValue, 0b01000000, true);  // 가운데 콜론
  
  Serial.print("[표시] 시간: ");
  Serial.print(hours);
  Serial.print(":");
  if (minutes < 10) Serial.print("0");
  Serial.println(minutes);
}

// ========================================
// 온도 표시 함수 (°C)
// ========================================
void displayTemperature(int temp) {
  // -99 ~ 999 범위
  temp = constrain(temp, -99, 999);
  
  // 온도 표시 (°C 기호는 커스텀)
  uint8_t data[] = {0, 0, 0, 0};
  
  // 온도 값을 각 자리로 분리
  if (temp < 0) {
    data[0] = 0b01000000;  // 마이너스 기호
    temp = abs(temp);
    data[1] = display.encodeDigit(temp / 10);
    data[2] = display.encodeDigit(temp % 10);
  } else if (temp < 100) {
    data[0] = display.encodeDigit(temp / 10);
    data[1] = display.encodeDigit(temp % 10);
  } else {
    data[0] = display.encodeDigit(temp / 100);
    data[1] = display.encodeDigit((temp / 10) % 10);
    data[2] = display.encodeDigit(temp % 10);
  }
  
  // 마지막 자리는 'C' (커스텀)
  data[3] = 0b00111001;  // C
  
  display.setSegments(data);
  
  Serial.print("[표시] 온도: ");
  Serial.print(temp);
  Serial.println("°C");
}

// ========================================
// 밝기 조절 함수
// ========================================
void setBrightness(int level) {
  // 범위 제한 (0~7)
  level = constrain(level, 0, 7);
  brightness = level;
  
  display.setBrightness(brightness);
  
  Serial.print("[밝기 조절] 레벨: ");
  Serial.println(brightness);
}

// ========================================
// 카운트 업 함수
// ========================================
void countUp() {
  Serial.println("[실행] 카운트 업 (0~9999)");
  
  for (int i = 0; i <= 9999; i++) {
    display.showNumberDec(i, false);
    delay(10);
    
    // 100마다 출력
    if (i % 100 == 0) {
      Serial.print("현재: ");
      Serial.println(i);
    }
  }
  
  Serial.println("[완료] 카운트 업 종료");
}

// ========================================
// 시계 모드 함수
// ========================================
void clockMode() {
  Serial.println("[실행] 시계 모드 (60초간)");
  Serial.println("Serial 입력 시 종료");
  
  int seconds = 0;
  int minutes = 0;
  int hours = 12;
  
  unsigned long startTime = millis();
  unsigned long lastUpdate = millis();
  bool colonState = true;
  
  while (millis() - startTime < 60000) {
    // Serial 입력 시 종료
    if (Serial.available() > 0) {
      Serial.read();
      break;
    }
    
    // 1초마다 업데이트
    if (millis() - lastUpdate >= 1000) {
      lastUpdate = millis();
      
      seconds++;
      if (seconds >= 60) {
        seconds = 0;
        minutes++;
        if (minutes >= 60) {
          minutes = 0;
          hours++;
          if (hours >= 24) {
            hours = 0;
          }
        }
      }
      
      // 시간 표시 (콜론 깜빡임)
      int timeValue = hours * 100 + minutes;
      if (colonState) {
        display.showNumberDecEx(timeValue, 0b01000000, true);
      } else {
        display.showNumberDecEx(timeValue, 0, true);
      }
      
      colonState = !colonState;
    }
  }
  
  Serial.println("[완료] 시계 모드 종료");
}

// ========================================
// 시작 애니메이션 함수
// ========================================
void startupAnimation() {
  // 모든 세그먼트 켜기
  uint8_t allOn[] = {0xff, 0xff, 0xff, 0xff};
  display.setSegments(allOn);
  delay(500);
  
  // 0~9 빠르게 표시
  for (int i = 0; i <= 9; i++) {
    display.showNumberDec(i, false);
    delay(100);
  }
  
  // 화면 지우기
  display.clear();
}

// ========================================
// 단일 7 Segment 제어 함수 (옵션)
// ========================================
void displaySingleDigit(int digit) {
  if (digit < 0 || digit > 9) {
    return;
  }
  
  byte pattern = DIGIT_PATTERNS[digit];
  
  digitalWrite(SEG_A, (pattern & 0b00000001) ? HIGH : LOW);
  digitalWrite(SEG_B, (pattern & 0b00000010) ? HIGH : LOW);
  digitalWrite(SEG_C, (pattern & 0b00000100) ? HIGH : LOW);
  digitalWrite(SEG_D, (pattern & 0b00001000) ? HIGH : LOW);
  digitalWrite(SEG_E, (pattern & 0b00010000) ? HIGH : LOW);
  digitalWrite(SEG_F, (pattern & 0b00100000) ? HIGH : LOW);
  digitalWrite(SEG_G, (pattern & 0b01000000) ? HIGH : LOW);
  digitalWrite(SEG_DP, LOW);  // 소수점 끄기
}

// ========================================
// 스크롤 텍스트 함수
// ========================================
void scrollText(String text) {
  // 추후 확장: 텍스트 스크롤 기능
  Serial.println("[알림] 스크롤 기능은 추후 구현 예정");
}
