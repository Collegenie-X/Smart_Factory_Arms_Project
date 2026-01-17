# Teachable Machine + 앱인벤터 퀵스타트 가이드 🚀
> **10분 안에 핵심만 빠르게 이해하기**

## ✅ 결론부터: 가능합니다!

**Teachable Machine 모델을 앱인벤터에서 100% 사용 가능합니다.**

단, **이미지 분류**만 지원됩니다 (포즈 인식, 객체 검출은 불가).

---

## 🎯 5단계 요약

```mermaid
graph LR
    A[1. TM에서<br/>모델 학습] --> B[2. TensorFlow.js<br/>내보내기]
    B --> C[3. 클라우드<br/>업로드]
    C --> D[4. 앱인벤터에<br/>Extension 추가]
    D --> E[5. 블록 코딩<br/>완성!]
    
    style E fill:#90EE90,color:#111
```

**예상 소요 시간:** 2-3시간 (처음 하는 경우)

---

## 📋 체크리스트

### 준비물
- [ ] Google 계정
- [ ] 안드로이드 스마트폰 (Android 8.0+)
- [ ] 학습용 이미지 (클래스당 50-100장)
- [ ] TMIC Extension 파일 (.aix)

### 5단계 작업

#### 1단계: Teachable Machine에서 모델 학습 ⏱️ 30분
- [ ] [teachablemachine.withgoogle.com](https://teachablemachine.withgoogle.com) 접속
- [ ] Image Project 선택
- [ ] 3-5개 클래스 생성 (예: class_A, class_B, class_C)
- [ ] 각 클래스당 100장 이미지 수집
- [ ] Train Model 클릭
- [ ] 웹에서 테스트 (80% 이상 정확도 확인)

**⚠️ 주의사항:**
- 클래스 이름은 **영문**으로만 (특수문자 X)
- 다양한 조명, 각도에서 촬영
- 배경도 다양하게

#### 2단계: 모델 내보내기 ⏱️ 5분
- [ ] Export Model 클릭
- [ ] TensorFlow.js 선택
- [ ] **"Upload my model"** 클릭 (가장 쉬움!)
- [ ] URL 복사 (예: `https://teachablemachine.withgoogle.com/models/ABC123/`)

**💡 팁:** Upload 방식이 가장 간단! 별도 호스팅 불필요

#### 3단계: (2단계에서 Upload 선택 시 생략)
- [ ] 이미 완료!

#### 4단계: 앱인벤터 설정 ⏱️ 20분
- [ ] [ai2.appinventor.mit.edu](http://ai2.appinventor.mit.edu) 접속
- [ ] 새 프로젝트 생성
- [ ] [TMIC.aix 다운로드](https://computacaonaescola.ufsc.br/en/tmic/)
- [ ] Extension → Import extension → TMIC.aix 업로드
- [ ] 컴포넌트 추가:
  - Camera (Media)
  - Button (User Interface)
  - Image (User Interface)
  - Label (User Interface)
  - TMIC (Extension)

#### 5단계: 블록 코딩 ⏱️ 30분
- [ ] Screen1.Initialize: ModelURL 설정
- [ ] Button.Click: Camera.TakePicture
- [ ] Camera.AfterPicture: Image 표시 + ClassifyImage
- [ ] TMIC.GotClassification: 결과 표시
- [ ] TMIC.Error: 오류 처리

**📱 테스트:**
- [ ] MIT AI2 Companion 앱 설치
- [ ] Connect → AI Companion
- [ ] QR 코드 스캔
- [ ] 실제 기기에서 테스트!

---

## 🔗 핵심 링크 모음

| 목적 | 링크 |
|------|------|
| **모델 학습** | [Teachable Machine](https://teachablemachine.withgoogle.com) |
| **앱 개발** | [MIT App Inventor](http://ai2.appinventor.mit.edu) |
| **Extension 다운로드** | [TMIC 공식 사이트](https://computacaonaescola.ufsc.br/en/tmic/) |
| **상세 가이드** | [07_앱인벤터_Teachable_Machine_연동가이드.md](./07_앱인벤터_Teachable_Machine_연동가이드.md) |
| **질문/답변** | [MIT App Inventor Community](http://community.appinventor.mit.edu) |

---

## ⚠️ 주요 제약사항

| 기능 | 지원 여부 |
|------|----------|
| 이미지 분류 | ✅ 완벽 지원 |
| 객체 검출 | ❌ 미지원 |
| 포즈 인식 | ❌ 미지원 |
| 오디오 분류 | ❌ 미지원 |
| 오프라인 사용 | ⚠️ 제한적 (첫 로딩 시 인터넷 필요) |

---

## 🆘 문제 해결

### 문제 1: 앱이 로딩 안됨
**해결:** 스마트폰과 컴퓨터가 같은 Wi-Fi에 연결되었는지 확인

### 문제 2: 모델 로딩 실패
**해결:** ModelURL이 정확한지 확인 (복사 시 빈칸 없이)

### 문제 3: 분류 정확도 낮음
**해결:** 
- 각 클래스당 100장 이상으로 데이터 추가
- 다양한 조명/각도에서 재촬영
- Teachable Machine에서 재학습

### 문제 4: 카메라 권한 오류
**해결:** 스마트폰 설정 → 앱 → MIT AI2 Companion → 권한 → 카메라 허용

---

## 🎓 추천 학습 경로

### 초급 (처음 하는 경우)
1. ⭐ 이 문서 읽기 (10분)
2. ⭐ Teachable Machine으로 간단한 모델 만들기 (30분)
3. ⭐ 앱인벤터 기본 튜토리얼 (30분)
4. ⭐ 첫 AI 앱 만들기 (1시간)

### 중급 (기본 완료 후)
1. ⭐⭐ [상세 가이드](./07_앱인벤터_Teachable_Machine_연동가이드.md) 전체 읽기
2. ⭐⭐ 고급 기능 추가 (Confidence 임계값 등)
3. ⭐⭐ 블루투스로 아두이노 연동

### 고급 (프로젝트 적용)
1. ⭐⭐⭐ 실제 문제 해결 프로젝트
2. ⭐⭐⭐ 로봇 팔 제어 통합
3. ⭐⭐⭐ 공모전 준비

---

## 📺 추천 유튜브 영상

- **"Teachable Machine 1: Image Classification"** (The Coding Train)
- **"ML model with Teachable Machine → App Inventor"** (Technovation)
- **"Personal Image Classifier Tutorial"** (MIT App Inventor)

검색 키워드: `"App Inventor Teachable Machine tutorial"`

---

## 💬 자주 묻는 질문 (FAQ)

**Q: 프로그래밍 몰라도 되나요?**
A: 네! 블록 코딩이라서 드래그 앤 드롭으로 가능합니다.

**Q: 정확도가 낮은데요?**
A: 데이터를 100장 이상, 다양한 조건에서 추가하세요.

**Q: 오프라인에서도 작동하나요?**
A: 첫 모델 로딩 시 인터넷 필요하지만, 이후는 가능합니다.

**Q: iOS에서도 되나요?**
A: 안드로이드만 완벽 지원됩니다.

**Q: 로봇 팔과 연결하려면?**
A: [02_블루투스_프로토콜.md](./02_블루투스_프로토콜.md) 참고하세요!

---

## 🎯 첫 프로젝트 추천

### 색상 분류기 (난이도: ⭐)
**목표:** 빨강/파랑/노랑 블록을 카메라로 찍으면 색상 인식

**학습 데이터:**
- 빨간색 물체 100장
- 파란색 물체 100장
- 노란색 물체 100장

**예상 완성 시간:** 2-3시간

### 불량품 검사기 (난이도: ⭐⭐⭐)
**목표:** 제품 사진 찍으면 정상/불량 판정 + 로봇 팔로 분류

**학습 데이터:**
- 정상 제품 100장
- 불량 제품 100장 (긁힘, 변색 등)

**예상 완성 시간:** 10-15시간

---

## 📖 더 자세한 내용은?

**👉 [07_앱인벤터_Teachable_Machine_연동가이드.md](./07_앱인벤터_Teachable_Machine_연동가이드.md)**

이 문서에는:
- ✅ 단계별 스크린샷
- ✅ 블록 코드 예시
- ✅ 문제 해결 상세 가이드
- ✅ 고급 기능 구현 방법
- ✅ 프로젝트 아이디어 10가지

---

**준비되셨나요? 지금 바로 시작해보세요! 🚀**

1. [Teachable Machine](https://teachablemachine.withgoogle.com) 열기
2. Image Project 시작
3. 첫 AI 모델 만들기!

**행운을 빕니다! 💪**
