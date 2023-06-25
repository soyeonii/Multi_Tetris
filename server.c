#include "netprog.h"

#define MAXLINE 100
#define MAX_ROOM 5
#define MAX_CLIENT 2

typedef struct {	// 클라이언트 정보를 저장하는 구조체
	int sock;					// 소켓번호
	struct sockaddr_in addr;	//소켓주소
	int room_id;				// 방 번호
} Client;

typedef struct {	// 방 정보를 저장하는 구조체
	int number_client;					// 클라이언트 수
	int client_sock_list[MAX_CLIENT];	// 클라이언트 소켓
	int score_list[MAX_CLIENT];			// 게임 점수
	bool is_playing;					// 게임 중인지 여부
} Room;

void remove_client(int room_id, int sock);	// 클라이언트 제거
void *handle_client(void *arg);				// 클라이언트 스레드 함수
void *check_room(void *arg);				// 모든 방마다 두 명이 모였는지(게임 시작이 가능한지) 확인하는 스레드 함수

Room room_list[MAX_ROOM];								// 방 배열
pthread_mutex_t room_lock = PTHREAD_MUTEX_INITIALIZER;	// 방 mutex (초기화)
pthread_cond_t room_cond[MAX_ROOM];						// 각 방의 condition variable

int main(int argc, char *argv[]) {
	struct sockaddr_in cliaddr;
	int listen_sock, sock, addrlen = sizeof(struct sockaddr_in), nbyte, i;
	char buf[MAXLINE+1];
	pthread_t tid;

	if (argc != 2) {
		printf("Usage: %s port\n", argv[0]);
		exit(1);
	}
	
	// 서버 소켓 생성 및 연결 요청 대기
	listen_sock = tcp_listen(INADDR_ANY, atoi(argv[1]), 5);

	printf("Server is running. Waiting for clients...\n\n");

	// 모든 방 & 조건 변수 초기화
	for (i = 0; i < MAX_ROOM; i++) {
		room_list[i].number_client = 0;
		room_list[i].score_list[0] = -1;
		room_list[i].score_list[1] = -1;
		room_list[i].is_playing = false;
		pthread_cond_init(&room_cond[i], NULL);
	}

	// 모든 방마다 두 명이 모였는지(게임 시작이 가능한지)를 확인하는 스레드
	pthread_create(&tid, NULL, check_room, NULL);

	while (1) {
		sock = accept(listen_sock, (struct sockaddr *)&cliaddr, &addrlen);	// 클라이언트 연결 요청 수락
		printf("★ Client connected: %s (%d)\n\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);

		// 클라이언트 정보 구조체 생성
		Client *client = (Client *)malloc(sizeof(Client));
		client->sock = sock;
		client->addr = cliaddr;
		client->room_id = -1;

		pthread_create(&tid, NULL, handle_client, client);	// 클라이언트 스레드 생성

		pthread_detach(tid);	// 스레드 분리 (스레드 종료 즉시 free())
	}

	close(listen_sock);
	return 0;
}

void remove_client(int room_id, int sock) {
	Room *room = &room_list[room_id];
	int i;

	if (room_id == -1) {
		return;
	}
	
	pthread_mutex_lock(&room_lock);
	for (i = 0; i < room->number_client; i++) {
		if (sock == room->client_sock_list[i]) {
			if (i != room->number_client-1) {
				room->client_sock_list[i] = room->client_sock_list[room->number_client-1];
				room->score_list[i] = room->score_list[room->number_client-1];
			}
			room->number_client--;
		}
	}
	pthread_mutex_unlock(&room_lock);
}

void *handle_client(void *arg) {
	Client client = *((Client *)arg);
	int i, nbyte;
	char buf[MAXLINE+1];

	while (1) {
		nbyte = read(client.sock, buf, MAXLINE);	// 클라이언트로부터 선택한 메뉴를 전달받음
		if (nbyte <= 0) {
			printf("☆ Client disconnected: %s (%d)\n\n", inet_ntoa(client.addr.sin_addr), client.addr.sin_port);
			remove_client(client.room_id, client.sock);	// 클라이언트 제거
			close(client.sock);
			break;
		}
		buf[nbyte] = 0;
	
		if (atoi(buf) == 1) {	// (1) Play 선택 시
			nbyte = read(client.sock, buf, MAXLINE);	// 클라이언트로부터 방 번호를 전달받음
			if (nbyte <= 0) {
				printf("☆ Client disconnected: %s (%d)\n\n", inet_ntoa(client.addr.sin_addr), client.addr.sin_port);
				remove_client(client.room_id, client.sock);	// 클라이언트 제거
				close(client.sock);
				break;
			}
			buf[nbyte] = 0;
		
			int room_id = atoi(buf)-1;
			if (!(room_id >= 0 && room_id < MAX_ROOM)) {	// 잘못된 방 번호인 경우
				sprintf(buf, "Invalid Room ID");
				nbyte = strlen(buf);
				write(client.sock, buf, nbyte);	// 클라이언트에게 오류 전달
				continue;
			}

			pthread_mutex_lock(&room_lock);	// 뮤텍스 잠금

			// 게임에 참여한 적이 있으면 이전에 들어간 방에서 자신의 정보를 제거
			int prev_room_id = client.room_id;
			if (prev_room_id != -1) {
				Room *prev_room = &room_list[prev_room_id];
				for (i = 0; i < prev_room->number_client; i++) {
					if (prev_room->client_sock_list[i] == client.sock) {
						prev_room->client_sock_list[i] = prev_room->client_sock_list[prev_room->number_client-1];
						prev_room->number_client--;
						break;
					}
				}
			}
	
			Room *room = &room_list[room_id];
			int player = room->number_client;
			if (!room->is_playing && room->number_client < MAX_CLIENT) {	// 입장 가능한 방인 경우
				room->client_sock_list[room->number_client] = client.sock;
				room->number_client++;
				client.room_id = room_id;

				sprintf(buf, "Joined Room #%d", room_id+1);	// 클라이언트에게 입장했음을 전달
				nbyte = strlen(buf);
				write(client.sock, buf, nbyte);
				printf("♬ %s: %s (%d)\n\n", buf, inet_ntoa(client.addr.sin_addr), client.addr.sin_port);

				if (room->number_client < MAX_CLIENT) {
					pthread_cond_wait(&room_cond[room_id], &room_lock);
				}
			} else {														// 이미 게임이 진행 중인 방인 경우
				sprintf(buf, "Game in Progress Room #%d", room_id+1);	// 클라이언트에게 오류 전달
				nbyte = strlen(buf);
				write(client.sock, buf, nbyte);
				pthread_mutex_unlock(&room_lock);	// 뮤텍스 해제
				continue;
			}

			pthread_mutex_unlock(&room_lock);	// 뮤텍스 해제

			nbyte = read(client.sock, buf, MAXLINE);	// 테트리스로부터 게임 점수를 전달받음
			if (nbyte <= 0) {
				remove_client(client.room_id, client.sock);	// 클라이언트 제거
				close(client.sock);
				break;
			}
			buf[nbyte] = 0;

			int my_score = atoi(buf);
			room->score_list[player] = my_score;	// 게임 점수 저장

			while (room->score_list[0] == -1 || room->score_list[1] == -1);	// 모든 플레이어의 게임이 끝날 때까지 대기

			int sum_score = 0;	// 플레이어의 게임 점수 합
			int max_score = 0;	// 플레이어의 최고 점수
			for (i = 0; i < MAX_CLIENT; i++) {
				sum_score += room->score_list[i];
				if (max_score < room->score_list[i]) {
					max_score = room->score_list[i];
				}
			}
			
			if (my_score == max_score) {
				if (my_score == sum_score / 2) {	// 비긴 경우
					sprintf(buf, "~~DEFEAT~~");
					nbyte = strlen(buf);
				} else {							// 이긴 경우
					sprintf(buf, "YOU WIN!!!");
					nbyte = strlen(buf);
				}
			} else {								// 진 경우
				sprintf(buf, "YOU LOSE..");
				nbyte = strlen(buf);
			}
			write(client.sock, buf, nbyte);	// 클라이언트에게 게임 결과 전달

			// 게임이 종료된 방을 다시 입장 가능한 방으로 만듦
			room->number_client--; 
			room->is_playing = false;

			printf("♪ Left Room #%d: %s (%d)\n\n", room_id+1, inet_ntoa(client.addr.sin_addr), client.addr.sin_port);
		} else {
			printf("☆ Client disconnected: %s (%d)\n\n", inet_ntoa(client.addr.sin_addr), client.addr.sin_port);
			remove_client(client.room_id, client.sock); //클라이언트 제거
			break;
		}
	}
	
	close(client.sock);	// 소켓 종료
}

void *check_room(void *arg) {
	int i, j, nbyte;
	char buf[MAXLINE+1];
	
	while (1) {
		pthread_mutex_lock(&room_lock);		// 뮤텍스 잠금
		for (i = 0; i < MAX_ROOM; i++) {
			Room *room = &room_list[i];
			if (!room->is_playing && room->number_client == MAX_CLIENT) {	// 게임이 진행 중이지 않고 두 명이 모인 경우
				room->is_playing = true;	// 방 상태 변경
				sprintf(buf, "Game Start!!\n");
				nbyte = strlen(buf);
				for (j = 0; j < MAX_CLIENT; j++) {
					write(room->client_sock_list[j], buf, nbyte);	// 모든 클라이언트에게 게임 시작 가능함을 전달
				}
				room->score_list[0] = -1;	// 게임 점수 초기화
				room->score_list[1] = -1;
				pthread_cond_signal(&room_cond[i]);	// 클라이언트 스레드에게 조건 알림을 보냄
			}
		}
		pthread_mutex_unlock(&room_lock);	// 뮤텍스 해제
		sleep(1);
	}
}
