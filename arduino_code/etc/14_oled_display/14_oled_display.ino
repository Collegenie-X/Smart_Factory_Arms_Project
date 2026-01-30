/*
 * ========================================
 * OLED 디스플레이 (SSD1306) 제어 예제
 * ========================================
 * 
 * 기능:
 * - 128x64 OLED 디스플레이 제어
 * - 텍스트, 그래픽, 아이콘 표시
 * - 센서 데이터 시각화
 * - 애니메이션 및 스크롤
 * 
 * 하드웨어:
 * - 0.96" OLED (SSD1306, I2C)
 * - 해상도: 128x64 픽셀
 * 
 * 라이브러리:
 * - Adafruit_GFX
 * - Adafruit_SSD1306
 * 
 * 작성일: 2026-01-30
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ========================================
// OLED 설정
// ========================================
#define SCREEN_WIDTH    128   // 픽셀
#define SCREEN_HEIGHT   64    // 픽셀
#define OLED_RESET      -1    // Reset 핀 (공유 시 -1)
#define OLED_ADDRESS    0x3C  // I2C 주소

// ========================================
// 전역 변수
// ========================================
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ========================================
// 초기화 함수
// ========================================
void setup() {
  Serial.begin(9600);
  Serial.println("========================================");
  Serial.println("  OLED 디스플레이 제어 시스템");
  Serial.println("========================================");
  
  // OLED 초기화
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("[에러] OLED 초기화 실패!");
    while(1);
  }
  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  Serial.println("[초기화 완료]");
  Serial.println("\n명령어:");
  Serial.println("  text:텍스트 - 텍스트 표시");
  Serial.println("  graph - 센서 그래프");
  Serial.println("  icon - 아이콘 표시");
  Serial.println("  chart - 차트 표시");
  Serial.println("  scroll - 스크롤 텍스트");
  Serial.println("  sensor - 센서 대시보드");
  Serial.println("  anim - 애니메이션");
  Serial.println("  clear - 화면 지우기");
  Serial.println("========================================");
  Serial.println();
  
  // 시작 화면
  showStartScreen();
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
  
  // 텍스트 표시
  if (cmd.startsWith("text:")) {
    String text = cmd.substring(5);
    displayText(text);
    
  // 그래프 표시
  } else if (cmd == "graph") {
    displayGraph();
    
  // 아이콘 표시
  } else if (cmd == "icon") {
    displayIcons();
    
  // 차트 표시
  } else if (cmd == "chart") {
    displayChart();
    
  // 스크롤 텍스트
  } else if (cmd == "scroll") {
    scrollText();
    
  // 센서 대시보드
  } else if (cmd == "sensor") {
    sensorDashboard();
    
  // 애니메이션
  } else if (cmd == "anim") {
    playAnimation();
    
  // 화면 지우기
  } else if (cmd == "clear") {
    display.clearDisplay();
    display.display();
    Serial.println("[실행] 화면 지우기");
    
  } else {
    Serial.println("[에러] 알 수 없는 명령어");
  }
}

// ========================================
// 시작 화면 함수
// ========================================
void showStartScreen() {
  display.clearDisplay();
  
  // 제목
  display.setTextSize(2);
  display.setCursor(10, 0);
  display.println("SMART");
  display.setCursor(10, 20);
  display.println("FACTORY");
  
  // 서브타이틀
  display.setTextSize(1);
  display.setCursor(20, 45);
  display.println("OLED Display");
  
  display.display();
  delay(2000);
}

// ========================================
// 텍스트 표시 함수
// ========================================
void displayText(String text) {
  display.clearDisplay();
  
  // 여러 줄 자동 줄바꿈
  display.setTextSize(1);
  display.setCursor(0, 0);
  
  int lineHeight = 10;
  int currentLine = 0;
  int startIdx = 0;
  
  while (startIdx < text.length()) {
    int endIdx = startIdx + 21;  // 한 줄에 약 21자
    if (endIdx > text.length()) {
      endIdx = text.length();
    }
    
    String line = text.substring(startIdx, endIdx);
    display.setCursor(0, currentLine * lineHeight);
    display.println(line);
    
    startIdx = endIdx;
    currentLine++;
    
    if (currentLine >= 6) break;  // 최대 6줄
  }
  
  display.display();
  
  Serial.print("[표시] 텍스트: ");
  Serial.println(text);
}

// ========================================
// 그래프 표시 함수
// ========================================
void displayGraph() {
  Serial.println("[실행] 실시간 그래프 표시 (10초)");
  
  int data[128];
  int dataIndex = 0;
  
  for (int i = 0; i < 100; i++) {
    // 임의의 센서 값 생성 (0~63)
    int value = random(0, 64);
    
    // 데이터 저장
    data[dataIndex] = value;
    dataIndex = (dataIndex + 1) % 128;
    
    // 화면 지우기
    display.clearDisplay();
    
    // 제목
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Sensor: ");
    display.println(value);
    
    // 그래프 그리기
    for (int x = 0; x < 127; x++) {
      int idx = (dataIndex + x) % 128;
      int y = 64 - data[idx];
      display.drawPixel(x, y, SSD1306_WHITE);
    }
    
    display.display();
    delay(100);
  }
  
  Serial.println("[완료] 그래프 표시 종료");
}

// ========================================
// 아이콘 표시 함수
// ========================================
void displayIcons() {
  display.clearDisplay();
  
  // 온도 아이콘
  drawTemperatureIcon(10, 10);
  display.setCursor(30, 15);
  display.print("25C");
  
  // 습도 아이콘
  drawHumidityIcon(70, 10);
  display.setCursor(90, 15);
  display.print("60%");
  
  // 체크 마크
  drawCheckIcon(10, 40);
  display.setCursor(30, 45);
  display.print("OK");
  
  // 경고 아이콘
  drawWarningIcon(70, 40);
  display.setCursor(90, 45);
  display.print("WARN");
  
  display.display();
  
  Serial.println("[표시] 아이콘");
}

// ========================================
// 온도 아이콘 그리기
// ========================================
void drawTemperatureIcon(int x, int y) {
  // 간단한 온도계 모양
  display.drawCircle(x + 5, y + 15, 3, SSD1306_WHITE);
  display.drawRect(x + 3, y, 5, 12, SSD1306_WHITE);
  display.fillCircle(x + 5, y + 15, 2, SSD1306_WHITE);
}

// ========================================
// 습도 아이콘 그리기
// ========================================
void drawHumidityIcon(int x, int y) {
  // 물방울 모양
  display.drawCircle(x + 5, y + 12, 5, SSD1306_WHITE);
  display.drawLine(x + 5, y + 5, x + 2, y + 10, SSD1306_WHITE);
  display.drawLine(x + 5, y + 5, x + 8, y + 10, SSD1306_WHITE);
}

// ========================================
// 체크 아이콘 그리기
// ========================================
void drawCheckIcon(int x, int y) {
  display.drawLine(x, y + 8, x + 4, y + 12, SSD1306_WHITE);
  display.drawLine(x + 4, y + 12, x + 12, y, SSD1306_WHITE);
}

// ========================================
// 경고 아이콘 그리기
// ========================================
void drawWarningIcon(int x, int y) {
  // 삼각형
  display.drawLine(x + 6, y, x, y + 12, SSD1306_WHITE);
  display.drawLine(x + 6, y, x + 12, y + 12, SSD1306_WHITE);
  display.drawLine(x, y + 12, x + 12, y + 12, SSD1306_WHITE);
  // 느낌표
  display.drawLine(x + 6, y + 4, x + 6, y + 8, SSD1306_WHITE);
  display.drawPixel(x + 6, y + 10, SSD1306_WHITE);
}

// ========================================
// 차트 표시 함수
// ========================================
void displayChart() {
  display.clearDisplay();
  
  // 제목
  display.setTextSize(1);
  display.setCursor(20, 0);
  display.println("Production");
  
  // 막대 그래프
  int data[] = {40, 60, 35, 50};
  int barWidth = 20;
  int spacing = 8;
  
  for (int i = 0; i < 4; i++) {
    int x = 10 + i * (barWidth + spacing);
    int height = data[i];
    int y = 64 - height;
    
    display.fillRect(x, y, barWidth, height, SSD1306_WHITE);
    
    // 값 표시
    display.setCursor(x + 5, 55);
    display.print(data[i]);
  }
  
  display.display();
  
  Serial.println("[표시] 차트");
}

// ========================================
// 스크롤 텍스트 함수
// ========================================
void scrollText() {
  Serial.println("[실행] 스크롤 텍스트");
  
  String message = "Smart Factory System - OLED Display Test - ";
  
  for (int offset = 0; offset < message.length() * 6; offset++) {
    display.clearDisplay();
    
    display.setTextSize(1);
    display.setCursor(-offset + 128, 28);
    display.print(message);
    
    display.display();
    delay(50);
  }
  
  Serial.println("[완료] 스크롤 종료");
}

// ========================================
// 센서 대시보드 함수
// ========================================
void sensorDashboard() {
  Serial.println("[실행] 센서 대시보드 (10초)");
  
  for (int i = 0; i < 100; i++) {
    // 임의 센서 값
    int temp = random(20, 30);
    int humidity = random(40, 80);
    int gas = random(0, 100);
    
    display.clearDisplay();
    
    // 제목
    display.setTextSize(1);
    display.setCursor(25, 0);
    display.println("Dashboard");
    
    // 구분선
    display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
    
    // 온도
    display.setCursor(0, 15);
    display.print("Temp: ");
    display.print(temp);
    display.print("C");
    
    // 습도
    display.setCursor(0, 30);
    display.print("Humid:");
    display.print(humidity);
    display.print("%");
    
    // 가스
    display.setCursor(0, 45);
    display.print("Gas:  ");
    display.print(gas);
    
    // 상태 표시
    display.setCursor(80, 15);
    if (gas < 50) {
      display.print("SAFE");
    } else {
      display.print("WARN");
    }
    
    display.display();
    delay(100);
  }
  
  Serial.println("[완료] 대시보드 종료");
}

// ========================================
// 애니메이션 함수
// ========================================
void playAnimation() {
  Serial.println("[실행] 애니메이션");
  
  // 원 애니메이션
  for (int r = 0; r < 32; r += 2) {
    display.clearDisplay();
    display.drawCircle(64, 32, r, SSD1306_WHITE);
    display.display();
    delay(50);
  }
  
  // 사각형 애니메이션
  for (int s = 0; s < 32; s += 2) {
    display.clearDisplay();
    display.drawRect(64 - s, 32 - s, s * 2, s * 2, SSD1306_WHITE);
    display.display();
    delay(50);
  }
  
  Serial.println("[완료] 애니메이션 종료");
}
