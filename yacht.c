#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32 // 크로스플랫폼 지원
#include <windows.h> 
#include <conio.h>
void clearScreen() {
	system("cls");
}

void delay(int milliseconds) {
	Sleep(milliseconds);
}
#else 
#include <unistd.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

void enableRawMode() {
	struct termios raw;
	tcgetattr(STDIN_FILENO, &raw);
	raw.c_lflag &= ~(ECHO | ICANON);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disableRawMode() {
	struct termios raw;
	tcgetattr(STDIN_FILENO, &raw);
	raw.c_lflag |= (ECHO | ICANON);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
void clearScreen() {
	printf("\033[2J\033[H");
}

void delay(int milliseconds) {
	usleep(milliseconds * 1000);
}
#endif
typedef struct player {
	int score;
	char name[100];
	int listArr[13];// = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
} player;
char getch_s() {
	char buf = 0;
#ifdef _WIN32
	buf = _getch();
#else
	struct termios old = { 0 };
	fflush(stdout);
	if (tcgetattr(0, &old) < 0)
		perror("tcsetattr()");
	old.c_lflag &= ~ICANON;
	old.c_lflag &= ~ECHO;
	old.c_cc[VMIN] = 1;
	old.c_cc[VTIME] = 0;
	if (tcsetattr(0, TCSANOW, &old) < 0)
		perror("tcsetattr ICANON");
	if (read(0, &buf, 1) < 0)
		perror("read()");
	old.c_lflag |= ICANON;
	old.c_lflag |= ECHO;
	if (tcsetattr(0, TCSADRAIN, &old) < 0)
		perror("tcsetattr ~ICANON");
#endif
	return (buf);
}
int parseInt(char a) {
	if (a == '1') {
		return 1;
	}
	else if (a == '2') {
		return 2;
	}
	else if (a == '3') {
		return 3;
	}
	else if (a == '4') {

		return 4;
	}
	else if (a == '5') {
		return 5;
	}
	else if (a == '6') {
		return 6;
	}
	else if (a == '7') {
		return 7;
	}
	else if (a == '8') {
		return 8;
	}
	else if (a == '9') {
		return 9;
	}
	else {
		return 0;
	}
}
int dice(int max) {
	srand(time(NULL));
	int rd = (rand() % max) + 1;
	return rd;
}
void clearBuffer() {
	int ch;
	while ((ch = getchar()) != '\n' && ch != EOF); // 입력 버퍼 비우기
}
int findIndex(int test, int* arr) {
	int length = 5;
	for (int i = 0; i < length; i++) {
		if (*(arr + i) == test) {
			return i;
		}
	}
	return -1;
}
int countIndex(int test, int* arr) {
	int length = 5;
	int count = 0;
	for (int i = 0; i < length; i++) {
		if (*(arr + i) == test) {
			count++;
		}
	}
	return count;
}
/*"Once", "Twos", "Threes", "Fours", "Fives", "Sixes", "Four of a kind", "Full House", "Little Straight", "Big Straight", "Yacht", "Choice", "BONUS";*/
int diceCheck(int arr[5], int listArr[13], int * scoreTemp) {
	int canCase = 0;
	int exs = 0;
	int fh = 0, foak = 0;
	for (int i = 1; i < 7; i++) {
		for (int j = 1; j < 7; j++) {
			if (countIndex(i, arr) == 1 && countIndex(j, arr) == 4) {
				foak = j * 4;
			}
			else if (countIndex(i, arr) == 2 && countIndex(j, arr) == 3) {
				fh = (i * 2) + (j * 3);
			}
		}
	}
	if ((arr[0] == arr[1] && arr[1] == arr[2] && arr[2] == arr[3] && arr[3] == arr[4]) && listArr[10]) { // yacht
		canCase++;
		*(scoreTemp + 10) = 50;
	}
	else if ((findIndex(2, arr) + 1) && (findIndex(3, arr) + 1) && (findIndex(4, arr) + 1) && (findIndex(5, arr) + 1) && (findIndex(6, arr) + 1) && listArr[9]) { // Big Straight
		canCase++;
		*(scoreTemp + 9) = 30;
	}
	else if ((findIndex(2, arr) + 1) && (findIndex(3, arr) + 1) && (findIndex(4, arr) + 1) && (findIndex(5, arr) + 1) && (findIndex(1, arr) + 1) && listArr[8]) { // Small Straight
		canCase++;
		*(scoreTemp + 8) = 30;
	}
	else if (fh && listArr[7]) { // Full House
		canCase++;
		*(scoreTemp + 7) = fh;
	}
	else if (foak && listArr[6]) { // Four of a kind
		canCase++;
		*(scoreTemp + 6) = foak;
	}
	for (int i = 0; i < 6; i++) {
		if (listArr[i]) {
			canCase++;
			
			*(scoreTemp + i) = countIndex(i + 1, arr) * (i + 1);
		}
	}
	if (listArr[11]) {
		canCase++;
		*(scoreTemp + 11) = (arr[0] + arr[1] + arr[2] + arr[3] + arr[4]);
	}

	return (exs * 10) + canCase;
}

int main() {
	int diceFive[5];
	static char list[13][100] = { "Once", "Twos", "Threes", "Fours", "Fives", "Sixes", "Four of a kind", "Full House", "Little Straight", "Big Straight", "Yacht", "Choice", "BONUS" };
	int part;
	printf("Hello, everyone ! Nice to meet you. I'm a MC of this game, please call me Silvia.\nHow many participants are there? : ");
	scanf("%d", &part);
	player * arr = (player*)malloc(part * sizeof(player));
	if (arr == NULL) {
		printf("메모리 할당 실패\n");
		return 0;
	}
	for (int i = 0; i < part; i++) {
		(arr + i)->score = 0;
		printf("Hi, %dp.What is your name ? : ", i + 1);
		scanf("%s", (arr + i)->name);
		printf("Hello, %s.Welcome ! \n", (arr + i)->name);
		for (int j = 0; j < 13; j++) {
			arr[i].listArr[j] = 1;
		}
	}

	delay(1000);
	clearScreen();
	printf("T");
	delay(1000);
	printf("h");
	delay(1000);
	printf("e");
	delay(1000);
	printf("n...");
	delay(1000);
	for (int tot = 1; tot <= 5; tot++) {
		for (int tt = 0; tt < part; tt++) {
			printf("\nLet's roll the dice ! %s, Are you ready ?\nThis is Your turn %d!\nIf you're ready, press any key!", (arr + tt)->name, tot);
			
			char key;
			int diceMAx = 6;
			clearBuffer();
			key = getch_s();
			
			printf("\nThe result...\n");
			int scoreTemp[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			for (int i = 0; i < 5; i++) {
				delay(1000);
				*(diceFive + i) = dice(diceMAx);
				printf("%d  ", *(diceFive + i));
			}
			printf("\nYou can choose that... : \nCount of %d case(s)", diceCheck(diceFive, (arr + tt)->listArr, scoreTemp));
			printf("Avilable : \n");
			int cc = 0;
			int choice[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			for (int i = 0; i < 13; i++) {
				//printf("%d : list : %d, scoreTemp : %d, ");
			}
			for (int i = 0; i < 13; i++) {
				if ((scoreTemp[i] || i <= 5) && ((arr + tt)->listArr[i])) {
					cc++;
					printf("%d : %s (Expect score : %d)\n", cc, list[i], scoreTemp[i]);
					choice[cc] = i;
				}
			}
			cc++;
			printf("%d : Give up (Expect score : 0)\nWhat is your choice ? ", cc);
			char cho;
			scanf("%c", &cho);
			
			if (parseInt(cho) != cc) {
				arr[tt].listArr[choice[parseInt(cho)]] = 0;
				arr[tt].score += scoreTemp[choice[parseInt(cho)]];
				printf("\n%d points get ! (now %dp)", scoreTemp[choice[parseInt(cho)]], (arr + tt)->score);
			}
			else {
				printf("You gave up to get point...");
			}
			delay(2000);
			clearScreen();
		}
	}
		printf("=====Hall of fame=====\n");
	for (int i = 0; i < part; i++) {
		printf("%d. Name : %s, Score : %d\n", i + 1, (arr + i)->name, (arr + i)->score);
	}
	printf("=========================");
	free(arr);
	return 0;
}
