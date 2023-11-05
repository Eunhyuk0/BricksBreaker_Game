/*
* Created By Koh Eunhyuk
* A Game for personal practice, based on Atari Breakout
* May 2023 ~ Sep 2023
* 제작: 고은혁
* 개인적인 연습을 위한 게임 프로그램으로, Atari Breakout 을 기반으로 함
* 2023.05 ~ 2023.09
*/

//라이브러리
#include <stdio.h>
#include <conio.h> //키 입력에 사용
#include <windows.h> //커서 좌표/색/표시 여부 등에 사용
#include <time.h> //srand() 에 사용
#include <stdlib.h> //랜덤에 사용

//전처리 상수

//오류가 생기지 않는 x, y 최댓값
#define Xmax 117
#define Ymax 29
//플레이트 관련
#define PlateY 27 //플레이트가 있는 y좌표
#define DefaultPlatePos 55 //플레이트 왼쪽 끝 (startpos) 의 기본값
//키 입력 관련
//화살표 키 ASCII 값 (2번째 자리)
#define UP 72
#define DOWN 80
#define LEFT 75
#define RIGHT 77
#define FirstKeyValue 224 //화살표 등 2자리 키의 1번째 자리 ASCII 값
#define ENTER 13 //ENTER ASCII 값
//벽 판단 기준 (BottomWall 제외 실제로는 벽보다 1칸 안쪽)
#define LeftWall 3 //왼쪽 벽 x값
#define RightWall 114 //오른쪽 벽 x값
#define CeilingWall 1 //위쪽 벽 y값
#define BottomWall 29 //아래쪽 벽 (볼 놓치는 상황 판단에 사용) y값
//브릭 관련
#define ColumnsStartingPoint 2 //브릭 세로줄 시작 y 좌표
#define ColumnBricks 8 //브릭 세로 개수
#define RowBricks 28 //브릭 가로 개수
//기타
#define BallDiagSleep 5 //BallDir 2,4,8,10 방향에 사용되는 추가적인 딜레이
#define MidX 58 //중앙 x값
#define MidY 14 //중앙 y값

//windows.h 의 STD_HANDLE 을 원형으로 하는 함수들
COORD getCursor(void); //커서 좌표 구하는 함수
void gotoxy(int x, int y); //커서를 입력한 좌표로 이동시키는 함수
int SetColour(int text, int bg);
void HideCursor(); //커서 표시 숨기는 함수 (토글)
void ShowCursors(); //커서 표시 보이게 하는 함수 (토글) 

//함수
int TitleScreen(); //타이틀 화면
int GuideScreen(); //how to play 동작 시 실행
int InfoScreen(); //Info 동작 시 실행 - 간단한 정보
int OptionsScreen(int * Option); //Options 동작 시 실행 - 목숨 개수, 플레이트 타입 설정 가능
int BasicTemplateDisplay(); //기본 틀 출력
int Game(int* Options); //게임
int GameOverScreen(int score, int* Options); //게임오버 화면
int ClearScreen(int* Options); //클리어 화면
int ExperimentMode(int* PlatePos, int* Options); //실험 모드 (현재는 실행 부분을 주석처리)
int PlateDisplay(int* plate, int startpos, int previousstartpos, int platetype); //플레이트 출력
COORD Ball(COORD oldBP, int* BallDir, int* PlatePos, int* brickcrashPT); //볼 출력 및 방향 변경
void BallDisplay(int x, int y); //볼을 입력한 좌표에 출력
COORD BallPosSet(COORD oldBP, int x, int y); //BallDir 상관없이 볼을 새로운 좌표로 출력 (초기화에 사용)
int Bricks(COORD BallPos, int* Balldir); //브릭 처리 및 BallDir 변경, 브릭 파괴 시 1 리턴
int BricksDisplay(); //Bricksarr[][] 을 반영해 브릭 출력
int BricksSet(); //브릭 초기화 (출력 및 Bricksarr[][] 값을 모두 1로)
int RandomTwoWProb(int A, int B, int prob); //2개 경우 중 A를 높은 확률로 리턴

//Global Variables

int Bricksarr[8][28]; //브릭 상태를 저장하는 배열, 1: 있음 / 0: 파괴됨

void main() { //main() 은 중요한 기능은 없으나 TitleScreen() 을 실행
	srand(time(NULL)); //랜덤을 사용하기 위한 준비
	HideCursor(); //커서 숨김 (토글)
	TitleScreen();
}

int Game(int* Options) { //게임
//변수
	//기타
	int Gamekey; //입력받는 키 값
	const int timervalue = 8; //반복 타이머에 사용하는 상수 값 (실험으로 찾은 값)
	int stocks = Options[1]; //목숨 개수, OptionsScreen() 에서 설정한 값으로 사용
	int score = 0; //점수
	int* scorePT; //score에 접근하기 위한 pointer variable
	scorePT = &score;


	//플레이트 관련
	int plate[7] = { 0, 0, 0, 0, 0, 0, 0 }; //플레이트의 각 x값, 처음엔 0으로 초기화함
	int currentPlatePos = DefaultPlatePos; //PlateDisplay()가 리턴하는 플레이트 왼쪽 끝 x좌표
	int* PlatePT; //plate[] 에 접근하기 위한 pointer variable
	PlatePT = &plate[0];

	//볼 관련
	COORD BallPos; //볼 좌표
	BallPos.X = MidX; //초기화는 중앙 x,y 로
	BallPos.Y = MidY;
	int BallDirection = 6; //볼 방향, 초기화는 6(하강) 으로
	int* BallDirPT = &BallDirection; //BallDirection 에 접근하기 위한 pointer variable

	//브릭 관련
	int brickcrash = 0; //브릭 파괴 여부, Bricks() 리턴 값을 저장함
	int* brickcrashPT; //brickcrash 에 접근하기 위한 pointer variable
	brickcrashPT = &brickcrash;
	int leftbricks; //남은 브릭 개수, 클리어 판단에 사용



//초기화
	system("cls"); //화면 리셋 후 틀 출력
	BasicTemplateDisplay();
	//볼과 플레이트 위치 초기화
	currentPlatePos = PlateDisplay(PlatePT, DefaultPlatePos, DefaultPlatePos, Options[0]);
	BallPosSet(BallPos, MidX, MidY);
	//남은 목숨 개수와 점수 최초 표시
	gotoxy(2, 28);
	printf(" O: %d", stocks - 1);
	gotoxy(112, 28);
	printf("%d", score);
	BricksSet(); //브릭 초기화

//반복
	for (int timerflag = 0;1;) { //무한반복
		Sleep(1); //반복에 소요되는 시간을 일정하게 하기 위함
		timerflag++; //반복 시마다 증가하는 값, 타이머에 사용됨

		//일정 시간마다 실행
		if (timerflag % timervalue == 0) //나누어떨어지는 경우에만
		{
			BallPos = Ball(BallPos, BallDirPT, PlatePT, brickcrashPT); //볼 이동
			currentPlatePos = PlateDisplay(PlatePT, currentPlatePos, currentPlatePos, Options[0]); //볼이 플레이트에 닿으면 일부가 지워지므로 다시 출력

			brickcrash = Bricks(BallPos, BallDirPT); //브릭은 볼이 움직일 때만 변하므로 여기서 확인
			if (brickcrash == 1)
				score++; //브릭 파괴시 점수 증가

			gotoxy(112, 28);
			printf("%d", score); //점수 표시 업데이트

			//남은 브릭 수 확인
			leftbricks = 0;
			for (int c = 0; c < ColumnBricks; c++) { 
				for (int r = 0; r < RowBricks; r++) {
					if (Bricksarr[c][r] == 1) {
						leftbricks++;
					}
				}
			}
			//클리어 판단
			if (leftbricks < 1) {
				ClearScreen(Options);
				break;
			}

		}

		if (BallPos.X == 0 && BallPos.Y == 0) { //볼을 놓친 상황이라면
			stocks--; //목숨 감소
			if (stocks == 0) { //게임오버 판단
				GameOverScreen(score, Options);
				break;
			}
			else { //목숨이 남아 있으면 볼, 플레이트 초기화 후 안내 문구 출력
				currentPlatePos = PlateDisplay(PlatePT, DefaultPlatePos, currentPlatePos, Options[0]);
				gotoxy(2, 28);
				printf("                        ");
				gotoxy(2, 28);
				printf(" O: %d", stocks - 1);   //목숨 개수 업데이트
				BallPos = BallPosSet(BallPos, MidX, MidY);
				//안내 문구가 3회 깜빡이게 함
				for (int i = 0; i < 3; i++) {
					Sleep(350);
					if (stocks == 1) {
						gotoxy(MidX - 5, 17);
						printf("LAST CHANCE"); //마지막 1개일 때 나올 문구
					}
					else {
						gotoxy(MidX - 4, 17);
						printf("CONTINUE");
					}

					Sleep(350);
					gotoxy(MidX - 6, 17);
					printf("                      ");
				}
			}
		}

		if (_kbhit() == 1) { //키보드가 눌렸다면
			Gamekey = _getch();  //키 입력 받기
			if (Gamekey == FirstKeyValue) {
				Gamekey = _getch();
				switch (Gamekey) { //2자리일 경우 다시 입력 받아 실행
				case LEFT:
					currentPlatePos = PlateDisplay(PlatePT, currentPlatePos - 1, currentPlatePos, Options[0]); //??? ?? ? ?? ??
					break;
				case RIGHT:
					currentPlatePos = PlateDisplay(PlatePT, currentPlatePos + 1, currentPlatePos, Options[0]); //???? ?? ? ?? ??
					break;
				}
			}
			else {
				switch (Gamekey) {
				//1자리일 경우 ExperimentMode() 를 실행하는 0밖에 없었으나 현재는 주석처리
				/*
				case 48: //0 입력 시
					ExperimentMode(PlatePT, Options);
					break;
				*/
				}
			}
		}
	}

	//반복 종료 상황은 클리어 또는 게임오버
	SetColour(8, 16);
	gotoxy(MidX - 13, MidY + 7);
	printf("Press (ENTER) to return to Title"); //ENTER 로 타이틀 복귀
	SetColour(15, 16);
	for (int gamefinishkey;1;) {
		if (_kbhit() == 1) { //무한 반복으로 키 감지
			gamefinishkey = _getch();
			switch (gamefinishkey) {
			case ENTER: //enter 입력 시 Game() 종료
				return 0;
				break;
			}
		}
	}
}

int GameOverScreen(int score, int * Options) { //게임오버 화면, 문구와 점수, 처음 목숨 개수 출력
	system("cls");
	gotoxy(0, MidY - 6);
	printf("\t\t     =====       =      =     =   ======         ====   =       =  ======  =====     \n");
	printf("\t\t    =           = =     ==   ==   =             =    =   =     =   =       =    =    \n");
	printf("\t\t    =    ==    =====    = = = =   ======       =      =   =   =    ======  =====     \n");
	printf("\t\t    =     =   =     =   =  =  =   =             =    =     = =     =       =    =    \n");
	printf("\t\t     =====   =       =  =     =   ======         ====       =      ======  =     =   \n");
	gotoxy(MidX - 7, MidY + 2);
	printf("score: %d  /  %d", score, (RowBricks * ColumnBricks));
	gotoxy(MidX - 7, MidY + 3);
	printf("Initial Lives : %d", Options[1]);
	return 0;
}

int ClearScreen(int* Options) { //클리어 화면, 문구와 처음 목숨 개수 출력
	system("cls");
	gotoxy(0, MidY - 6);
	printf("\t\t\t        ======   =         =======      =       ======    \n");
	printf("\t\t\t       =         =         =           = =      =     =   \n");
	printf("\t\t\t       =         =         =======    =====     ======    \n");
	printf("\t\t\t       =         =         =         =     =    =     =   \n");
	printf("\t\t\t        ======   =======   =======  =       =   =      =  \n");

	gotoxy(MidX - 15, MidY + 2);
	printf("Initial Lives : %d", Options[1]);
	gotoxy(MidX - 15, MidY + 4);
	printf("You won the game. Try with less lives!");
	return 0;
}

COORD Ball(COORD oldBP, int* BallDir, int* PlatePos, int* brickcrashPT) { //볼 이동 및 방향 변경, 손실 시 (0, 0) 리턴
	SetColour(15, 16); //다른 색이 들어가지 않도록 함

	//oldBP 위치로 이동해 이전에 출력한 볼 지우기
	gotoxy(oldBP.X, oldBP.Y);
	printf(" ");

	COORD newBP; //새로운 볼 좌표
	//플레이트에 볼이 닿았는지 감지하기 위해 왼쪽 끝/오른쪽 끝 좌표 저장
	int PlateL = *PlatePos;
	int PlateR = PlatePos[6];
	int Randomvalue; //랜덤 값 저장

	switch (*BallDir) { //BallDir을 기준으로 newBP 설정
	//10, 2, 4, 8 방향은 속도가 빨라 딜레이를 넣음
	case 10:
		newBP.Y = (oldBP.Y - 1);
		newBP.X = (oldBP.X - 2);
		Sleep(BallDiagSleep);
		break;
	case 11:
		newBP.Y = (oldBP.Y - 1);
		newBP.X = (oldBP.X - 1);
		break;
	case 12:
		newBP.Y = (oldBP.Y - 1);
		newBP.X = oldBP.X;
		break;
	case 1:
		newBP.Y = (oldBP.Y - 1);
		newBP.X = (oldBP.X + 1);
		break;
	case 2:
		newBP.Y = (oldBP.Y - 1);
		newBP.X = (oldBP.X + 2);
		Sleep(BallDiagSleep);
		break;
	case 3:
		newBP.Y = oldBP.Y;
		newBP.X = (oldBP.X + 1);
		break;
	case 4:
		newBP.Y = (oldBP.Y + 1);
		newBP.X = (oldBP.X + 1);
		Sleep(BallDiagSleep);
		break;
	case 5:
		newBP.Y = (oldBP.Y + 1);
		newBP.X = (oldBP.X + 2);
		break;
	case 6:
		newBP.Y = (oldBP.Y + 1); //기본 방향
		newBP.X = oldBP.X;
		break;
	case 7:
		newBP.Y = (oldBP.Y + 1);
		newBP.X = (oldBP.X - 1);
		break;
	case 8:
		newBP.Y = (oldBP.Y + 1);
		newBP.X = (oldBP.X - 2);
		Sleep(BallDiagSleep);
		break;
	case 9:
		newBP.Y = oldBP.Y;
		newBP.X = (oldBP.X - 1);
		break;
	}

	if (3 <= *BallDir && *BallDir <= 9 && newBP.Y >= BottomWall) { //볼을 놓쳤을 경우 (0, 0) 리턴
		*BallDir = 6;
		newBP.X = 0;
		newBP.Y = 0;
		return newBP;
	}

	if (3 <= *BallDir && *BallDir <= 9 && newBP.Y >= PlateY && newBP.Y < BottomWall && PlateL <= newBP.X && newBP.X <= PlateR) {
		//볼이 플레이트에 닿았을 경우 방향 변경
		switch (*BallDir) {
		case 4:
			*BallDir = RandomTwoWProb(2, 1, 2);
			break;
		case 5:
			*BallDir = RandomTwoWProb(1, 2, 2);
			break;
		case 6:
			Randomvalue = ((rand() % 3) + 1); //상하 수직 움직임은 3방향 같은 확률로 변경
			if (Randomvalue == 2)
				Randomvalue = 12;
			if (Randomvalue == 3)
				Randomvalue = 11;
			*BallDir = Randomvalue;
			break;
		case 7:
			*BallDir = RandomTwoWProb(11, 10, 2);
			break;
		case 8:
			*BallDir = RandomTwoWProb(10, 11, 2);
			break;
		}
	}

	if (newBP.Y <= CeilingWall) {   //볼이 위쪽 벽에 닿았을 경우: 규칙으로 변경
		switch (*BallDir) {
		case 10:
			*BallDir = 8;
			break;
		case 11:
			*BallDir = 7;
			break;
		case 12:
			*BallDir = 6;
			break;
		case 1:
			*BallDir = 5;
			break;
		case 2:
			*BallDir = 4;
			break;
		}
	}
	if (newBP.X <= LeftWall) {  //왼쪽 벽에 닿았을 경우: 규칙으로 변경
		switch (*BallDir) {
		case 9:
			*BallDir = 3;
			break;
		case 8:
			*BallDir = 4;
			break;
		case 7:
			*BallDir = 5;
			break;
		case 10:
			*BallDir = 2;
			break;
		case 11:
			*BallDir = 1;
			break;

		}
	}
	if (newBP.X >= RightWall) { //오른쪽 벽에 닿았을 경우: 규칙으로 변경
		switch (*BallDir) {
		case 1:
			*BallDir = 11;
			break;
		case 2:
			*BallDir = 10;
			break;
		case 3:
			*BallDir = 9;
			break;
		case 4:
			*BallDir = 8;
			break;
		case 5:
			*BallDir = 7;
			break;
		}
	}


	BallDisplay(newBP.X, newBP.Y); //볼 출력
	//oldBP 업데이트 및 리턴
	oldBP.X = newBP.X;
	oldBP.Y = newBP.Y;

	return oldBP;
}

COORD BallPosSet(COORD oldBP, int x, int y) { //볼을 설정한 좌표로 이동, 초기화에 사용
	COORD SetBP; //볼을 출력할 좌표

	//이전에 출력된 볼 삭제
	gotoxy(oldBP.X, oldBP.Y);
	printf(" ");

	//입력한 좌표에 출력
	SetBP.X = x;
	SetBP.Y = y;
	BallDisplay(SetBP.X, SetBP.Y);

	return SetBP; //출력한 좌표 리턴
}

void BallDisplay(int x, int y) { //볼 출력
	SetColour(15, 16); //색 초기화
	gotoxy(x, y);
	printf("O");
}

int PlateDisplay(int* plate, int startpos, int previousstartpos, int platetype) { //플레이트를 설정한 위치로 옮김
	//plate[]: 왼쪽 끝부터 각 텍스트의 x값
	//startpos: 왼쪽 끝 텍스트의 x값
	//previousstartpos: 이전에 출력한 플레이트의 startpos

	//이전에 출력한 플레이트 삭제
	gotoxy(previousstartpos, PlateY);
	for (int cnt = 0; cnt < 7; cnt++) {
		printf(" ");
	}

	if (startpos + 6 >= 114) { //오른쪽 벽을 넘어가지 않도록 출력 좌표 고정해 리턴
		gotoxy(108, PlateY);
		if (platetype == 1) { //설정 시 배경색 적용
			SetColour(16, 15);
		}
		printf("_______");
		SetColour(15, 16); //색 초기화
		for (int cnt = 0; cnt < 7; cnt++) {
			plate[cnt] = 108 + cnt;
		}
		return 108;
	}
	else if (startpos <= 3) { //왼쪽 벽을 넘어가지 않도록 출력 좌표 고정해 리턴
		gotoxy(3, PlateY);
		if (platetype == 1) { //설정 시 배경색 적용
			SetColour(16, 15);
		}
		printf("_______");
		SetColour(15, 16); //색 초기화
		for (int cnt = 0; cnt < 7; cnt++) {
			plate[cnt] = 3 + cnt;
		}
		return 3;
	}
	else {  //이외 상황에서는 좌표 변경
		gotoxy(startpos, PlateY); //출력할 위치로 이동
		if (platetype == 1) { //설정 시 배경색 적용
			SetColour(16, 15);
		}
		printf("_______");
		SetColour(15, 16); //색 초기화
		for (int cnt = 0; cnt < 7; cnt++) {
			plate[cnt] = startpos + cnt;
		}

		//plate[] 확인을 위한 부분으로, 현재는 ExperimentMode() 에 있음
		//printf("\n%d %d %d %d %d %d %d", plate[0], plate[1], plate[2], plate[3], plate[4], plate[5], plate[6]);

		return startpos; //출력한 왼쪽 끝 좌표 리턴
	}
}

int Bricks(COORD BallPos, int* BallDir) { //브릭 처리 및 BallDir 변경, 브릭이 파괴되었다면 1 리턴

	int breakingbrickC; //볼과 닿은 브릭의 Column 값
	int breakingbrickR; //볼과 닿은 브릭의 Row 값

	int Randomvalue; //랜덤 값 저장을 위한 변수

	if (BallPos.Y <= (ColumnBricks + CeilingWall)) { //볼이 브릭 구간에 진입한 경우
		breakingbrickR = ((BallPos.X - LeftWall) / 4);
		breakingbrickC = (BallPos.Y - ColumnsStartingPoint);

		if (Bricksarr[breakingbrickC][breakingbrickR] == 1) { //브릭이 있는 자리였다면 브릭 파괴
			Bricksarr[breakingbrickC][breakingbrickR] = 0;

			switch (*BallDir) { //원래는 Ball() 에서 해야 하나 브릭이 더 깨지는 문제로 여기서 실행
				//3,6,9,12 는 3방향(같은 확률) 중, 나머지는 2방향(다른 확률) 랜덤
			case 6:
				Randomvalue = ((rand() % 3) + 1);
				if (Randomvalue == 1)
					Randomvalue = 12;
				if (Randomvalue == 2)
					Randomvalue = 1;
				if (Randomvalue == 3)
					Randomvalue = 11;
				*BallDir = Randomvalue;
				break;
			case 7:
				*BallDir = RandomTwoWProb(11, 10, 2);
				break;
			case 8:
				*BallDir = RandomTwoWProb(10, 11, 2);
				break;
			case 9:
				Randomvalue = ((rand() % 3) + 1);
				if (Randomvalue == 1)
					Randomvalue = 3;
				if (Randomvalue == 2)
					Randomvalue = 2;
				if (Randomvalue == 3)
					Randomvalue = 4;
				*BallDir = Randomvalue;
				break;
			case 10:
				*BallDir = RandomTwoWProb(8, 7, 2); //8
				break;
			case 11:
				*BallDir = RandomTwoWProb(7, 8, 2); //7
				break;
			case 12:
				Randomvalue = ((rand() % 3) + 1); //6
				if (Randomvalue == 1)
					Randomvalue = 5;
				if (Randomvalue == 2)
					Randomvalue = 6;
				if (Randomvalue == 3)
					Randomvalue = 4;
				*BallDir = Randomvalue;
				break;
			case 1:
				*BallDir = RandomTwoWProb(5, 4, 2); ///5
				break;
			case 2:
				*BallDir = RandomTwoWProb(4, 5, 2); ///4
				break;
			case 3:
				Randomvalue = ((rand() % 3) + 1);
				if (Randomvalue == 1)
					Randomvalue = 9;
				if (Randomvalue == 2)
					Randomvalue = 8;
				if (Randomvalue == 3)
					Randomvalue = 10;
				*BallDir = Randomvalue;
				break;
			case 4:
				*BallDir = RandomTwoWProb(2, 1, 2);
				break;
			case 5:
				*BallDir = RandomTwoWProb(1, 2, 2);
				break;
			}

			//볼이 브릭에 튕겨서 좌우 벽을 뚫지 않게 함
			if (BallPos.X >= 110) { //오른쪽 끝 브릭에 있다면
				*BallDir = RandomTwoWProb(8, 7, 2); //강제로 좌하향 시킴
			}
			if (BallPos.X <= 5) { //왼쪽 끝 브릭에 있다면
				*BallDir = RandomTwoWProb(4, 5, 2); //강제로 우하향 시킴
			}

			BricksDisplay(); //브릭 변화 반영해 출력


			return 1;
		}
	}
	return 0; //브릭이 파괴되지 않았다면 0 리턴
}

int BricksDisplay() {  //값 변화를 반영해 브릭 출력

	gotoxy(LeftWall, ColumnsStartingPoint);
	for (int columns = 0; columns < ColumnBricks; columns++) {
		gotoxy(LeftWall, (ColumnsStartingPoint + columns));
		for (int rows = 0; rows < RowBricks; rows++) {
			if (Bricksarr[columns][rows] == 1) { //해당 위치에 브릭이 있는지 확인
				if (columns % 2 == 0) {
					if (rows % 2 == 0) {  //1개마다 브릭 색을 바꾸기 위함
						SetColour(7, 7);
						printf("////");
					}
					else {
						SetColour(8, 8);
						printf("////");

					}
				}
				else {
					if (rows % 2 == 0) {  //1개마다 브릭 색을 바꾸기 위함
						SetColour(8, 8);
						printf("////");
					}
					else {
						SetColour(7, 7);
						printf("////");

					}

				}
			}
			else { //브릭이 없는 위치라면 공백 출력
				SetColour(15, 16);
				printf("    ");
			}

		}
	}

	SetColour(15, 16);

	//아래는 Bricksarr[][] 값 확인을 위해 넣음, ExperimentMode() 에서는 게임이 정지하므로 실시간 확인이 불가함
	/*
	printf("\n\n");
	for (int c = 0; c < ColumnBricks; c++) {
		for (int r = 0; r < RowBricks; r++) {
			printf("%d ", Bricksarr[c][r]);
		}
		printf("\n");
	}
	*/

	return 0;

}

int BricksSet() { //브릭 초기화
	int columns = 0;
	gotoxy(LeftWall, ColumnsStartingPoint);
	for (; columns < ColumnBricks; columns++) {
		gotoxy(LeftWall, (ColumnsStartingPoint + columns));
		for (int r = 0; r < RowBricks; r++) {
			if (columns % 2 == 0) {
				if (r % 2 == 0) {  //1개마다 브릭 색을 바꾸기 위함
					SetColour(7, 7);
					printf("////");
				}
				else {
					SetColour(8, 8);
					printf("////");
				}
			}
			else {
				if (r % 2 == 0) {  //1개마다 브릭 색을 바꾸기 위함
					SetColour(8, 8);
					printf("////");
				}
				else {
					SetColour(7, 7);
					printf("////");
				}
			}
		}
	}

	for (int c = 0; c < ColumnBricks; c++) {    //Bricksarr[][] 1로 초기화
		for (int r = 0; r < RowBricks; r++) {
			Bricksarr[c][r] = 1;
		}
	}


	SetColour(15, 16);
	return 0;
}

int RandomTwoWProb(int A, int B, int prob) { //두 가지 경우 중 A를 높은 확률로 리턴
	int randomvalue;
	randomvalue = ((rand() % (1 + prob)) + 1); //prob 값의 개수만큼은 A가 됨
	if (randomvalue > 1) {
		randomvalue = A;
	}
	else {
		randomvalue = B;
	}
	return randomvalue;
}

ExperimentMode(int* PlatePos, int* Options) { //실험 모드, 현재는 실행되지 않음
	/*
	* 화살표 입력 시 커서가 해당 방향으로 1칸씩 움직임
	* q,Q 입력 시 ExperimentMode 종료
	* space 입력 시 현재 커서 좌표 출력 (위 칸에 출력하므로 연속해 확인 가능)
	* p,P 입력시 플레이트 전체 좌표 출력
	* b,B 입력시 Bricksarr[][] 값 출력
	*/

	ShowCursors(); //커서 보이게 함

	COORD ExperimentPos = getCursor(); //현재 커서 좌표 저장
	int ExperimentKey; //키 값
	int Experimentquit = 0; //종료 여부, 1이 되면 반복을 종료해 함수 종료
	//위 칸에 안내 출력 후 원래 위치로 복귀
	gotoxy(0, 0);
	printf("Experiment Mode"); 
	gotoxy(ExperimentPos.X, ExperimentPos.Y);

	while (Experimentquit < 1) {
		//키 입력받음
		ExperimentKey = _getch();
		if (ExperimentKey == FirstKeyValue) {
			ExperimentKey = _getch();
			switch (ExperimentKey) { //화살표: 커서 1칸씩 이동
			case UP:
				ExperimentPos = getCursor();
				gotoxy(ExperimentPos.X, (ExperimentPos.Y) - 1);
				break;
			case DOWN:
				ExperimentPos = getCursor();
				gotoxy(ExperimentPos.X, (ExperimentPos.Y) + 1);
				break;
			case LEFT:
				ExperimentPos = getCursor();
				gotoxy((ExperimentPos.X) - 1, ExperimentPos.Y);
				break;
			case RIGHT:
				ExperimentPos = getCursor();
				gotoxy((ExperimentPos.X) + 1, ExperimentPos.Y);
				break;
			}
		}
		else {
			switch (ExperimentKey) {
			case 32: //space 입력 시
				ExperimentPos = getCursor();
				gotoxy(ExperimentPos.X, ExperimentPos.Y + 1);
				printf("%d, %d", ExperimentPos.X, ExperimentPos.Y); //? ?? ?? ?? ?? ? ?? ??? ??
				gotoxy(ExperimentPos.X, ExperimentPos.Y);
				break;
			case 113:
			case 81: //q,Q 입력 시
				Experimentquit++; //ExperimentMode() 종료
				break;
			case 80:
			case 112: //p,P 입력 시 plate[] 출력
				printf("\n");
				for (int cnt = 0; cnt < 7; cnt++) {
					printf("%d ", PlatePos[cnt]);
				}
				break;
			case 66:   //b,B 입력 시 Bricksarr[][] 출력
			case 98:
				gotoxy(4, 16);
				SetColour(15, 16);
				printf("\n");
				for (int c = 0; c < ColumnBricks; c++) {
					gotoxy(LeftWall, (14 + c));
					for (int r = 0; r < RowBricks; r++) {
						printf("%d", Bricksarr[c][r]);
					}
				}
			}
		}

	}
	Game(Options); //종료 시 Game() 재실행
	return 0;
}

int TitleScreen() { //타이틀 화면

	//변수
	int Options[2] = {0, 3}; //목숨 갯수는 3, 플레이트는 배경색 없는 타입으로 초기화
	int* OptionsPT = &Options[0]; //Options[] 에 접근하기 위한 pointer variable
	/*
	[0]: 플레이트 타입
	[1]: 기본 목숨 개수
	*/
	int TitleKey; //입력받는 키 값
	int TitleStringType = 1; //BRICKS BREAKER 문구 이동 효과를 위한 변수
	int TItlequit = 0; //종료에 사용. 1이 되면 main 으로 복귀

	BasicTemplateDisplay(); //틀 출력

	
	for (int timer = 0; TItlequit < 1;) { //종료하지 않는 한 무한반복
		timer++; //반복 횟수 타이머 사용
		if (_kbhit() == 1) {
			TitleKey = _getch();
			switch (TitleKey) {
			case ENTER: //enter 입력시 Game() 실행
				HideCursor();
				Game(Options); //?? ??
				system("cls");
				BasicTemplateDisplay();
				break;
			case 71: //g,G 입력시 GuideScreen() 실행
			case 103:
				GuideScreen();
				break;
			case 113:  //q,Q 입력시 종료
			case 81:
				system("cls");
				Sleep(5);
				TItlequit++;
				break;
				break;
			case 105:
			case 73: //i,I 입력시 InfoScreen() 실행
				InfoScreen();
				break;
			case 79:
			case 111: //o, O 입력시 OptionsScreen() 실행
				OptionsScreen(OptionsPT);
				system("cls");
				BasicTemplateDisplay();
				break;
			}
		}
		if (TitleStringType == 0) { //0일때의 화면

			gotoxy(0, 5);
			SetColour(15, 16);
			printf("\t ====   ====   =   ===  =  =  ====\n");
			printf("\t =   =  =   =  =  =     = =  =\n");
			printf("\t ====   ===    =  =     ==    ====\n");
			printf("\t =   =  =  =   =  =     = =       =\n");
			printf("\t ====   =   =  =   ===  =  =  ====              ===      \n");
			printf("\t                                                =  o     \n");
			printf("\t ====   ====   ====    ==    =  =  ====  ====       \\   \n");
			printf("\t =   =  =   =  =      =  =   = =   =     =   =       \\  \n");
			printf("\t ====   ===    ====   ====   ==    ====  ===  \n");
			printf("\t =   =  =  =   =     =    =  = =   =     =  =  \n");
			printf("\t ====   =   =  ====  =    =  =  =  ====  =   =  \n");
			printf("\t\n");
			printf("\t\n");
			printf("\t\n");
			printf("\t\t\t PRESS (ENTER) TO PLAY\n");
			printf("\t\n");
			SetColour(8, 16);
			printf("\t\t\t (G) How to play  (O) Options  (I) Info  (Q) Quit\n");
		}
		else { //1일때의 화면

			gotoxy(0, 5);
			SetColour(15, 16);
			printf("\t====   ====   =   ===  =  =  ====  \n");
			printf("\t=   =  =   =  =  =     = =  =  \n");
			printf("\t====   ===    =  =     ==    ====  \n");
			printf("\t=   =  =  =   =  =     = =       =  \n");
			printf("\t====   =   =  =   ===  =  =  ====                o      \n");
			printf("\t                                                  \\    \n");
			printf("\t  ====   ====   ====    ==    =  =  ====  ====          \n");
			printf("\t  =   =  =   =  =      =  =   = =   =     =   =         \n");
			printf("\t  ====   ===    ====   ====   ==    ====  ===\n");
			printf("\t  =   =  =  =   =     =    =  = =   =     =  =\n");
			printf("\t  ====   =   =  ====  =    =  =  =  ====  =   =\n");
			printf("\t\n");
			printf("\t\n");
			printf("\t\n");
			printf("\t\t\t PRESS (ENTER) TO PLAY\n");
			printf("\t\n");
			SetColour(8, 16);
			printf("\t\t\t (G) How to play  (O) Options  (I) Info  (Q) Quit\n");
		}
		if (timer % 1700 == 0) {   //일정한 주기로 변경, 1700 은 실험으로 얻은 값
			TitleStringType++;
			if (TitleStringType > 1)
				TitleStringType = 0;
		}
	}
	system("cls"); //반복종료시 화면을 모두 지우고 종료
	return 0;
}

int GuideScreen() { //사용법 화면, 정보는 적음
	system("cls");

	SetColour(15, 16);
	gotoxy(MidX - 17, MidY - 10);
	printf("(<-) (->) Arrow Keys to move the plate");
	gotoxy(MidX - 17, MidY - 9);
	printf("Tip. always follow the ball, press longer!");
	gotoxy(MidX - 13, MidY - 5);
	SetColour(8, 16);
	printf("Press any key to go back");
	SetColour(15, 16);

	for(int back = 0; back < 1;) { //_kbhit() 으로 키 입력 시 종료하게 함
		if (_kbhit() == 1) {
			back++;
			system("cls");
			break;
		}
	}
	
	return 0;

}

int InfoScreen() { //정보 화면, 제작자와 제작기간, 목적 표시
	system("cls");

	SetColour(15, 16);
	gotoxy(MidX - 13, MidY - 10);
	printf("Created By Koh Eunhyuk\n\n");
	printf("\t\t\tA Game for personal practice, based on Atari Breakout\n");
	printf("\t\t\tMay 2023 ~ Sep 2023\n\n\n");
	SetColour(8, 16);
	printf("\t\t\tPress any key to go back");
	SetColour(15, 16);

	for (int back = 0; back < 1;) { //_kbhit() 으로 키 입력 시 종료하게 함
		if (_kbhit() == 1) {
			back++;
			system("cls");
			break;
		}
	}

	return 0;
}

int OptionsScreen(int* Option) { //설정 화면
	int optionkey; //키 값
	system("cls");
	SetColour(15, 16);

	gotoxy(0, 5); //설정 방법 안내
	printf("\n\t\t(<-) (->) adjust number of lives\n");
	printf("\t\t(P) High Visiblity Plate on/off\n");
	printf("\t\t(Q) go back");



	for (int backcondition = 0; backcondition < 1;) {
		if (_kbhit() == 1) {
			optionkey = _getch();
			if (optionkey == FirstKeyValue) { //화살표 키는 2자리
				optionkey = _getch();
				switch (optionkey) { //2??
				case LEFT: //arrow L, R : 목숨 개수 감소/증가
					Option[1]--;
					break;
				case RIGHT:
					Option[1]++;
					break;
				}
			}
			else { //1자리
				switch (optionkey) {
				case 80:
				case 112: //p, P 입력시 플레이트 타입 변경
					Option[0]++;
					break;
				case 81: //q, Q 입력시 종료
				case 113:
					return 0;
					break;
				}
			}
		}
		
		//값 조정 (플레이트는 0,1 중 하나로, 목숨 개수는 1~10 중으로
		if (Option[0] > 1)
			Option[0] = 0;
		if (Option[1] < 1)   //? ??
			Option[1] = 1;
		if (Option[1] > 10)
			Option[1] = 10;

		Sleep(1); //값이 업데이트되도록 미세하게 딜레이

		//변화된 값 출력

		gotoxy(10, 15);
		printf("Number Of Lives : < %d > ", Option[1]);

		gotoxy(10, 17);
		if (Option[0] == 0)
			printf("Plate Type : _______\n");
		if (Option[0] == 1) {
			printf("Plate Type : ");
			SetColour(16, 15);
			printf("_______\n");
			SetColour(15, 16);
		}
	}
	return 0; //반복종료 시 종료
}

int BasicTemplateDisplay() { //사각형 틀 출력
	COORD templatepos = getCursor(); //여기서만 사용함
	gotoxy(1, 0);
	SetColour(15, 16);
	printf("|"); //좌측 상단 모서리

	for (int cnt = 0; cnt < 114; cnt++)
		printf("-"); //위쪽 가로 출력

	printf("|"); //우측 상단 모서리

	gotoxy(0, 1);
	for (int cnt = 0; cnt < 28; cnt++)
		printf(" |\n"); //좌측 세로 출력

	gotoxy(116, 1);
	templatepos = getCursor();
	for (int cnt = 0; cnt < 28; cnt++) {
		gotoxy(116, templatepos.Y);
		printf("|\n");  //우측 세로 출력
		templatepos = getCursor();
	}
	gotoxy(1, 29);
	printf("|"); //좌측 하단 모서리

	for (int cnt = 0; cnt < 114; cnt++)
		printf("-"); //아래쪽 가로 출력

	printf("|"); //우측 하단 모서리


	return 0;
}

COORD getCursor(void) { //현재 커서 좌표를 COORD 로 리턴
	COORD curPoint;
	CONSOLE_SCREEN_BUFFER_INFO pos;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &pos);
	curPoint.X = pos.dwCursorPosition.X;
	curPoint.Y = pos.dwCursorPosition.Y;
	return curPoint;
}

void gotoxy(int x, int y) {//입력한 좌표로 커서 이동
	COORD Pos;
	Pos.X = x;
	Pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

void HideCursor() { //커서 표시 숨김 (토글)
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(consoleHandle, &info);
}

void ShowCursors() {   //커서 표시 보이게 함 (토글)
	//원인은 알 수 없지만 이름을 ShowCursor 로 할 경우 HidCursor() 과 충돌 발생
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = TRUE;
	SetConsoleCursorInfo(consoleHandle, &info);
}

SetColour(int text, int bg) { //출력 색 변경 (텍스트, 배경)
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), text | (bg << 4));
	return 0;
}