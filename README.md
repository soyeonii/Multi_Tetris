# 멀티 테트리스 게임

![image](https://github.com/soyeonii/Multi_Tetris/assets/69310195/ac43c8aa-b0bb-4770-b102-3e43325fca1e)

2인용 멀티 테트리스 게임입니다.

## 게임 소개

- 멀티 테트리스 게임은 전통적인 테트리스 게임의 규칙을 따르며, 두 명의 플레이어가 동시에 게임을 진행할 수 있습니다.
- 플레이어는 떨어지는 블록을 조작하여 가로줄을 완성해 해당 줄이 사라지도록 해야 합니다.
- 시간이 지날수록 블록이 점점 더 빨리 내려오며, 높은 점수를 얻기 위해서는 빠른 반응과 전략이 필요합니다.

## 게임 시나리오

- 테트리스 게임이 진행될 5개의 방이 있습니다.
- 플레이어는 원하는 방을 선택해 들어갑니다.
- 게임이 진행 중인 방에는 들어갈 수 없습니다.
- 방에 플레이어가 두 명이 모이면 게임이 시작됩니다.
- 방 안의 모든 플레이어가 게임을 마치면 각 플레이어에게 게임 결과가 전송됩니다.

### 게임 설명서
```
<조작법>
↑ : 방향 전환 (Shift)
←/→ : 왼쪽/오른쪽으로 이동 (Left/Right)
↓ : 아래로 이동 (Soft Drop)
Space bar : 바닥에 닿을 때까지 이동 (Hard Drop)
P or p : 일시 정지 (Pause)
Esc : 종료 (Quit)

<레벨>
GOAL만큼 가로줄을 없애면 LEVEL이 1 오릅니다.
LEVEL이 오를수록 속도가 빨라집니다.

<점수>
가로줄을 없애면 점수를 얻습니다. 이는 LEVEL에 비례합니다.
여러 줄을 한 번에 없애면 combo배로 점수를 얻습니다. 
Hard Drop 시 보너스 점수를 얻습니다.

블록이 줄에 닿으면 Game Over!
```

## 게임 스크린샷

### 클라이언트 화면

#### 시작화면

<img width="234" alt="image" src="https://github.com/soyeonii/Multi_Tetris/assets/69310195/86375a99-ac22-4a2e-92fc-b1ea28dbace5">
<img width="234" alt="image" src="https://github.com/soyeonii/Multi_Tetris/assets/69310195/f5d872f2-9196-422a-ac0e-5c1f5a641264">
<img width="234" alt="image" src="https://github.com/soyeonii/Multi_Tetris/assets/69310195/903e3997-040e-4c9d-83f8-75114d7e1e01">
<img width="234" alt="image" src="https://github.com/soyeonii/Multi_Tetris/assets/69310195/8afa1ca8-1429-4c04-9a57-6a902563d7bc">

#### 게임 화면

<img width="312" alt="image" src="https://github.com/soyeonii/Multi_Tetris/assets/69310195/7d44e0fc-6154-4ad5-a01f-ac55ac2a34d2">
<img width="312" alt="image" src="https://github.com/soyeonii/Multi_Tetris/assets/69310195/23443e0e-7ff0-4b16-b341-bd25f7b24998">
<img width="312" alt="image" src="https://github.com/soyeonii/Multi_Tetris/assets/69310195/abe3776e-89e3-4eea-8c7b-765877ffcd8c">
<img width="312" alt="image" src="https://github.com/soyeonii/Multi_Tetris/assets/69310195/7679bd87-3530-42db-ab4b-1795835cfae1">
<img width="312" alt="image" src="https://github.com/soyeonii/Multi_Tetris/assets/69310195/572ca2bd-c93a-4950-8808-82113379656b">

#### 게임 결과 화면

<img width="234" alt="image" src="https://github.com/soyeonii/Multi_Tetris/assets/69310195/de619dc3-dd23-4836-9079-b7e22846dc48">
<img width="234" alt="image" src="https://github.com/soyeonii/Multi_Tetris/assets/69310195/95ad2bda-e01c-417b-8153-13ee605e1415">
<img width="234" alt="image" src="https://github.com/soyeonii/Multi_Tetris/assets/69310195/a2f925e8-fb39-409b-b1e7-0c012e066efc">

### 서버 화면

<img width="936" alt="image" src="https://github.com/soyeonii/Multi_Tetris/assets/69310195/c075d7aa-bc66-4a45-808d-c9983baa67b0">

### 플레이 화면

<img width="936" alt="image" src="https://github.com/soyeonii/Multi_Tetris/assets/69310195/40fdce42-e0a1-4ae3-8f51-4cf75e01d2fa">

## 사용된 기술

- **TCP**                 : 플레이어 간의 통신을 처리합니다.
- **mutex**               : 서버 측에서 스레드 간의 동기화 문제가 발생하지 않도록 합니다.
- **condition variable**  : 서버 측에서 방에 처음 들어간 플레이어가 대기 모드에 있다가 한 명이 더 들어오면 게임을 시작하도록 합니다.
- **signal**              : 클라이언트 및 테트리스 게임에서의 active close(CTRL+C)를 무시합니다.
- **execl**               : 클라이언트 프로세스의 이미지를 테트리스 게임으로 변경합니다.
- **fork**                : 클라이언트 측에서 자식 프로세스에서 게임 진행 후 다시 원래의 화면(부모 프로세스)으로 돌아오도록 합니다.
- **select**              : 테트리스 게임 시작 화면에서 별이 반짝이는 효과를 위해 비동기적 입력을 받습니다.
- **ncurses**             : 테트리스 게임 화면을 꾸밉니다.
- **file I/O**                : 테트리스 게임의 최고 점수를 저장합니다.

## 실행 방법

1. 실행 파일을 컴파일합니다.
```shell
make
```

2. 실행 파일을 실행합니다.
  ```shell
  server 포트번호
  ```
  ```shell
  client 127.0.0.1 포트번호
  ```

## 실행 환경

```
OS: Unix
Emulator: PuTTY
Font: Fixedsys, 18-point
```
