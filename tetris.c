#include "netprog.h"

#define MAXLINE 100

#define SPACE 32	// hard drop
#define P 80		// 일시 정지
#define p 112
#define ESC 27		// 게임 종료
#define SPECIAL 100	// 특수 동작

#define ACTIVE_BLOCK -2		// 게임판 배열에 저장될 블록의 상태
#define CEILLING -1			// 블록이 이동할 수 "있"는 공간
#define EMPTY 0				// 블록이 이동할 수 "있"는 공간
#define WALL 1				// 블록이 이동할 수 "없"는 공간
#define INACTIVE_BLOCK 2	// 이동이 완료된 블록

#define MAIN_X 22		// 게임판 가로 크기
#define MAIN_Y 23		// 게임판 세로 크기
#define MAIN_X_ADJ 3	// 게임판 위치 조정
#define MAIN_Y_ADJ 1	// 게임판 위치 조정

#define STATUS_X_ADJ MAIN_X_ADJ+MAIN_X+1	// 게임 정보 표시 위치 조정

int STATUS_Y_GOAL;	// GOAL 정보 표시 위치 Y 좌표
int STATUS_Y_LEVEL;	// LEVEL 정보 표시 위치 Y 좌표
int STATUS_Y_SCORE;	// SCORE 정보 표시 위치 Y 좌표

int blocks[7][4][4][4] = {{{0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0}},
	{{0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0}, {0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0}, {0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0}},
	{{0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0}, {0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0}},
	{{0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0}, {0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0}},
	{{0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0}},
	{{0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0}},
	{{0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0}, {0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0}}};

int b_type;			// 블록 종류
int b_rotation;		// 블록 회전값
int b_type_next;	// 다음 블록값
int b_x, b_y;		// 이동 중인 블록의 x, y 좌표

int main_org[MAIN_Y][MAIN_X];		// 게임판을 모니터에 표시 후 main_cpy로 복사
int main_cpy[MAIN_Y][MAIN_X];		// 즉, main_cpy는 이전의 정보를 가지고 있음
									// main_org와 main_cpy를 비교해서 값이 달라진 곳만 모니터에 수정

int key;	// 키보드 입력

int speed[] = { 0, 100, 50, 25, 10, 5, 4, 3, 2, 1, 0 };			// 게임 진행 속도
int level;														// 현재 level
int level_goal;													// 다음 레벨로 넘어가기 위한 목표 점수
int count;														// 현재 레벨에서 제거한 줄의 개수
int score = 0;													// 현재 점수
int best_score = 0;												// 최고 점수

bool new_block_on = false;	// 새로운 블록이 필요함을 알리는 flag
bool crush_on = false;		// 현재 이동 중인 블록이 충돌 상태인지 알리는 flag
bool level_up_on = false;	// 다음 레벨로의 진행을 알리는 flag
bool space_key_on = false;	// hard drop 상태임을 알리는 flag

int sock;

void title();										// 게임 시작 화면
void reset();										// 게임판 초기화
void reset_main_org();								// main_org 게임판 초기화
void reset_main_copy();								// main_cpy 게임판 초기화
void draw_interface();								// 게임 전체 인터페이스 표시
void draw_main_org();								// 게임판을 그림
void new_block();									// 새로운 블록을 만듦
void get_key();										// 키보드 입력받기
void drop_block();									// 블록을 아래로 떨어뜨림
int check_crush(int x, int y, int rotation);		// x, y 위치에 rotation 회전값을 갖는 경우 충돌 판단
void move_block(int key);							// key 방향으로 블록을 움직임
void check_line();									// 줄이 가득찼는지 확인 후 지움
void check_level_up();								// 레벨 목표가 성되었는지를 확인 후 레벨 업
void check_game_over();								// 게임 오버를 확인 후 게임 오버
void pause_game();									// 게임 일시정지
void quit_game();									// 게임 나가기

int main(int argc, char *argv[]) {
	int i;
	
	signal(SIGINT, INTHandler);

	sock = atoi(argv[1]);

	srand((unsigned)time(NULL));	// 난수 표 생성

	setlocale(LC_CTYPE, "ko_KR.utf-8");

	initscr();
	start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);
	init_pair(4, COLOR_BLUE, COLOR_BLACK);
	init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(6, COLOR_CYAN, COLOR_BLACK);
	init_pair(7, COLOR_WHITE, COLOR_BLACK);
	cbreak();
	noecho();
	keypad(stdscr, true);
	curs_set(false);
	flushinp();

	title();
	reset();

	while (true) {
		for (i = 0; i < 5; i++) {	// 블록이 한 칸 떨어지는 동안 5번 키 입력 가능
			get_key();
			draw_main_org();

			usleep(speed[level] * 1000);

			// 블록이 충돌 중인 경우 추가로 이동 및 회전할 시간을 가짐
			if (crush_on && !check_crush(b_x, b_y+1, b_rotation)) {
				usleep(50000);
			}

			// 스페이스바를 누른 경우(hard drop) 이동 및 회전 불가능
			if (space_key_on) {
				space_key_on = false;
				break;
			}
		}

		drop_block();
		check_level_up();
		check_game_over();

		if (new_block_on) {
			new_block();
		}
	}

	endwin();
	return 0;
}

void title() {
	int x = 5, y = 4, count;
	char buf[MAXLINE+1];
	fd_set fds;
	struct timeval tv;

	clear();
	
	mvprintw(y+0, x, "□□□ ■■ □□■■□□■■□□□□■■□□■ □■□□");
	refresh();
	usleep(30000);
	mvprintw(y+1, x, " □  ■■□□★■■□□■■     ■■□■■□■■□");
	refresh();
	usleep(30000);
	mvprintw(y+2, x, "■■□■□□■               □■□□ ■□");
	refresh();
	usleep(30000);
	mvprintw(y+3, x, "■□□■□■■□□■  □□  ■□  ■■■□■■■□ ");
	refresh();
	usleep(30000);
	mvprintw(y+4, x, "■□■■□ ■□□■■■□□■■■□□□■□□□■ □□□");
	refresh();
	usleep(30000);
	mvprintw(y+2, x+9, "T E T R I S");
	refresh();
	usleep(30000);
	mvprintw(y+5, x+12, "2020301054 soyeon");
	refresh();
	usleep(30000);
	mvprintw(y+7, x, "Please Enter Any Key to Start...");
	refresh();
	usleep(30000);
	mvprintw(y+9, x, "   △   : Shift");
	refresh();
	usleep(30000);
	mvprintw(y+10, x, "  ◁ ▷  : Left / Right");
	refresh();
	usleep(30000);
	mvprintw(y+11, x, "   ▽   : Soft Drop");
	refresh();
	usleep(30000);
	mvprintw(y+12, x, " SPACE : Hard Drop");
	refresh();
	usleep(30000);
	mvprintw(y+13, x, "   P   : Pause");
	refresh();
	usleep(30000);
	mvprintw(y+14, x, "  ESC  : Quit");
	refresh();
	usleep(30000);
	mvprintw(y+16, x, "BONUS FOR HARD DROPS / COMBOS");
	refresh();

	nodelay(stdscr, false);
	flushinp();

	for (count = 0; ; count++) {
		FD_ZERO(&fds);
		FD_SET(STDIN_FILENO, &fds);
		tv.tv_sec = 0;
		tv.tv_usec = 0;

		if (select(STDIN_FILENO+1, &fds, NULL, NULL, &tv) > 0) {	// 키 입력 시 게임 시작 (ESC 입력 시 프로그램 종료)
			if (FD_ISSET(STDIN_FILENO, &fds)) {
				key = getch();
				if (key == ESC) {
					quit_game();
				
					while (key = getch()) {
						if (key == 89 || key == 121) {	// Y or y 입력
							sprintf(buf, "%d", 0);
							write(sock, buf, strlen(buf));
							endwin();
							exit(0);
						} else if (key == 78 || key == 110) {	// N or n 입력
							title();
							break;
						}
					}
				} else {
					break;
				}
			}
		}

		y = 4;
		if (count % 200 == 0) {
			mvprintw(y+1, x+8, "★");
		} else if (count % 200 == 100) {
			mvprintw(y+1, x+8, " ");
		} else if (count % 350 == 0) {
			mvprintw(y+2, x+26, "☆");
		} else if (count % 350 == 100) {
			mvprintw(y+2, x+26, " ");
		}
		refresh();
		usleep(3000);
	}
}

void reset() {
	FILE *file = fopen("score.dat", "rt");	// score.dat 파일 연결
	if (file == 0) {
		best_score = 0;
	} else {
		fscanf(file, "%d", &best_score);	// 최고 점수 불러오기
		fclose(file);						// 파일 닫기
	}

	level = 1;
	score = 0;
	level_goal = 1000;
	key = 0;
	crush_on = false;
	count = 0;

	nodelay(stdscr, true);
	clear();	// 화면 지우기
	reset_main_org();
	draw_interface();
	draw_main_org();

	b_type_next = rand() % 7;	// 다음 블록 랜덤 생성
	new_block();
}

void reset_main_org() {
	int i, j;

	for (i = 0; i < MAIN_Y; i++) {	// 게임판 초기화
		for (j = 0; j < MAIN_X; j++) {
			main_org[i][j] = 0;
			main_cpy[i][j] = 100;
		}
	}

	for (i = 1; i < MAIN_X; i++) {		// 천장 만들기
		main_org[3][i] = CEILLING;
	}

	for (i = 1; i < MAIN_Y-1; i++) {	// 좌우 벽 만들기
		main_org[i][0] = WALL;
		main_org[i][MAIN_X-1] = WALL;
	}

	for (i = 0; i < MAIN_X; i++) {		// 바닥 벽 만들기
		main_org[MAIN_Y-1][i] = WALL;
	}
}

void reset_main_cpy() {
	int i, j;

	for (i = 0; i < MAIN_Y; i++) {
		for (j = 0; j < MAIN_X; j++) {
			main_cpy[i][j] = 100;
		}
	}
}

void draw_interface() {
	int y = 3;

	mvprintw(STATUS_Y_LEVEL = y, STATUS_X_ADJ, " LEVEL : %5d", level);
	mvprintw(STATUS_Y_GOAL = y+1, STATUS_X_ADJ, " GOAL  : %5d", 10-count);
	mvprintw(y+3, STATUS_X_ADJ, " +-  N E X T  -+ ");
	mvprintw(y+4, STATUS_X_ADJ, " |             | ");
	mvprintw(y+5, STATUS_X_ADJ, " |             | ");
	mvprintw(y+6, STATUS_X_ADJ, " |             | ");
	mvprintw(y+7, STATUS_X_ADJ, " |             | ");
	mvprintw(y+8, STATUS_X_ADJ, " +-- -  -  - --+ ");
	mvprintw(y+10, STATUS_X_ADJ, " YOUR SCORE : %6d", score);
	mvprintw(y+12, STATUS_X_ADJ, " BEST SCORE : %6d", best_score);
	mvprintw(y+15, STATUS_X_ADJ, "  △   : Shift         SPACE : Hard Drop");
	mvprintw(y+16, STATUS_X_ADJ, " ◁ ▷  : Left / Right    P   : Pause");
	mvprintw(y+17, STATUS_X_ADJ, "  ▽   : Soft Drop      ESC  : Quit");
	mvprintw(y+20, STATUS_X_ADJ, " 2020301054 soyeon");
}

void draw_main_org() {
	int i, j;

	for (i = 1; i < MAIN_X; i++) {	// 새로운 블록이 지나가서 천장이 지워지면 새로 그리기
		if (main_org[3][i] == EMPTY) {
			main_org[3][i] = CEILLING;
		}
	}

	for (i = 0; i < MAIN_Y; i++) {
		for (j = 0; j < MAIN_X; j++) {
			if (main_cpy[i][j] != main_org[i][j]) {
				switch (main_org[i][j]) {
					case EMPTY:				// 빈 칸
						mvprintw(MAIN_Y_ADJ+i, MAIN_X_ADJ+j, " ");
						break;
					case CEILLING:			// 천장
						mvprintw(MAIN_Y_ADJ+i, MAIN_X_ADJ+j, ".");
						break;
					case WALL:				// 벽
						mvprintw(MAIN_Y_ADJ+i, MAIN_X_ADJ+j, "▩");
						break;
					case INACTIVE_BLOCK:	// 떨어진 블록
						mvprintw(MAIN_Y_ADJ+i, MAIN_X_ADJ+j, "□");
						break;
					case ACTIVE_BLOCK:		// 움직이는 블록
						attron(COLOR_PAIR(b_type+1));
						mvprintw(MAIN_Y_ADJ+i, MAIN_X_ADJ+j, "■");
						attroff(COLOR_PAIR(b_type+1));
						break;
				}
			}
		}
	}
	refresh();

	for (i = 0; i < MAIN_Y; i++) {
		for (j = 0; j < MAIN_X; j++) {
			main_cpy[i][j] = main_org[i][j];
		}
	}
}

void new_block() {
	int i, j;

	b_x = MAIN_X / 2 - 1;
	b_y = 0;
	b_type = b_type_next;
	b_type_next = rand() % 7;
	b_rotation = 0;

	new_block_on = false;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			if (blocks[b_type][b_rotation][i][j] == 1) {
				main_org[b_y+i][b_x+j] = ACTIVE_BLOCK;
			}
		}
	}
	
	// 다음에 나올 블록을 그림
	for (i = 1; i < 3; i++) {
		for (j = 0; j < 4; j++) {
			if (blocks[b_type_next][0][i][j] == 1) {
				attron(COLOR_PAIR(b_type_next+1));
				mvprintw(i+7, STATUS_X_ADJ+7+j, "■");
				attroff(COLOR_PAIR(b_type_next+1));
			} else {
				mvprintw(i+7, STATUS_X_ADJ+7+j, " ");
			}
		}
	}
	refresh();
}

void get_key() {
	int i, j, x = 5, y = 7;
	char buf[MAXLINE+1];

	key = 0;
	if (key = getch()) {
		flushinp();

		switch (key) {
			case KEY_LEFT:
				if (check_crush(b_x-1, b_y, b_rotation)) {
					move_block(key);
				}
				break;
			case KEY_RIGHT:
				if (check_crush(b_x+1, b_y, b_rotation)) {
					move_block(key);
				}
				break;
			case KEY_DOWN:
				if (check_crush(b_x, b_y+1, b_rotation)) {
					move_block(key);
				}
				break;
			case KEY_UP:
				if (check_crush(b_x, b_y, (b_rotation+1)%4)) {
					move_block(key);
				} else if (crush_on && check_crush(b_x, b_y-1, (b_rotation+1)%4)) {
					move_block(SPECIAL);
				}
				break;
			case SPACE:
				space_key_on = true;
				while (!crush_on) {	// 바닥에 닿을 때까지
					drop_block();
					score += level;	// hard drop 보너스
					mvprintw(13, STATUS_X_ADJ, " YOUR SCORE : %6d", score);
					refresh();
				}
				break;
			case P:
			case p:
				pause_game();
				break;
			case ESC:
				quit_game();
				while (key = getch()) {
					if (key == 89 || key == 121) {  // Y or y 입력
						sprintf(buf, "%d", score);
						write(sock, buf, strlen(buf));
						endwin();
						exit(0);
					} else if (key == 78 || key == 110) {   // N or n 입력
						clear();
						reset_main_cpy();
						draw_main_org();
						draw_interface();
						for (i = 1; i < 3; i++) {
							for (j = 0; j < 4; j++) {
								if (blocks[b_type_next][0][i][j] == 1) {
									attron(COLOR_PAIR(b_type_next+1));
									mvprintw(i+7, STATUS_X_ADJ+7+j, "■");
									attroff(COLOR_PAIR(b_type_next+1));
								} else {
									mvprintw(i+7, STATUS_X_ADJ+7+j, " ");
								}
							}
						}
						refresh();
						break;
					}
				}
		}
	}
}

void drop_block() {
	int i, j;

	if (crush_on) {
		if (check_crush(b_x, b_y+1, b_rotation)) {
			crush_on = false;
		} else {
			for (i = 0; i < MAIN_Y; i++) {
				for (j = 0; j < MAIN_X; j++) {
					if (main_org[i][j] == ACTIVE_BLOCK) {
						main_org[i][j] = INACTIVE_BLOCK;
					}
				}
			}
			crush_on = false;
			check_line();
			new_block_on = true;
			return;
		}
	}

	if (check_crush(b_x, b_y+1, b_rotation)) {
		move_block(KEY_DOWN);
	} else {
		crush_on = true;
	}
}

int check_crush(int x, int y, int rotation) {
	int i, j;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			if (blocks[b_type][rotation][i][j] == 1 && main_org[y+i][x+j] > 0) {
				return false;
			}
		}
	}

	return true;
}

void move_block(int key) {
	int i, j;

	switch (key) {
		case KEY_LEFT:
			for (i = 0; i < 4; i++) {
				for (j = 0; j < 4; j++) {
					if (blocks[b_type][b_rotation][i][j] == 1) {
						main_org[b_y+i][b_x+j] = EMPTY;
					}
				}
			}
			for (i = 0; i < 4; i++) {
				for (j = 0; j < 4; j++) {
					if (blocks[b_type][b_rotation][i][j] == 1) {
						main_org[b_y+i][b_x+j-1] = ACTIVE_BLOCK;
					}
				}
			}
			b_x--;
			break;
		case KEY_RIGHT:
			for (i = 0; i < 4; i++) {
				for (j = 0; j < 4; j++) {
					if (blocks[b_type][b_rotation][i][j] == 1) {
						main_org[b_y+i][b_x+j] = EMPTY;
					}
				}
			}
			for (i = 0; i < 4; i++) {
				for (j = 0; j < 4; j++) {
					if (blocks[b_type][b_rotation][i][j] == 1) {
						main_org[b_y+i][b_x+j+1] = ACTIVE_BLOCK;
					}
				}
			}
			b_x++;
			break;
		case KEY_DOWN:
			for (i = 0; i < 4; i++) {
				for (j = 0; j < 4; j++) {
					if (blocks[b_type][b_rotation][i][j] == 1) {
						main_org[b_y+i][b_x+j] = EMPTY;
					}
				}
			}
			for (i = 0; i < 4; i++) {
				for (j = 0; j < 4; j++) {
					if (blocks[b_type][b_rotation][i][j] == 1) {
						main_org[b_y+i+1][b_x+j] = ACTIVE_BLOCK;
					}
				}
			}
			b_y++;
			break;
		case KEY_UP:
			for (i = 0; i < 4; i++) {
				for (j = 0; j < 4; j++) {
					if (blocks[b_type][b_rotation][i][j] == 1) {
						main_org[b_y+i][b_x+j] = EMPTY;
					}
				}
			}
			b_rotation = (b_rotation + 1) % 4;	// 회전값을 1 증가
			for (i = 0; i < 4; i++) {
				for (j = 0; j < 4; j++) {
					if (blocks[b_type][b_rotation][i][j] == 1) {
						main_org[b_y+i][b_x+j] = ACTIVE_BLOCK;
					}
				}
			}
			break;
		case SPECIAL:
			for (i = 0; i < 4; i++) {
				for (j = 0; j < 4; j++) {
					if (blocks[b_type][b_rotation][i][j] == 1) {
						main_org[b_y+i][b_x+j] = EMPTY;
					}
				}
			}
			b_rotation = (b_rotation + 1) % 4;
			for (i = 0; i < 4; i++) {
				for (j = 0; j < 4; j++) {
					if (blocks[b_type][b_rotation][i][j] == 1) {
						main_org[b_y+i-1][b_x+j] = ACTIVE_BLOCK;
					}
				}
			}
			b_y--;
			break;
	}
}

void check_line() {
	int i, j, k, l;
	int block_amount;	// 한 줄의 블록 개수
	int combo = 0;		// 콤보 개수

	for (i = MAIN_Y-2; i > 3;) {
		block_amount = 0;
		for (j = 1; j < MAIN_X-1; j++) {
			if (main_org[i][j] > 0) {
				block_amount++;
			}
		}
		if (block_amount == MAIN_X-2) {
			if (!level_up_on) {
				score += 100 * level;
				count++;
				combo++;
			}
			for (k = i; k > 1; k--) {
				for (l = 1; l < MAIN_X-1; l++) {
					if (main_org[k-1][l] != CEILLING) {
						main_org[k][l] = main_org[k-1][l];
					} else {
						main_org[k][l] = EMPTY;
					}
				}
			}
		} else {
			i--;
		}
	}

	if (combo > 1) {
		mvprintw(MAIN_Y_ADJ+b_y-2, MAIN_X_ADJ+MAIN_X/2-4, "%d COMBO!", combo);
		refresh();
		usleep(500000);
		score += combo * level * 100;
		mvprintw(STATUS_Y_GOAL, STATUS_X_ADJ, " GOAL  : %5d", (count <= 10) ? 10-count : 0);
		mvprintw(13, STATUS_X_ADJ, " YOUR SCORE : %6d", score);
		reset_main_cpy();	// 텍스트를 지우기 위해 main_cpy를 초기화
							// main_org와 main_cpy가 전부 다르므로 다음 번 draw() 호출 시 게임판 전체를 새로 그림
		refresh();
	}
}

void check_level_up() {
	int i, j;

	if (count >= 10) {
		draw_main_org();
		level_up_on = true;
		level += 1;
		count = 0;

		for (i = 0; i < 4; i++) {
			mvprintw(MAIN_Y_ADJ+5, MAIN_X_ADJ+MAIN_X/2-5, "             ");
			mvprintw(MAIN_Y_ADJ+7, MAIN_X_ADJ+MAIN_X/2-5, "             ");
			refresh();
			usleep(100000);

			mvprintw(MAIN_Y_ADJ+5, MAIN_X_ADJ+MAIN_X/2-5, "☆LEVEL UP!☆");
			mvprintw(MAIN_Y_ADJ+7, MAIN_X_ADJ+MAIN_X/2-5, "☆SPEED UP!☆");
			refresh();
			usleep(100000);
		}

		reset_main_cpy();

		for (i = MAIN_Y-2; i > MAIN_Y-level-1; i--) {
			for (j = 1; j < MAIN_X-1; j++) {
				main_org[i][j] = INACTIVE_BLOCK;
				mvprintw(MAIN_Y_ADJ+i, MAIN_X_ADJ+j, "★");
				refresh();
				usleep(20000);
			}
		}
		usleep(100000);	// 별 찍은 것을 보여주기 위한 delay
		check_line();	// 블록으로 모두 채운 것 지우기
		level_up_on = false;

		mvprintw(STATUS_Y_LEVEL, STATUS_X_ADJ, " LEVEL : %5d", level);
		mvprintw(STATUS_Y_GOAL, STATUS_X_ADJ, " GOAL  : %5d", 10-count);
		refresh();
	}
}

void check_game_over() {
	int i;
	int x = 7, y = 7;
	char buf[MAXLINE+1];

	for (i = 1; i < MAIN_X-2; i++) {
		if (main_org[3][i] > 0) {
			mvprintw(y+0, x, "▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤");
			mvprintw(y+1, x, "▤                              ▤");
			mvprintw(y+2, x, "▤  +------------------------+  ▤");
			mvprintw(y+3, x, "▤  |   G A M E  O V E R..   |  ▤");
			mvprintw(y+4, x, "▤  +------------------------+  ▤");
			mvprintw(y+5, x, "▤      YOUR SCORE: %6d      ▤", score);
			mvprintw(y+6, x, "▤                              ▤");
			mvprintw(y+7, x, "▤   Press any key to exit...   ▤");
			mvprintw(y+8, x, "▤                              ▤");
			mvprintw(y+9, x, "▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤");
			
			if (score > best_score) {
				FILE *file = fopen("score.dat", "wt");

				mvprintw(y+6, x, "▤     ★★★ BEST SCORE! ★★★      ▤");

				if (file == 0) {
					mvprintw(0, 0, "FILE ERROR: SYSTEM CANNOT WRITE BEST SCORE ON \"SCORE.DAT\"");
				} else {
					fprintf(file, "%d", score);
					fclose(file);
				}
			}

			sprintf(buf, "%d", score);
			write(sock, buf, strlen(buf));

			sleep(1);

			nodelay(stdscr, false);
			flushinp();
			getch();
			endwin();
			exit(0);
		}
	}
}

void pause_game() {
	int i, j;
	int x = 7, y = 7;

	mvprintw(y+0, x, "▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤");
	mvprintw(y+1, x, "▤                             ▤");
	mvprintw(y+2, x, "▤  +-----------------------+  ▤");
	mvprintw(y+3, x, "▤  |       P A U S E       |  ▤");
	mvprintw(y+4, x, "▤  +-----------------------+  ▤");
	mvprintw(y+5, x, "▤  Press any key to resume..  ▤");
	mvprintw(y+6, x, "▤                             ▤");
	mvprintw(y+7, x, "▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤");
	
	nodelay(stdscr, false);
	getch();
	nodelay(stdscr, true);

	clear();
	reset_main_cpy();
	draw_main_org();
	draw_interface();

	for (i = 1; i < 3; i++) {
		for (j = 0; j < 4; j++) {
			if (blocks[b_type_next][0][i][j] == 1) {
				attron(COLOR_PAIR(b_type_next+1));
				mvprintw(i+7, STATUS_X_ADJ+7+j, "■");
				attroff(COLOR_PAIR(b_type_next+1));
			} else {
				mvprintw(i+7, STATUS_X_ADJ+7+j, " ");
			}
		}
	}
	refresh();
}

void quit_game() {
	int x = 7, y = 7;

	mvprintw(y+0, x, "▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤");
	mvprintw(y+1, x, "▤                                   ▤");
	mvprintw(y+2, x, "▤  +-----------------------------+  ▤");
	mvprintw(y+3, x, "▤  | DO YOU REALLY WANT TO EXIT? |  ▤");
	mvprintw(y+4, x, "▤  +-----------------------------+  ▤");
	mvprintw(y+5, x, "▤                                   ▤");
	mvprintw(y+6, x, "▤        Y: EXIT    N: CANCEL       ▤");
	mvprintw(y+7, x, "▤                                   ▤");
	mvprintw(y+8, x, "▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤");
}
