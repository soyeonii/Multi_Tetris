#include "netprog.h"

#define MAXLINE 100

int main(int argc, char *argv[]) {
	struct sockaddr_in servaddr;
	int sock, i, nbyte;
	char buf[MAXLINE+1];
	pid_t pid;

	if (argc != 3) {
		printf("Usage: %s ip_addr port\n");
		exit(1);
	}
	
	signal(SIGINT, INTHandler);	// CTRL+C(active close) 무시

	sock = tcp_connect(AF_INET, argv[1], atoi(argv[2]));
	if (sock == -1) {
		errquit("Connection fail");
	}

	// 게임 시작 로고
	printf("\n");
	printf("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
	printf("┃   _____  _____  _____  ____   _  _____   ┃\n");
	printf("┃  |_   _||  ___||_   _||  _ \\ | |(  __/   ┃\n");
	printf("┃    | |  | |__    | |  | |/ / | | \\ \\     ┃\n");
	printf("┃    | |  |  _/    | |  | |\\ \\ | |  \\ \\    ┃\n");
	printf("┃    | |  | |____  | |  | | \\ || | __\\ \\   ┃\n");
	printf("┃    |_|  \\______\\ |_|  |_|  \\||_||_____)  ┃\n");
	printf("┃                                          ┃\n");
	printf("┗━━━━━━━━━━━━━┓              ┏━━━━━━━━━━━━━┛\n");
	printf("              ┃              ┃              \n");
	printf("              ┃              ┃              \n");
	printf("              ┃              ┃              \n");
	printf("              ┃              ┃              \n");
	printf("              ┃              ┃              \n");
	printf("              ┃              ┃              \n");
	printf("              ┃              ┃              \n");
	printf("              ┗━━━━━━━━━━━━━━┛");
	printf("\n");

	while (1) {
		printf("\nLet's play Tetris!\n");
		printf("==================\n");
		printf("(1) Play\n");
		printf("(2) Exit\n");
		printf("==================\n");
		printf("Enter: ");
		fgets(buf, sizeof(buf), stdin);
		nbyte = strlen(buf);
		write(sock, buf, nbyte);	// 서버에게 메뉴 선택 전달

		int input = atoi(buf);
		if (input == 1) {			// (1) Play 선택 시
			printf("Room ID(1~5): ");
			fgets(buf, sizeof(buf), stdin);
			nbyte = strlen(buf);
			write(sock, buf, nbyte);	// 서버에게 방 번호 전달

			nbyte = read(sock, buf, MAXLINE);	// 방 입장 결과 출력
			if (nbyte <= 0) {
				close(sock);
				break;
			}
			buf[nbyte] = 0;
			printf("\n%s\n", buf);	// "Joined Room #?"
									// "Invalid Room ID"
									// "Game in Progress Room #?"

			// 이미 게임이 진행 중이거나 잘못된 방 번호를 입력했을 경우 다시 선택하도록
			if (strstr(buf, "Progress") != NULL || strstr(buf, "Invalid") != NULL) {
				continue;
			}

			printf("\nWaiting for the other player...\n");

			nbyte = read(sock, buf, MAXLINE);	// 방에 2명이 모일 때까지 기다림
			if (nbyte <= 0) {
				close(sock);
				break;
			}
			buf[nbyte] = 0;
			printf("%s\n", buf);	// "Game Start!!"

			sleep(1);

			if ((pid = fork()) < 0) {
				errquit("fork error");
			} else if (pid == 0) {	// 자식 프로세스
				sprintf(buf, "%d", sock);
				execl("tetris", "tetris", buf);
			} else {				// 부모 프로세스
				int status;
				waitpid(pid, &status, 0);	// 자식 프로세스의 종료를 기다림

				nbyte = read(sock, buf, MAXLINE);	// 서버로부터 게임 결과를 전달받음
				if (nbyte <= 0) {
					close(sock);
					break;
				}
				buf[nbyte] = 0;
				printf("+------------+\n");
				printf("| %s |\n", buf);	// "YOU WIN!!!", "YOU LOSE..", "~~DEFEAT~~"
				printf("+------------+\n");
			}
		} else if (input == 2) {	// (2) Exit 선택 시
			puts("\nThanks for Playing!");
			close(sock);
			break;
		}
	}

	return 0;
}
