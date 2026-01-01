#include "raylib.h"
#include "stdio.h"

int gameboard[25*25];
int SQUARE_SIZE = 30;
int wall_count = 0;
Rectangle walls[25*25];

enum {
up, left, down, right, no_direction
};


typedef struct {
    Rectangle position;
    int direction;
} Tank;

void build_walls(void) {
    wall_count = 0;  // IMPORTANT: reset

    for (int i = 0; i < 25*25; i++) {
        int row = i / 25;
        int column = i % 25;

        if (gameboard[i] == 1) {
            walls[wall_count++] = (Rectangle) {
                .x = column * SQUARE_SIZE,
                .y = row * SQUARE_SIZE,
                .width = SQUARE_SIZE,
                .height = SQUARE_SIZE
            };
        }
    }
}

void shoot_tank(Tank* tank) {
    printf("Bang bang!\n");
}

void move_tank(Tank* tank) {

    Tank tank_copy = *tank;
    float delta = GetFrameTime();
    float multiplier = delta * 100;
    
    if (tank_copy.direction == up) tank_copy.position.y -= multiplier * 1;
    else if (tank_copy.direction == down) tank_copy.position.y += multiplier * 1;

    else if (tank_copy.direction == right) tank_copy.position.x += multiplier * 1;
    else if (tank_copy.direction == left) tank_copy.position.x -= multiplier * 1;

    for (int i = 0; i < wall_count; i++) {
        if (CheckCollisionRecs(tank_copy.position, walls[i])) {
            return;
        }
    }
    *tank = tank_copy;
}

void read_base_file(void) {
    int c;
    FILE *file = fopen("base.txt", "r");
    int iteration = 0;
    if (file) {
        while ((c = getc(file)) != EOF && iteration < 25 * 25) {
            if (c == '0' || c == '1') {
                gameboard[iteration++] = c - '0'; // save the value of char not the byte value
            }
        }
        fclose(file);
    }
}

void draw_tank(Tank* tank) {
    DrawRectangle(tank->position.x, tank->position.y, 24, 24, BLACK);
}

void draw_gameboard(void) {
    int row = 0;
    int column = 0;
    for (int i = 0; i < 25*25; i++) {
        row    = i / 25;
        column = i % 25;
        int square = gameboard[i];
        int posX = column * SQUARE_SIZE;
        int posY = row * SQUARE_SIZE;
        
        if (square == 1) {
            DrawRectangle(posX, posY, SQUARE_SIZE, SQUARE_SIZE, BROWN);
        }
        else if (square == 0) {
            DrawRectangle(posX, posY, SQUARE_SIZE, SQUARE_SIZE, RAYWHITE);
        }
        column++;
    }

}

void draw_stuff(Tank* tank) {
    ClearBackground(RAYWHITE); // draw a static background

    draw_gameboard(); // draw the gameboard
    draw_tank(tank);
}

Tank init_tank() {
    Tank tank = {
    .direction = 0,
    .position.x = 32,
    .position.y = 32,
    .position.height = 24,
    .position.width = 24
};
    return tank;
}

void key_press_checking(Tank *tank_p) {
        // Set direction based on key press
        if (IsKeyDown(KEY_W)) tank_p->direction = up;
        else if (IsKeyDown(KEY_S)) tank_p->direction = down;
        else if (IsKeyDown(KEY_D)) tank_p->direction = right;
        else if (IsKeyDown(KEY_A)) tank_p->direction = left;
        else tank_p->direction = no_direction;

        // when pressing space make the tank fire its gun
        if (IsKeyPressed(KEY_SPACE)) shoot_tank(tank_p);
        // move the tank in the direction of the key press
}

int main(void)
{
    InitWindow(SQUARE_SIZE * 25, SQUARE_SIZE * 25, "This is my test program for raylib");
    
    read_base_file();
    Tank tank = init_tank();
    // make a pointer to the tank
    Tank* tank_p = &tank;
    build_walls();
    while (!WindowShouldClose())
    {
        key_press_checking(tank_p); // function for handling and listening for key presses
        move_tank(tank_p); // based on key strokes move the tank

        BeginDrawing();
        draw_stuff(tank_p); // update the graphics
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
