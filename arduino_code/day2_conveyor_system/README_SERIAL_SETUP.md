# 시리얼 모니터 한글 설정 가이드

## 📌 한글이 깨져 보이는 경우

아두이노 시리얼 모니터에서 한글이 깨져서 나오는 경우, 다음 설정을 확인하세요.

---

## 🔧 아두이노 IDE 1.x 버전

### 방법 1: 기본 설정 확인
1. 시리얼 모니터를 엽니다 (Ctrl+Shift+M 또는 Tools > Serial Monitor)
2. 우측 하단의 보드레이트를 **9600 baud**로 설정
3. 줄 끝 처리를 **Newline** 또는 **Both NL & CR**로 설정

### 방법 2: 폰트 설정 (Windows)
1. `파일` → `환경설정`
2. `편집기 폰트`를 한글을 지원하는 폰트로 변경
   - 추천 폰트: `맑은 고딕`, `나눔고딕`, `D2Coding`

### 방법 3: 대체 터미널 사용
아두이노 IDE의 시리얼 모니터가 한글을 제대로 표시하지 못하는 경우:

**PuTTY 사용 (Windows):**
1. PuTTY 다운로드 및 설치
2. Connection type: Serial
3. Serial line: COM 포트 번호 (장치 관리자에서 확인)
4. Speed: 9600
5. Window > Translation > UTF-8 선택

**screen 사용 (Mac/Linux):**
```bash
screen /dev/tty.usbmodem* 9600
```
종료: Ctrl+A, K

---

## 🔧 아두이노 IDE 2.x 버전

### 한글 표시 설정
1. 시리얼 모니터 열기
2. 우측 상단의 보드레이트를 **9600 baud**로 설정
3. 한글이 깨지는 경우, 터미널 프로그램 사용 권장

---

## 🖥️ 대체 시리얼 터미널 프로그램

### 1. CoolTerm (Windows/Mac/Linux)
- 다운로드: https://freeware.the-meiers.org/
- 한글 UTF-8 완벽 지원
- 설정:
  - Options > Serial Port > Baud Rate: 9600
  - Options > Terminal > Terminal Mode: Line Mode
  - Options > Terminal > Enter Key Emulation: CR+LF

### 2. Tera Term (Windows)
- 다운로드: https://ttssh2.osdn.jp/
- 설정:
  - Setup > Serial port > Speed: 9600
  - Setup > Terminal > Receive: UTF-8

### 3. Serial Monitor Extension (VS Code)
VS Code를 사용하는 경우:
1. "Serial Monitor" 확장 설치
2. 포트 선택
3. Baud Rate: 9600
4. UTF-8 자동 지원

---

## 💡 코드 수정 없이 해결하기

### 모든 이모지 제거
이 프로젝트의 모든 `.ino` 파일에서는 이미 이모지를 제거했습니다:
- ✓, ✗, ❓ 등의 특수 문자 제거
- 🔴, 🟢, 🔵 등의 이모지 제거
- 순수 한글 텍스트만 사용

### F() 매크로 사용
플래시 메모리 절약을 위해 F() 매크로를 사용하고 있습니다:
```cpp
Serial.println(F("한글 텍스트"));
```

이것은 정상적으로 작동하며, 시리얼 모니터의 인코딩 설정만 올바르면 한글이 표시됩니다.

---

## 🔍 문제 해결 체크리스트

- [ ] 보드레이트가 9600으로 설정되어 있는가?
- [ ] 시리얼 모니터의 인코딩이 UTF-8인가?
- [ ] 아두이노가 제대로 연결되어 있는가? (장치 관리자 확인)
- [ ] COM 포트가 올바르게 선택되어 있는가?
- [ ] 다른 프로그램이 같은 COM 포트를 사용하고 있지 않은가?

---

## 📱 추천 설정

### Windows 사용자
- **CoolTerm** 또는 **Tera Term** 사용
- UTF-8 인코딩 설정
- 9600 baud

### Mac 사용자
- **CoolTerm** 사용
- 또는 터미널에서 `screen` 명령 사용
- 9600 baud

### Linux 사용자
- **CoolTerm** 또는 **minicom** 사용
- UTF-8 인코딩 설정
- 9600 baud

---

## ⚙️ 시리얼 통신 테스트

간단한 테스트 코드로 한글이 제대로 표시되는지 확인:

```cpp
void setup() {
  Serial.begin(9600);
  Serial.println("아두이노 시작");
  Serial.println("한글 테스트");
}

void loop() {
  Serial.println("실행중...");
  delay(1000);
}
```

이 코드를 업로드하고 시리얼 모니터에서 "아두이노 시작", "한글 테스트", "실행중..."이 제대로 표시되는지 확인하세요.

---

## 📞 추가 지원

한글이 여전히 깨지는 경우:
1. 아두이노 IDE 버전 확인 (최신 버전 권장)
2. USB 드라이버 재설치
3. 다른 USB 포트 시도
4. 대체 터미널 프로그램 사용

**중요:** 이 프로젝트의 모든 코드는 이모지를 제거하고 순수 한글 텍스트만 사용하도록 수정되었습니다.
