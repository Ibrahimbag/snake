#include "../libs/miniaudio.h"
#include "../libs/tigr.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

typedef struct vector_2 {
	int x;
	int y;
} vector_2;

#define SCREEN_WIDTH 32
#define SCREEN_HEIGHT 32
#define MAX_SCORE (SCREEN_WIDTH * SCREEN_HEIGHT)

void get_direction(Tigr *screen, vector_2 *direction);
void move_snake(Tigr *screen, vector_2 *head, vector_2 body[], vector_2 *direction, int score);
bool snake_touches_apple(vector_2 *head, vector_2 *apple);
void spawn_apple(Tigr *screen, vector_2 *apple, vector_2 *head, vector_2 body[], int *score);
void draw_snake(Tigr *screen, vector_2 *head, vector_2 body[], int score);
void draw_apple(Tigr *screen, vector_2 *apple);
void draw_border(Tigr *screen);
bool snake_collided(Tigr *screen, vector_2 *head);

int main(void)
{
	vector_2 head = {10, 10};
	vector_2 body[MAX_SCORE];
	vector_2 direction = {0, 1};
	vector_2 apple = {0, 0};
	int score = 0;

	ma_result result;
	ma_engine engine;

	result = ma_engine_init(NULL, &engine);
	if (result != MA_SUCCESS) {
		fprintf(stderr, "Failed to initialize sound engine\n");
	}

	srand(time(NULL));
	Tigr *screen = tigrWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "snake", 0);
	tigrSetPostFX(screen, 0.5, 0.5, 0, 1);

	spawn_apple(screen, &apple, &head, body, &score);
	
	while (!tigrClosed(screen) && !tigrKeyDown(screen, TK_ESCAPE)) {
		tigrClear(screen, tigrRGB(0x98, 0x93, 0x25));

		get_direction(screen, &direction);
		move_snake(screen, &head, body, &direction,score);

		if (snake_touches_apple(&head, &apple)) {
			spawn_apple(screen, &apple, &head, body, &score);

			if (result == MA_SUCCESS) {
				ma_engine_play_sound(&engine, "sounds/score.mp3", NULL);
			}

			score++;
		}

		draw_snake(screen, &head, body, score);
		draw_apple(screen, &apple);
		draw_border(screen);

		if (snake_collided(screen, &head)) {
			if (result == MA_SUCCESS) {
				ma_engine_play_sound(&engine, "sounds/crash.mp3", NULL);
				sleep(2);
			}
			
			break; // Change this later
		}

		struct timespec remaining, request = {0, 70000000};
		nanosleep(&request, &remaining);
		
        tigrUpdate(screen);		
    }
    
	ma_engine_uninit(&engine);
    tigrFree(screen);

    return 0;
}

void get_direction(Tigr *screen, vector_2 *direction)
{	
	if (tigrKeyDown(screen, TK_UP) && direction->y != 1) {
		direction->y = -1;
		direction->x = 0; 
	} else if (tigrKeyDown(screen, TK_DOWN) && direction->y != -1) {
		direction->y = 1;
		direction->x = 0;
	} else if (tigrKeyDown(screen, TK_LEFT) && direction->x != 1) {
		direction->y = 0;
		direction->x = -1;
	} else if (tigrKeyDown(screen, TK_RIGHT) && direction->x != -1) {
		direction->y = 0;
		direction->x = 1;
	}
}

void move_snake(Tigr *screen, vector_2 *head, vector_2 body[], vector_2 *direction, int score)
{
	for (int i = score; i > 0; i--) {
		body[i] = body[i - 1];
	}
	body[0] = *head;

	head->x += direction->x;
	head->y += direction->y;
}

bool snake_touches_apple(vector_2 *head, vector_2 *apple)
{
	if (head->x == apple->x && head->y == apple->y) {
		return true;
	}

	return false;
}

void spawn_apple(Tigr *screen, vector_2 *apple, vector_2 *head, vector_2 body[], int *score)
{
	int x, y;
	bool repeat;

	do {
		x = 2 + rand() % (SCREEN_WIDTH - 5);
		y = 2 + rand() % (SCREEN_HEIGHT - 5);

		if (head->x == x && head->y == y) {
			continue;
		}

		repeat = false;
		for (int i = 0; i < *score; i++) {
			if (body[i].x == x && body[i].y == y) {
				repeat = true;
				break;
			}
		}
	} while (repeat);

	apple->x = x;
	apple->y = y;
}

void draw_snake(Tigr *screen, vector_2 *head, vector_2 body[], int score)
{
	tigrPlot(screen, head->x, head->y, tigrRGB(0x29, 0x22, 0x0B));

	for (int i = 0; i < score; i++) {
		tigrPlot(screen, body[i].x, body[i].y, tigrRGB(0x34, 0x2B, 0x0E));
	}
}

void draw_apple(Tigr *screen, vector_2 *apple)
{
	tigrPlot(screen, apple->x, apple->y, tigrRGB(0x83, 0x27, 0x27));
}

void draw_border(Tigr *screen) 
{
	tigrLine(screen, 1, 1, SCREEN_WIDTH - 1, 1, tigrRGB(0x34, 0x2B, 0x0E));
	tigrLine(screen, 1, 1, 1, SCREEN_HEIGHT - 1, tigrRGB(0x34, 0x2B, 0x0E));
	tigrLine(screen, SCREEN_WIDTH - 2, 1, SCREEN_WIDTH - 2, SCREEN_HEIGHT - 1, tigrRGB(0x34, 0x2B, 0x0E));
	tigrLine(screen, 1, SCREEN_HEIGHT - 2, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 2, tigrRGB(0x34, 0x2B, 0x0E));
}

bool snake_collided(Tigr *screen, vector_2 *head)
{
	int x = head->x, y = head->y;
	int index = y * screen->w + x;
	return screen->pix[index].r == 0x34 ? true : false;
}