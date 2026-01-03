#include "raylib.h"
#include "stdlib.h"
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
} Shot;

typedef struct {
    Shot *items;
    size_t count;
    size_t capacity;
} Dyn_array;

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

void shoot_tank(Tank* tank, Dyn_array *shots) {
    int shot_direction = no_direction;

    shot_direction = tank->direction;

    Shot shot = {
    .direction = shot_direction,
    .position.x = tank->position.x,
    .position.y = tank->position.y,
    .position.height = 10,
    .position.width = 10

};

    if (shots->count >= shots->capacity) {
        if (shots->capacity == 0) shots->capacity = 8;
        else shots->capacity *= 2;
        shots->items = realloc(shots->items, shots->capacity * sizeof(*shots->items));
    }
    shots->items[shots->count++] = shot; 
}

void move_tank(Tank* tank, float delta) {

    Tank tank_copy = *tank;
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

void draw_shots(Dyn_array *shots) {
    for ( int i = 0; i < shots->count; i++ ) {
        Shot *shot = &shots->items[i];
        int posX = shot->position.x;
        int posY = shot->position.y;
        int width = shot->position.width;
        
        int height = shot->position.height;
        DrawRectangle(posX, posY, width, height, RED);
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

void draw_stuff(Tank* tank, Dyn_array *shots) {
    ClearBackground(RAYWHITE); // draw a static background

    draw_gameboard(); // draw the gameboard
    draw_tank(tank);
    draw_shots(shots);


    int fps = GetFPS();
    int posX = 10;
    int posY = 10;
    int font_size = 12;
    DrawText(TextFormat("%d", fps), posX, posY, font_size, BLACK);
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

void key_press_checking(Tank *tank_p, Dyn_array* shots) {
        // Set direction based on key press
        if (IsKeyDown(KEY_W)) tank_p->direction = up;
        else if (IsKeyDown(KEY_S)) tank_p->direction = down;
        else if (IsKeyDown(KEY_D)) tank_p->direction = right;
        else if (IsKeyDown(KEY_A)) tank_p->direction = left;
        else tank_p->direction = no_direction;

        // when pressing space make the tank fire its gun
        if (IsKeyPressed(KEY_SPACE)) shoot_tank(tank_p, shots);
        // move the tank in the direction of the key press
}

void update_array_order(int idx, Dyn_array *shots) {
    for (int i = idx; i < shots->count; i++) {
        shots->items[i] = shots->items[i+1];
    }
}

void update_shots(Dyn_array* shots, float delta) {
    size_t i = 0;

    while (i < shots->count) {
        Shot shot_copy = shots->items[i];
        float shot_speed = 200;
        float multiplier = delta * shot_speed;

        if (shot_copy.direction == left)  shot_copy.position.x -= multiplier;
        else if (shot_copy.direction == right) shot_copy.position.x += multiplier;
        else if (shot_copy.direction == up)    shot_copy.position.y -= multiplier;
        else if (shot_copy.direction == down)  shot_copy.position.y += multiplier;

        bool hit_wall = false;
        for (int j = 0; j < wall_count; j++) {
            if (CheckCollisionRecs(shot_copy.position, walls[j])) {
                hit_wall = true;
                break;
            }
        }

        if (hit_wall) {
            // remove shot i
            for (size_t k = i + 1; k < shots->count; k++) {
                shots->items[k - 1] = shots->items[k];
            }
            shots->count--;
            // do NOT increment i
        } else {
            shots->items[i] = shot_copy;
            i++; // only advance if no deletion
        }
    }
}

int main(void)
{
    InitWindow(SQUARE_SIZE * 25, SQUARE_SIZE * 25, "This is my test program for raylib");
    SetTargetFPS(60);

    // make an array for the bullets
    Dyn_array shots = {0};
    
    read_base_file();
    Tank tank = init_tank();
    // make a pointer to the tank
    Tank* tank_p = &tank;
    build_walls();
    while (!WindowShouldClose())
    {
        float delta = GetFrameTime();
        key_press_checking(tank_p, &shots); // function for handling and listening for key presses
        update_shots(&shots, delta);
        move_tank(tank_p, delta); // based on key strokes move the tank

        BeginDrawing(); draw_stuff(tank_p, &shots); // update the graphics
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
