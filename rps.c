#include <raylib.h>
#include <stdlib.h>
#include <time.h>



//global values

#define grid_w 300
#define grid_h 150
#define convert_nb 2
#define bg_color (Color) {0, 0, 0, 255}
#define empty_color (Color) {20, 20, 20, 255}
#define color_1 (Color) {205, 196, 255, 255}
#define color_2 (Color) {169, 94, 255, 255}
#define color_3 (Color) {98, 0, 255, 255}
#define selector_color (Color) {255, 255, 255, 255}



//types

typedef struct {
	int width;
	int height;
	int *tab; //values
} Board;

Board* init_board(int n, int m) {
	Board *b = malloc(sizeof(Board));
	b->width = n;
	b->height = m;
	b->tab = malloc(n*m * (sizeof(int)));
	return b;
}

void free_board(Board *b) {
	free(b->tab);
	free(b);
}



//functions

void window_init(Color bg) {
	InitWindow(0, 0, "Rock paper scissor");
	ClearBackground(bg);
	SetTargetFPS(60);
}

int min(int x, int y) {
	if (x < y) x;
	else y;
}

int max(int x, int y) {
	if (x > y) x;
	else y;
}

void draw_color_display(int active_color) {
	int w = GetScreenWidth();
	int h = GetScreenHeight();
	int start_x = h - 100;
	int start_y = w / 2 - 75;
	DrawRectangle(start_y, start_x, 50, 50, color_1);
	DrawRectangle(start_y + 50, start_x, 50, 50, color_2);
	DrawRectangle(start_y + 100, start_x, 50, 50, color_3);
	//draw selected rectangle
	Color active_truecolor;
	switch (active_color) {
	case 2 : active_truecolor = color_2; break;
	case 3 : active_truecolor = color_3; break;
	default : active_truecolor = color_1; break;
	}
	int offset = active_color * 50;
	DrawRectangle(start_y - 55 + offset, start_x - 5, 60, 60, selector_color);
	DrawRectangle(start_y - 50 + offset, start_x, 50, 50, active_truecolor);
	DrawText("1", start_y + 21, start_x + 12, 30, selector_color);
	DrawText("2", start_y + 67, start_x + 12, 30, selector_color);
	DrawText("3", start_y + 117, start_x + 12, 30, selector_color);
}

void draw(Board *b, bool display_grid, int active_color) {
	int w = GetScreenWidth();
	int h = GetScreenHeight();
	int px = min((w-400) / grid_w, (h-400) / grid_h);
	int start_x = h / 2 - px * grid_h / 2;
	int start_y = w / 2 - px * grid_w / 2;

	ClearBackground(bg_color);
	for (int i=0; i < grid_h - 1; i++) {
		for (int j=0; j < grid_w - 1; j++) {
			int cell_size = px;
			if (display_grid) cell_size--;
			if (b->tab[j*grid_h + i] == 1) DrawRectangle(start_y + j * px, start_x + i * px, cell_size, cell_size, color_1);
			if (b->tab[j*grid_h + i] == 2) DrawRectangle(start_y + j * px, start_x + i * px, cell_size, cell_size, color_2);
			if (b->tab[j*grid_h + i] == 3) DrawRectangle(start_y + j * px, start_x + i * px, cell_size, cell_size, color_3);
			if (b->tab[j*grid_h + i] == 0) DrawRectangle(start_y + j * px, start_x + i * px, cell_size, cell_size, empty_color);
		}
	}
	draw_color_display(active_color);
}

int neighbors(Board *b, int n, int x, int y) {
	//count head neighbors without index out of bounds
	int count = 0;
	if (x > 0 && b->tab[y * grid_h + (x-1)] == n) count++;
	if (x < grid_h - 1 && b->tab[y * grid_h + (x+1)] == n) count++;
	if (y > 0 && b->tab[(y-1) * grid_h + x] == n) count++;
	if (y < grid_w - 1 && b->tab[(y+1) * grid_h + x] == n) count++;
	if (x > 0 && y > 0 && b->tab[(y-1) * grid_h + (x-1)] == n) count++;
	if (x > 0 && y < grid_w - 1 && b->tab[(y+1) * grid_h + (x-1)] == n) count++;
	if (x < grid_h - 1 && y > 0 && b->tab[(y-1) * grid_h + (x+1)] == n) count++;
	if (x < grid_h - 1 && y < grid_w - 1 && b->tab[(y+1) * grid_h + (x+1)] == n) count++;
	return count;
}

void update(Board *b) {
	//new updated board
	Board *b2 = init_board(grid_w, grid_h);
	for (int i=0; i < grid_h - 1; i++) {
		for (int j=0; j < grid_w - 1; j++) {
			//apply Rock Paper Cisors rules
			if (b->tab[j * grid_h + i] == 0) {
				int n1 = neighbors(b, 1, i, j);
				int n2 = neighbors(b, 2, i, j);
				int n3 = neighbors(b, 3, i, j);
				if (n1 > 0 && n1 >= n2 && n1 >= n3)	b2->tab[j * grid_h + i] = 1;
				if (n2 > 0 && n2 >= n1 && n3 >= n3)	b2->tab[j * grid_h + i] = 2;
				if (n3 > 0 && n3 >= n2 && n3 >= n1)	b2->tab[j * grid_h + i] = 3;
				if (n1 == 0 && n2 == 0 && n3 == 0)b2->tab[j * grid_h + i] = 0;
			}
			else if (b->tab[j * grid_h + i] == 1) {
				int neighbors_2 = neighbors(b, 2, i, j);
				if (neighbors_2 > convert_nb) b2->tab[j * grid_h + i] = 2;
				else b2->tab[j * grid_h + i] = 1;
			}
			else if (b->tab[j * grid_h + i] == 2) {
				int neighbors_3 = neighbors(b, 3, i, j);
				if (neighbors_3 > convert_nb) b2->tab[j * grid_h + i] = 3;
				else b2->tab[j * grid_h + i] = 2;
			}
			else if (b->tab[j * grid_h + i] == 3) {
				int neighbors_1 = neighbors(b, 1, i, j);
				if (neighbors_1 > convert_nb) b2->tab[j * grid_h + i] = 1;
				else b2->tab[j * grid_h + i] = 3;
			}
		}
	}
	//paste values in main board and free the temp board
	free(b->tab);
	b->tab = b2->tab;
	free(b2);
}

void main() {
	//initialization
	srand(time(NULL));
	window_init(bg_color);
	bool display_grid = false;
	bool running = true;
	int active_color = 1;

	//create blank board
	Board *b = init_board(grid_w, grid_h);
	for (int i=0; i < grid_h - 1; i++) {
		for (int j=0; j < grid_w - 1; j++) {
			b->tab[j*grid_h + i] = rand() % 4;
		}
	}
	draw(b, display_grid, active_color);

	//loop
	while (!WindowShouldClose()) {
		//controls : change_color
		if (IsKeyPressed(KEY_KP_1)) {active_color = 1; draw(b, display_grid, active_color);}
		if (IsKeyPressed(KEY_KP_2)) {active_color = 2; draw(b, display_grid, active_color);}
		if (IsKeyPressed(KEY_KP_3)) {active_color = 3; draw(b, display_grid, active_color);}

		//controls : add cells
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			int w = GetScreenWidth();
			int h = GetScreenHeight();
			int px = min((w-400) / grid_w, (h-400) / grid_h);
			int start_x = h / 2 - px * grid_h / 2;
			int start_y = w / 2 - px * grid_w / 2;
			int mx = GetMouseX();
			int my = GetMouseY();
			if ((mx > start_y) && (my > start_x) && (mx < w - start_y) && (my < h - start_x)) {
				b->tab[grid_h * ((mx - start_y) / px) + ((my - start_x) / px)] = active_color;
				draw(b, display_grid, active_color);
			}
		}

		//controls : delete cells
		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			int w = GetScreenWidth();
			int h = GetScreenHeight();
			int px = min((w-400) / grid_w, (h-400) / grid_h);
			int start_x = h / 2 - px * grid_h / 2;
			int start_y = w / 2 - px * grid_w / 2;
			int mx = GetMouseX();
			int my = GetMouseY();
			if ((mx > start_y) && (my > start_x) && (mx < w - start_y) && (my < h - start_x)) {
				b->tab[grid_h * ((mx - start_y) / px) + ((my - start_x) / px)] = 0;
				draw(b, display_grid, active_color);
			}
		}

		//controls : pause - unpause
		if (IsKeyPressed(KEY_SPACE)) running = !running;

		//controls : switch grid
		if (IsKeyPressed(KEY_G)) {display_grid = !display_grid; draw(b, display_grid, active_color);}

		//controls : randomize grid
		if (IsKeyPressed(KEY_R)) {
			for (int i=0; i < grid_h - 1; i++) {
				for (int j=0; j < grid_w - 1; j++) {
					b->tab[j*grid_h + i] = rand() % 4;
				}
			}
			draw(b, display_grid, active_color);
		}

		//controls : clear grid
		if (IsKeyPressed(KEY_C)) {
			for (int i=0; i < grid_h - 1; i++) {
				for (int j=0; j < grid_w - 1; j++) {
					b->tab[j*grid_h + i] = 0;
				}
			}
			draw(b, display_grid, active_color);
		}

		//controls : step one frame
		if (IsKeyPressed(KEY_S)) {
			update(b);
			draw(b, display_grid, active_color);
		}

		//update board
		if (running) {
			update(b);
			draw(b, display_grid, active_color);
		}

		BeginDrawing();
		EndDrawing();
	}

	//de-initialization
	free_board(b);
}
