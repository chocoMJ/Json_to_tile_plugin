## Json to Tile plugin
####  프로젝트 구성
SketchToJson 프로젝트는, 사용자의 손그림 이미지 입력을 받아 이를 타일 맵으로 변환하는 프로젝트이다. 사용자가 이미지를 입력시키면, 웹 에디터상에서 수정을 거치고 최종적으로 타일맵 정보를 담은 Json 파일을 획득할 수 있다. 본 레포지토리는, 획득한 Json파일을 언리얼 에셋과 매핑하여 실제 언리얼 타일 맵을 생성하는 플러그인이다.

---
#### 설치 및 사용 방법

1. 저장소를 클론하거나, JsonTOTileGame.zip파일을 다운로드 한 후 압축을 푼다.

2. `JsonTOTileGame` 폴더를 사용하려는 Unreal 프로젝트의 `Plugins` 폴더 안에 복사한다. 프로젝트에 `Plugins` 폴더가 없다면 직접 생성한다. 여기서, JsonTOTileGame 폴더 이름이 다르다면 플러그인이 제대로 작동하지 않음에 주의한다.

```text
YourUnrealProject/
  Plugins/
    JsonTOTileGame/
      JsonTOTileGame.uplugin
      Source/
      Resources/
      Config/
```
3. 에디터 내 Window탭에서 JsonTOTileGame을 통해 플러그인을 사용할 수 있다. 클릭시 다음과 같은 화면이 나오게 된다.
 <img width="1039" height="686" alt="스크린샷 2026-06-05 010518" src="https://github.com/user-attachments/assets/1b29db56-75cc-4342-b1a0-111a12f3915e" />
4. Browse를 통해 웹 에디터에서 획득한 Json 파일을 선택한 후, Create Mapping DA 버튼과 Create Object Mapping DA 버튼을 클릭한다. 이 경우 각각에 타입에 맞는 Data 매핑 테이블이 자동 생성된다.<img width="1173" height="494" alt="스크린샷 2026-06-05 010624" src="https://github.com/user-attachments/assets/6614adb3-0eeb-483d-b872-9fb2913b71bd" />

5. GenerateMap 버튼을 클릭시 해당되는 타일맵이 생성된다.

---

#### 폴더 구조

