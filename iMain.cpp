﻿#include "iGraphics.h"
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <time.h>
#include <string.h>
#include "bitmap_loader.h"
#include <stdio.h>


//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::Idraw Here::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
#define screenWidth 1000
#define screenHeight 600

//Grid
#define GRID_START_X 180
#define GRID_START_Y 10
#define GRID_WIDTH 730
#define GRID_HEIGHT 500
#define GRID_COLS 9
#define GRID_ROWS 5

int CELL_WIDTH = GRID_WIDTH / GRID_COLS;
int CELL_HEIGHT = GRID_HEIGHT / GRID_ROWS;

//Pages
int homepage = 1, startpage = 0, scorePage = 0, aboutpage = 0, controlpage = 0, storypage = 0;
int level1page = 0, level2page = 0;
int gameStatus = 0; // 0=playing, 1=win, 2=lose
int level2_unlocked = 0, score_for_level2 = 0;
int name_input_page = 1;
char str1[40];
int len = 0;
int retrypage = 0;

//....Structures....
typedef struct Player {
	char name[30];
	int score;
} Player;
Player o1;


typedef struct FilePlayer {
	char name[30];
	int score;
} FilePlayer;


// Peashooter
char ps[5][20] = { "gimages\\ps1.bmp", "gimages\\ps2.bmp", "gimages\\ps3.bmp", "gimages\\ps4.bmp", "gimages\\ps5.bmp" };
#define MAX_PEASHOOTERS 10

struct Peashooter {
	int x, y;
	int p_x, p_y;
	int psIndex;
	bool isActive;
};
Peashooter peashooters[MAX_PEASHOOTERS];
int peashooterCount = 0;

//Potato Mine
#define MAX_MINES 10
char mine[30] = "gimages\\mine2.bmp";
char blast[30] = "gimages\\blast.bmp";

struct PotatoMine {
	int x, y;
	bool isActive;
	bool isBlasting;
	int blastTimer;
};
PotatoMine mines[MAX_MINES];
int mineCount = 0;
int plantType = 1;

//ZOMBIE
#define MAX_ZOMBIES 4
char zm[5][30] = { "gimages\\zm1.bmp", "gimages\\zm2.bmp", "gimages\\zm3.bmp", "gimages\\zm4.bmp", "gimages\\zm5.bmp" };
char coneZmImg[30] = "gimages\\carzoombie.bmp";

struct Zombie {
	int x, y;
	int health;
	int animationIndex;
	bool isActive;
	int type;
};
Zombie zombies[MAX_ZOMBIES];
int activeZombieCount = 0, zombiesDefeated = 0, zombiesToDefeat = 6;
// Boss Zombie
bool bossAppeared = false;
bool bossWarningActive = false;
int bossHealth = 200;
int bossX = 1000;
int bossY = 200;
bool bossWarningSoundPlayed = false;

//Sun, score and timer
struct Sun {
	int x, y, image, speed;
	bool visible;
} S;
#define SUN_SPEED 10

int score = 0;
int game_time = 60;
char txt[30];
bool musicOn = true;

//Page call
void drawHomePage();
void drawStartPage();
void drawScorePage();
void drawAboutPage();
void drawStoryPage();
void drawControlPage();
void drawLevel1Page();
void drawLevel2Page();
void initializePeashooters();
void initializeZombies();
void spawnZombie();
void checkGameStatus();
void drawWinScreen();
void drawLoseScreen();
void sunSpawnTimer();
void zombieSpawnTimer();
void gameover();
void readScore();
bool newscore = true;


//save high score
void saveHighScore() {
	FILE *fp = fopen("high_score.txt", "a");
	if (fp != NULL) {
		fprintf(fp, "%s %d\n", o1.name, score);
		fclose(fp);
	}
}

void showing_score_page()
{
	FILE *fp = fopen("high_score.txt", "r");
	if (fp == NULL)
	{
		iSetColor(255, 255, 255);
		iText(400, 400, "No high scores yet.", GLUT_BITMAP_TIMES_ROMAN_24);
		return;
	}

	FilePlayer fileplayer[100];
	int playerCount = 0;
	char name[30];
	int score;


	while (fscanf(fp, "%s%d", name, &score) != EOF)
	{
		int found = 0;
		for (int i = 0; i < playerCount; ++i)
		{
			if (strcmp(fileplayer[i].name, name) == 0)
			{
				if (score > fileplayer[i].score)
				{
					fileplayer[i].score = score;
				}
				found = 1;
				break;
			}
		}
		if (!found)
		{
			strcpy(fileplayer[playerCount].name, name);
			fileplayer[playerCount].score = score;
			playerCount++;
		}
	}
	fclose(fp);


	for (int i = 0; i < playerCount - 1; i++)
	{
		for (int j = i + 1; j < playerCount; j++)
		{
			if (fileplayer[i].score < fileplayer[j].score)
			{
				FilePlayer temp = fileplayer[i];
				fileplayer[i] = fileplayer[j];
				fileplayer[j] = temp;
			}
		}
	}
	int displayCount;

	if (playerCount > 7) {
		displayCount = 7;
	}
	else {
		displayCount = playerCount;
	}

	iSetColor(255, 255, 255);
	iText(250, 500, "HIGH SCORES", GLUT_BITMAP_TIMES_ROMAN_24);
	iText(250, 470, "-------------------------", GLUT_BITMAP_TIMES_ROMAN_24);

	for (int i = 0; i < displayCount; i++) {
		char name[30];
		char scr[10];
		sprintf(name, "%s", fileplayer[i].name);
		sprintf(scr, "%d", fileplayer[i].score);
		iSetColor(255, 255, 0);
		iText(250, 420 - 50 * i, name, GLUT_BITMAP_TIMES_ROMAN_24);
		iText(500, 420 - 50 * i, scr, GLUT_BITMAP_TIMES_ROMAN_24);
	}
}

void showChar() {
	iShowBMP2(0, 0, "image\\menu.bmp", 0);
	iSetColor(239, 114, 0);
	iText(600, 400, "Enter Your Name:", GLUT_BITMAP_TIMES_ROMAN_24);
	iSetColor(239, 114, 0);
	iRectangle(600, 350, 200, 30);
	iText(610, 360, str1, GLUT_BITMAP_HELVETICA_18);
	iText(600, 320, "Press Enter to continue", GLUT_BITMAP_HELVETICA_18);
}

void spawnZombie() {
	if (activeZombieCount < MAX_ZOMBIES) {
		for (int i = 0; i < MAX_ZOMBIES; i++) {
			if (!zombies[i].isActive) {
				zombies[i].isActive = true;
				zombies[i].x = screenWidth + (rand() % 100);

				int randomRow = rand() % GRID_ROWS;
				zombies[i].y = (randomRow * CELL_HEIGHT) + (CELL_HEIGHT / 2);

				if (level1page == 1) {

					zombies[i].type = 0;
					zombies[i].health = 50;
				}
				else if (level2page == 1) {

					if (rand() % 2 == 0) {
						zombies[i].type = 0;
						zombies[i].health = 50;
					}
					else {
						zombies[i].type = 1;
						zombies[i].health = 120;
					}
				}

				zombies[i].animationIndex = 0;
				activeZombieCount++;
				return;
			}
		}
	}
}

//Collision
void collision()
{
	if (gameStatus != 0) return;
	if (bossAppeared) {
		for (int i = 0; i < peashooterCount; i++) {
			if (!peashooters[i].isActive) continue;
			int px = peashooters[i].p_x;
			int py = peashooters[i].p_y;

			if (px >= bossX && px <= bossX + 100 && py >= bossY && py <= bossY + 100) {
				bossHealth -= 10;

				// Reset peashooter projectile
				peashooters[i].p_x = peashooters[i].x + 70;
				peashooters[i].p_y = peashooters[i].y + 50;

				// Random vanish chance
				if (rand() % 10 == 0) {
					peashooters[i].isActive = false;
				}

				if (bossHealth <= 0) {
					bossAppeared = false;
				}
				break;
			}
		}
	}

	for (int i = 0; i < peashooterCount; i++) {
		if (!peashooters[i].isActive) continue;

		int px = peashooters[i].p_x, py = peashooters[i].p_y;

		for (int j = 0; j < MAX_ZOMBIES; j++) {
			if (!zombies[j].isActive) continue;


			if (px >= zombies[j].x && px <= zombies[j].x + 80 &&
				py >= zombies[j].y && py <= zombies[j].y + 100)
			{
				zombies[j].health -= 10;
				score += 10;


				peashooters[i].p_x = peashooters[i].x + 70;
				peashooters[i].p_y = peashooters[i].y + 50;

				if (zombies[j].health <= 0) {
					zombies[j].isActive = false;
					activeZombieCount--;
					zombiesDefeated++;
				}
				break;
			}
		}
	}

	for (int i = 0; i < mineCount; i++) {
		if (mines[i].isActive && !mines[i].isBlasting) {
			int mineCellX = mines[i].x / CELL_WIDTH;
			int mineCellY = mines[i].y / CELL_HEIGHT;


			for (int j = 0; j < MAX_ZOMBIES; j++) {
				if (zombies[j].isActive) {
					int zombieCellX = zombies[j].x / CELL_WIDTH;
					int zombieCellY = zombies[j].y / CELL_HEIGHT;


					if (zombieCellX == mineCellX && zombieCellY == mineCellY) {
						zombies[j].isActive = false;
						activeZombieCount--;
						zombiesDefeated++;


						mines[i].isBlasting = true;
						mines[i].blastTimer = 20;
						mines[i].isActive = false;
						break;
					}
				}
			}
		}
	}
}

void zMovement() {
	if ((level2page == 1 || level1page == 1) && gameStatus == 0){
		for (int i = 0; i < MAX_ZOMBIES; i++) {
			if (zombies[i].isActive) {
				zombies[i].x -= 2;
			}
		}
	}
}

void pMovement() {
	if ((level2page == 1 || level1page == 1) && gameStatus == 0) {
		int peaSpeed = 8;
		for (int i = 0; i < peashooterCount; i++) {
			if (peashooters[i].isActive) {
				peashooters[i].p_x += peaSpeed;
				if (peashooters[i].p_x >= 1000) {
					peashooters[i].p_x = peashooters[i].x + 70;
				}
			}
		}
	}
}

void change_zombie_animation() {
	if ((level2page == 1 || level1page == 1) && gameStatus == 0) {
		for (int i = 0; i < MAX_ZOMBIES; i++) {
			if (zombies[i].isActive) {
				zombies[i].animationIndex = (zombies[i].animationIndex + 1) % 5;
			}
		}
	}
}

void change_peashooter_animation() {
	if ((level2page == 1 || level1page == 1) && gameStatus == 0) {
		for (int i = 0; i < peashooterCount; i++) {
			if (peashooters[i].isActive) {
				peashooters[i].psIndex = (peashooters[i].psIndex + 1) % 5;
			}
		}
	}
}

void countdown_timer() {
	if ((level2page == 1 || level1page == 1) && gameStatus == 0) {
		game_time--;
		if (game_time < 0) {
			gameStatus = 2;
		}
	}
}

double getDistance(int x1, int x2, int y1, int y2) {
	return sqrt((double)(x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

void resetSun() {
	S.y = screenHeight;
	S.x = rand() % (screenWidth - 100);
	S.speed = SUN_SPEED;
	S.visible = true;
}

void moveSunDown() {
	if ((level2page == 1 || level1page == 1) && gameStatus == 0) {
		S.y -= S.speed;
		if (S.y <= 0) {
			S.visible = false;
		}
	}
}

void loadSunImage() {
	S.image = iLoadImage("Images\\sun.png");
	S.visible = false;
}

void initializePeashooters() {
	peashooterCount = 0;
	for (int i = 0; i < MAX_PEASHOOTERS; i++) {
		peashooters[i].isActive = false;
	}
}

void initializeZombies() {
	activeZombieCount = 0;
	for (int i = 0; i < MAX_ZOMBIES; i++) {
		zombies[i].isActive = false;
	}
}

void initializeMines() {
	mineCount = 0;
	for (int i = 0; i < MAX_MINES; i++) {
		mines[i].isActive = false;
		mines[i].isBlasting = false;
		mines[i].blastTimer = 0;
	}
}

void update_mines() {

	if (level2page != 1 || gameStatus != 0) return;
	for (int i = 0; i < mineCount; i++) {
		if (!mines[i].isActive) continue;
		if (mines[i].isBlasting) {
			if (mines[i].blastTimer > 0) mines[i].blastTimer--;
			else {

				mines[i].isActive = false;
				mines[i].isBlasting = false;
			}
		}
	}
}
void checkGameStatus() {
	if (gameStatus != 0) return;

	for (int i = 0; i < MAX_ZOMBIES; i++) {
		if (zombies[i].isActive && zombies[i].x <= 100) {
			gameStatus = 2;
			saveHighScore();
			PlaySound("music\\lose.wav", NULL, SND_ASYNC);
			//PlaySound(NULL, 0, 0);
		}
	}
	if (level2page == 1 && bossX <= 100){
		gameStatus = 2;
		saveHighScore();
		PlaySound("music\\lose.wav", NULL, SND_ASYNC);
	}

	if (level1page == 1 && zombiesDefeated >= zombiesToDefeat) {
		gameStatus = 1;
		saveHighScore();
		PlaySound("music\\victory.wav", NULL, SND_ASYNC);
		//PlaySound(NULL, 0, 0);
	}

	if (level2page == 1 && bossHealth <= 0) {
		gameStatus = 1;
		saveHighScore();
		PlaySound("music\\victory.wav", NULL, SND_ASYNC);
		//PlaySound(NULL, 0, 0);
	}
}

void sunSpawnTimer() {
	if ((level2page == 1 || level1page == 1) && gameStatus == 0 && !S.visible) {
		resetSun();
	}
}

void zombieSpawnTimer() {
	if ((level2page == 1 || level1page == 1) && gameStatus == 0) {
		spawnZombie();
	}
}
//reset
void resetLevel1() {
	score = 0;
	game_time = 60;
	zombiesDefeated = 0;
	initializePeashooters();
	initializeZombies();
	S.visible = false;
	level1page = 1;
	level2page = 0;
	gameStatus = 0;
	if (musicOn) PlaySound("music\\menusound.wav", NULL, SND_LOOP | SND_ASYNC);
}

void resetLevel2() {
	score = score_for_level2;
	game_time = 100;
	zombiesDefeated = 0;
	initializePeashooters();
	initializeZombies();
	initializeMines();
	S.visible = false;
	level1page = 0;
	level2page = 1;
	gameStatus = 0;
	if (musicOn) PlaySound("music\\menusound.wav", NULL, SND_LOOP | SND_ASYNC);
}


void iDraw()
{
	iClear();
	if (name_input_page) {
		showChar();
		return;
	}

	checkGameStatus();

	if (homepage == 1) {
		drawHomePage();
	}
	else if (startpage == 1) {
		drawStartPage();
	}
	else if (aboutpage == 1) {
		drawAboutPage();
	}
	else if (controlpage == 1) {
		drawControlPage();
	}
	else if (storypage == 1) {
		drawStoryPage();
	}
	else if (scorePage == 1) {
		drawScorePage();
	}
	else if (level1page == 1) {
		if (gameStatus == 0) {
			drawLevel1Page();
		}
		else if (gameStatus == 1) {
			drawWinScreen();
		}
		else if (gameStatus == 2) {
			drawLoseScreen();
		}
	}
	else if (level2page == 1) {
		if (gameStatus == 0) {
			drawLevel2Page();
		}
		else if (gameStatus == 1) {
			drawWinScreen();
		}
		else if (gameStatus == 2) {
			drawLoseScreen();
		}
	}

}

void iMouseMove(int mx, int my) {
	int x = mx;
	int y = my;
	printf("x=%d", x);
	printf("y=%d", y);
}

void iPassiveMouseMove(int mx, int my)
{
}

void iMouse(int button, int state, int mx, int my)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{

		if ((mx >= 9 && mx <= 58) && (my >= 9 && my <= 59)) {
			homepage = 1;
			startpage = 0;
			aboutpage = 0;
			controlpage = 0;
			storypage = 0;
			scorePage = 0;
			level1page = 0;
			level2page = 0;
			gameStatus = 0;
			return;
		}


		if (homepage == 1) {
			if ((mx >= 380 && mx <= 599) && (my >= 380 && my <= 430)) { homepage = 0; startpage = 1; }
			else if ((mx >= 380 && mx <= 598) && (my >= 320 && my <= 370)) { homepage = 0; scorePage = 1; }
			else if ((mx >= 380 && mx <= 597) && (my >= 140 && my <= 190)) { homepage = 0; aboutpage = 1; }
			else if ((mx >= 381 && mx <= 597) && (my >= 204 && my <= 249)) { homepage = 0; controlpage = 1; }
			else if ((mx >= 380 && mx <= 600) && (my >= 260 && my <= 310)) { homepage = 0; storypage = 1; }
			else if ((mx >= 380 && mx <= 600) && (my >= 80 && my <= 130)) { exit(0); }
		}
		else if (startpage == 1) {
			//LEVEL-1
			if ((mx >= 380 && mx <= 599) && (my >= 380 && my <= 430)) {
				level1page = 1;
				homepage = 0;
				startpage = 0;
				score = 0;
				game_time = 60;
				zombiesToDefeat = 6;
				zombiesDefeated = 0;
				gameStatus = 0;
				initializePeashooters();
				initializeZombies();
				S.visible = false;
				if (musicOn) PlaySound("music\\menusound.wav", NULL, SND_LOOP | SND_ASYNC);
			}

			//LEVEL-2
			else if ((mx >= 380 && mx <= 599) && (my >= 260 && my <= 310)) {
				if (level2_unlocked) {
					level2page = 1;
					homepage = 0;
					startpage = 0;
					score = score_for_level2;
					game_time = 100;
					zombiesToDefeat = 6;
					zombiesDefeated = 0;
					gameStatus = 0;
					initializePeashooters();
					initializeZombies();
					initializeMines();
					S.visible = false;
					if (musicOn) PlaySound("music\\menusound.wav", NULL, SND_LOOP | SND_ASYNC);
				}
			}
		}
		else if ((level2page == 1 || level1page == 1) && gameStatus == 0) {
			if (S.visible && getDistance(S.x + 50, mx, S.y + 50, my) <= 50) {
				score += 50;
				S.visible = false;
			}
		}
		//retry button on the lose screen
		else if (gameStatus == 2) {
			if ((mx >= 362 && mx <= 534) && (my >= 281 && my <= 324)) {
				if (level1page == 1) {
					resetLevel1();
				}
				else if (level2page == 1) {
					resetLevel2();
				}
			}
		}
		//hard level button on the win screen
		else if (level1page == 1 && gameStatus == 1) {
			if ((mx >= 339 && mx <= 567) && (my >= 278 && my <= 322)) {
				level1page = 0;
				level2page = 1;
				gameStatus = 0;
				game_time = 100;
				zombiesToDefeat = 6;
				zombiesDefeated = 0;
				initializePeashooters();
				initializeZombies();
				initializeMines();
				S.visible = false;
				if (musicOn) PlaySound("music\\menusound.wav", NULL, SND_LOOP | SND_ASYNC);
			}
		}

	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		if (level1page == 1 && gameStatus == 0) {

			if (score >= 100 && peashooterCount < MAX_PEASHOOTERS) {
				int column = (mx - GRID_START_X) / CELL_WIDTH;
				int row = (my - GRID_START_Y) / CELL_HEIGHT;

				score -= 100;

				peashooters[peashooterCount].x = GRID_START_X + column * CELL_WIDTH + (CELL_WIDTH / 2);
				peashooters[peashooterCount].y = GRID_START_Y + row * CELL_HEIGHT + (CELL_HEIGHT / 2);

				peashooters[peashooterCount].isActive = true;
				peashooters[peashooterCount].psIndex = 0;

				peashooters[peashooterCount].p_x = peashooters[peashooterCount].x + 70;
				peashooters[peashooterCount].p_y = peashooters[peashooterCount].y + 50;

				peashooterCount++;
			}
		}
		if (level2page == 1 && gameStatus == 0) {
			if (plantType == 1){
				if (score >= 100 && peashooterCount < MAX_PEASHOOTERS) {
					int column = (mx - GRID_START_X) / CELL_WIDTH;
					int row = (my - GRID_START_Y) / CELL_HEIGHT;

					score -= 100;

					peashooters[peashooterCount].x = GRID_START_X + column * CELL_WIDTH + (CELL_WIDTH / 2);
					peashooters[peashooterCount].y = GRID_START_Y + row * CELL_HEIGHT + (CELL_HEIGHT / 2);

					peashooters[peashooterCount].isActive = true;
					peashooters[peashooterCount].psIndex = 0;

					peashooters[peashooterCount].p_x = peashooters[peashooterCount].x + 70;
					peashooters[peashooterCount].p_y = peashooters[peashooterCount].y + 50;

					peashooterCount++;
				}
			}

			else if (plantType == 2){
				if (mineCount < MAX_MINES && score >= 200) {
					int column = (mx - GRID_START_X) / CELL_WIDTH;
					int row = (my - GRID_START_Y) / CELL_HEIGHT;

					int pmx = GRID_START_X + column * CELL_WIDTH + (CELL_WIDTH / 2);
					int pmy = GRID_START_Y + row * CELL_HEIGHT + (CELL_HEIGHT / 2);

					score -= 200;
					mines[mineCount].x = pmx;
					mines[mineCount].y = pmy;
					mines[mineCount].isActive = true;
					mines[mineCount].isBlasting = false;
					mines[mineCount].blastTimer = 0;
					mineCount++;
				}
			}
		}
	}
}

// NEW: Function to handle name input
void takeinput(unsigned char key){
	if (key == '\r'){
		strcpy(o1.name, str1);
		name_input_page = 0;
		homepage = 1;
		startpage = 0;
		scorePage = 0;
		len = 0;
	}
	else if (key == '\b'){
		if (len > 0){
			len--;
			str1[len] = '\0';
		}
	}
	else{
		str1[len] = key;
		len++;
		if (len > 15){
			len = 15;
		}
		str1[len] = '\0';
	}
}

// NEW: Updated iKeyboard to handle different pages
void iKeyboard(unsigned char key) {
	if (name_input_page) {
		takeinput(key);
	}
	else {
		if (key == '1') plantType = 1;
		else if (key == '2') plantType = 2;
	}
}

void iSpecialKeyboard(unsigned char key)
{
}

void drawHomePage() {
	iShowBMP2(0, 0, "image\\menu.bmp", 0);

	iShowBMP2(380, 385, "image\\start.bmp", 0);
	iShowBMP2(380, 320, "image\\score.bmp", 0);
	iShowBMP2(380, 260, "image\\story.bmp", 0);
	iShowBMP2(370, 200, "image\\control.bmp", 0);
	iShowBMP2(380, 140, "image\\about.bmp", 0);
	iShowBMP2(380, 80, "image\\exit.bmp", 0);

}

void drawStartPage() {
	iShowBMP2(0, 0, "image\\background.bmp", 0);
	iShowBMP2(10, 10, "image\\back.bmp", 0);
	iShowBMP2(380, 380, "image\\easy.bmp", 0);
	if (level2_unlocked) {
		iShowBMP2(380, 238, "image\\hard.bmp", 0);
	}
	else {
		iShowBMP2(380, 238, "image\\hardash.bmp", 0);
	}

}

void drawAboutPage() {
	iShowBMP2(0, 0, "image\\aboutpage.bmp", 0);
	iShowBMP2(10, 10, "image\\back.bmp", 0);
}

void drawStoryPage() {
	iShowBMP2(0, 0, "image\\storypage.bmp", 0);
	iShowBMP2(10, 10, "image\\back.bmp", 0);


}

void drawControlPage() {
	iShowBMP2(0, 0, "image\\controlpage.bmp", 0);
	iShowBMP2(10, 10, "image\\back.bmp", 0);
}

void drawScorePage() {
	iShowBMP2(0, 0, "image\\bg.bmp", 0);
	iShowBMP2(10, 10, "image\\back.bmp", 0);
	showing_score_page();
}

void drawLevel1Page() {
	iShowBMP2(0, 0, "gimages\\background.bmp", 0);

	for (int i = 0; i < peashooterCount; i++) {
		if (peashooters[i].isActive) {
			iShowBMP2(peashooters[i].x, peashooters[i].y, ps[peashooters[i].psIndex], 0);
			iShowBMP2(peashooters[i].p_x, peashooters[i].p_y, "gimages\\p.bmp", 0);
		}
	}

	for (int i = 0; i < MAX_ZOMBIES; i++) {
		if (zombies[i].isActive) {
			iShowBMP2(zombies[i].x, zombies[i].y, zm[zombies[i].animationIndex], 0);

			iSetColor(255, 0, 0);
			iFilledRectangle(zombies[i].x, zombies[i].y + 130, zombies[i].health, 10);
		}
	}

	if (S.visible) iShowBMP2(S.x, S.y, "image\\sun.bmp", 0);

	iSetColor(150, 75, 0); iFilledRectangle(841, 543, 115, 33);
	iSetColor(150, 255, 244);

	sprintf_s(txt, "SUN : %d", score);
	iText(850, 550, txt, GLUT_BITMAP_TIMES_ROMAN_24);


	char time_text[30];
	iSetColor(150, 75, 0); iFilledRectangle(39, 543, 123, 33);
	iSetColor(150, 255, 244);
	sprintf_s(time_text, "TIME : %d", game_time);
	iText(50, 550, time_text, GLUT_BITMAP_TIMES_ROMAN_24);

	char zombies_text[30];
	iSetColor(150, 75, 0); iFilledRectangle(340, 543, 309, 33);
	iSetColor(150, 255, 244);
	sprintf_s(zombies_text, "ZOMBIES DEFEATED: %d / %d", zombiesDefeated, zombiesToDefeat);
	iText(350, 550, zombies_text, GLUT_BITMAP_TIMES_ROMAN_24);

	iShowBMP2(10, 10, "image\\back.bmp", 0);
}

void drawLevel2Page() {
	iShowBMP2(0, 0, "gimages\\background.bmp", 0);

	//peashooters
	for (int i = 0; i < peashooterCount; i++) {
		if (peashooters[i].isActive) {
			iShowBMP2(peashooters[i].x, peashooters[i].y, ps[peashooters[i].psIndex], 0);
			iShowBMP2(peashooters[i].p_x, peashooters[i].p_y, "gimages\\p.bmp", 0);
		}
	}
	/////////////////////////////////
	for (int i = 0; i < mineCount; i++) {
		if (mines[i].isActive) {
			iShowBMP2(mines[i].x, mines[i].y, mine, 0);
		}
		else if (mines[i].isBlasting) {
			iShowBMP2(mines[i].x, mines[i].y, blast, 0);
		}
	}
	// Draw zombies
	for (int i = 0; i < MAX_ZOMBIES; i++) {
		if (zombies[i].isActive) {
			if (zombies[i].type == 0) {

				iShowBMP2(zombies[i].x, zombies[i].y, zm[zombies[i].animationIndex], 0);
			}
			else if (zombies[i].type == 1) {

				iShowBMP2(zombies[i].x, zombies[i].y, coneZmImg, 0);
			}


			iSetColor(255, 0, 0);
			iFilledRectangle(zombies[i].x, zombies[i].y + 130, zombies[i].health, 10);
		}
	}

	// Draw Sun
	if (S.visible) iShowBMP2(S.x, S.y, "image\\sun.bmp", 0);

	// Sun score
	iSetColor(150, 75, 0); iFilledRectangle(841, 543, 115, 33);
	iSetColor(150, 255, 244);

	sprintf_s(txt, "SUN : %d", score);
	iText(850, 550, txt, GLUT_BITMAP_TIMES_ROMAN_24);


	char time_text[30];
	iSetColor(150, 75, 0); iFilledRectangle(39, 543, 123, 33);
	iSetColor(150, 255, 244);
	sprintf_s(time_text, "TIME : %d", game_time);
	iText(50, 550, time_text, GLUT_BITMAP_TIMES_ROMAN_24);

	char zombies_text[30];
	/*iSetColor(150, 75, 0); iFilledRectangle(340, 543, 309, 33);
	iSetColor(150, 255, 244);
	sprintf_s(zombies_text, "ZOMBIES DEFEATED: %d / %d", zombiesDefeated, zombiesToDefeat);
	//iText(350, 550, zombies_text, GLUT_BITMAP_TIMES_ROMAN_24);*/

	//planttype 
	if (plantType == 1)
		iText(400, 50, "Peashooter (100)", GLUT_BITMAP_9_BY_15);
	else iText(400, 50, "Potato Mine (200)", GLUT_BITMAP_9_BY_15);
	// Boss warning & spawn logic
	if (!bossAppeared) {
		if (zombiesDefeated >= 6) {
			bossWarningActive = true;
			if (!bossWarningSoundPlayed) {
				//PlaySound("music\\zombies.wav", NULL, SND_ASYNC);
				bossWarningSoundPlayed = true;
			}
		}
		if (zombiesDefeated >= 7) {
			bossWarningActive = false;
			bossAppeared = true;
		}
	}
	if (bossWarningActive) {
		iSetColor(255, 255, 255);
		iText(350, 100, "WARNING! BOSS INCOMING!", GLUT_BITMAP_TIMES_ROMAN_24);
	}

	// Draw boss
	if (bossAppeared) {
		iSetColor(255, 0, 0);
		iFilledRectangle(bossX, bossY + 230, bossHealth, 10);
		iShowBMP2(bossX, bossY, "gimages\\boss.bmp", 0);

		// Boss movement
		bossX -= 2;
	}
	iShowBMP2(10, 10, "image\\back.bmp", 0);
}

void drawWinScreen() {

	iShowBMP2(10, 10, "image\\back.bmp", 0);
	iShowBMP2(0, 0, "image\\bg.bmp", 0);
	iShowBMP2(190, 443, "image\\win.bmp", 0);
	iShowBMP2(360, 364, "image\\score_show.bmp", 0);
	iSetColor(255, 255, 255);
	char final_score_text[50];
	sprintf(final_score_text, "%d", score);
	iText(560, 380, final_score_text, GLUT_BITMAP_TIMES_ROMAN_24);

	if (level1page == 1) {
		level2_unlocked = 1;
		score_for_level2 = score;

		iShowBMP2(335, 273, "image\\hard.bmp", 0);

	}
	iShowBMP2(10, 10, "image\\back.bmp", 0);

}


void drawLoseScreen() {
	iShowBMP2(10, 10, "image\\back.bmp", 0);
	iShowBMP2(0, 0, "image\\bg.bmp", 0);
	iShowBMP2(190, 443, "image\\loose.bmp", 0);
	iShowBMP2(360, 364, "image\\score_show.bmp", 0);
	iSetColor(255, 255, 255);
	char final_score_text[50];
	sprintf(final_score_text, "%d", score);
	iText(560, 380, final_score_text, GLUT_BITMAP_TIMES_ROMAN_24);
	iShowBMP2(360, 276, "image\\retry.bmp", 0);
	iShowBMP2(10, 10, "image\\back.bmp", 0);
}

int main()
{
	iSetTimer(40, pMovement);
	iSetTimer(750, change_peashooter_animation);
	iSetTimer(20, zMovement);
	iSetTimer(300, change_zombie_animation);
	iSetTimer(40, moveSunDown);
	iSetTimer(1000, countdown_timer);
	iSetTimer(10, collision);
	iSetTimer(4000, sunSpawnTimer);
	iSetTimer(4000, zombieSpawnTimer);
	iSetTimer(40, update_mines);

	loadSunImage();

	iInitialize(screenWidth, screenHeight, "BATTLE OF DEFENSE");
	iStart();
	return 0;
}