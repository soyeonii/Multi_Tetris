# 멀티 테트리스 게임

![image](https://github.com/soyeonii/Multi_Tetris/assets/69310195/ac43c8aa-b0bb-4770-b102-3e43325fca1e)

2인용 멀티 테트리스 게임입니다.

## 게임 소개

- 멀티 테트리스 게임은 전통적인 테트리스 게임의 규칙을 따르며, 두 명의 플레이어가 동시에 게임을 진행할 수 있습니다.
- 플레이어는 떨어지는 블록을 조작하여 가로 줄을 완성시켜 해당 줄이 사라지도록 해야 합니다.
- 시간이 지날수록 블록이 점점 더 빨리 내려오며, 높은 점수를 얻기 위해서는 빠른 반응과 전략이 필요합니다.

## 게임 시나리오

- 테트리스 게임이 진행될 5개의 방이 있습니다.
- 플레이어는 원하는 방을 선택해 들어갑니다.
- 게임이 진행 중인 방에는 들어갈 수 없습니다.
- 방에 플레이어가 두 명이 모이면 게임이 시작됩니다.
- 방 안의 모든 플레이어가 게임을 마치면 각 플레이어에게 게임 결과가 전송됩니다.

## 게임 스크린샷

### 클라이언트 화면

#### 시작화면

<img width="369" alt="image" src="https://github.com/soyeonii/Multi_Tetris/assets/69310195/a18c2a6a-bbad-4e97-af5f-fb154d7f8b09">
<img width="369" alt="image" src="https://github.com/soyeonii/Multi_Tetris/assets/69310195/494b7c8a-8a0c-4abc-9635-e6fca7b99560">
<img width="369" alt="image" src="https://github.com/soyeonii/Multi_Tetris/assets/69310195/3596b70d-0ad0-4fce-a31d-8c0792025fc2">
<img width="369" alt="image" src="https://github.com/soyeonii/Multi_Tetris/assets/69310195/09989485-029d-45e7-82df-68c3e6069af4">

#### 게임 화면

<img width="561" alt="image" src="https://github.com/soyeonii/Multi_Tetris/assets/69310195/5a8ddec2-6f98-4470-bd10-736cfce97012">
<img width="561" alt="image" src="https://github.com/soyeonii/Multi_Tetris/assets/69310195/4e68ddd6-f718-40de-98f7-0773738672fc">
<img width="561" alt="image" src="https://github.com/soyeonii/Multi_Tetris/assets/69310195/23aec7c0-ce79-4cc2-9e6b-b39236ad8e22">
<img width="561" alt="image" src="https://github.com/soyeonii/Multi_Tetris/assets/69310195/eecbddb4-dee1-4475-8490-d3e8c353f12f">
<img width="561" alt="image" src="https://github.com/soyeonii/Multi_Tetris/assets/69310195/f9cb2da4-7e27-4e05-a1bd-5590ee0f7e79">

#### 게임 결과 화면

<img width="369" alt="image" src="https://github.com/soyeonii/Multi_Tetris/assets/69310195/198462ab-f7f0-442a-88cb-272b44fdab67">
<img width="369" alt="image" src="https://github.com/soyeonii/Multi_Tetris/assets/69310195/ac8f8907-e6f7-43a5-ba1d-a881db5ea23b">
<img width="369" alt="image" src="https://github.com/soyeonii/Multi_Tetris/assets/69310195/ecc86791-a9ed-4bcf-a8ea-436ecf8fdad7">

### 서버 화면

<img width="960" alt="image" src="https://github.com/soyeonii/Multi_Tetris/assets/69310195/c075d7aa-bc66-4a45-808d-c9983baa67b0">

## 사용된 기술

- **TCP**                 : 플레이어 간의 통신을 처리합니다.
- **mutex**               : 서버 측에서 스레드 간의 동기화 문제가 발생하지 않도록 합니다.
- **condition variable**  : 서버 측에서 방에 들어간 스레드가 대기 모드에 있다가 한 명이 더 들어오면 게임을 시작합니다.
- **signal**              : 클라이언트 및 테트리스 게임에서의 active close(CTRL+C)를 금지합니다.
- **execl**               : 클라이언트 프로세스 이미지를 변경합니다.
- **fork**                : 클라이언트 측에서 게임 진행 후 다시 원래의 화면으로 돌아오도록 합니다.
- **select**              : 테트리스 게임 시작 화면에서 별이 반짝이는 효과를 위해 비동기적 입력을 받습니다.
- **ncurses**             : 테트리스 게임 화면을 꾸밉니다.

## 실행 방법

1. 실행 파일을 만듭니다.
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
