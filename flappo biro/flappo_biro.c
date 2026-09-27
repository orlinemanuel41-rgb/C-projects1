#include<raylib.h>
#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<time.h>
#include<math.h>

Rectangle biro_col = {250.0f, 300.0f, 50.0f, 50.0f};

typedef struct pipe {
Vector2 pos;
float width;
float height;
} pipe;
typedef struct {
    Rectangle top;
    Rectangle bottom;
} PipePair;

PipePair CreatePipePair(float x_pos, float gap_center_y, float gap_size) {
  float pipe_width = 80.0f;
  float screen_height = 1000.0f;
  PipePair pair;
    
  pair.top = (Rectangle){
    x_pos, 
    0, 
    pipe_width, 
    gap_center_y - (gap_size / 2.0f)
  };
    
  pair.bottom = (Rectangle){
    x_pos, 
    gap_center_y + (gap_size / 2.0f), 
    pipe_width, 
    screen_height
  };

 return pair;
}

void DrawPipePair(PipePair pair, Texture2D pipe_img) {
  float width_scale = 8.0f;
  float x_offset = (pair.top.width * (width_scale - 1.0f)) / 2.0f;

  // Scaled visual bounds
  Rectangle visual_top = {
      pair.top.x - x_offset,
      pair.top.y + 45,
      pair.top.width * width_scale,
      pair.top.height
  };

  Rectangle visual_bottom = {
      pair.bottom.x - x_offset,
      pair.bottom.y - 95,
      pair.bottom.width * width_scale,
      pair.bottom.height
  };

  // Top pipe (flipped)
  Rectangle src_top = { 0.0f, 0.0f, (float)pipe_img.width, -(float)pipe_img.height };
  DrawTexturePro(pipe_img, src_top, visual_top, (Vector2){ 0, 0 }, 0.0f, WHITE);

  // Bottom pipe
  Rectangle src_bottom = { 0.0f, 0.0f, (float)pipe_img.width, (float)pipe_img.height };
  DrawTexturePro(pipe_img, src_bottom, visual_bottom, (Vector2){ 0, 0 }, 0.0f, WHITE);
}

bool iscloseto(float target, float num, float closeness) {
  if ((target >= (num - closeness)) && (target <= (num + closeness))) {
    return true;
  }
 return false;
}
float Yspeed_biro = 1;
float biro_accel = 1.5f;

float pipes1_x = 900.0f;
float pipes1_y = 350.0f; //pretty sure all this is redundant

float pipes2_x = 1350.0f;
float pipes2_y = 350.0f;
int points = 0;
bool pointplus = false;
int dummy_var = 0;
int main(void) {

//init inside main
SetConfigFlags(FLAG_WINDOW_RESIZABLE);
InitWindow(1000, 700, "flappo biro");
InitAudioDevice();
//init Textures
Texture2D biro_img = LoadTexture("Textures/biro.png");
if (biro_img.id == 0) {
  TraceLog(LOG_ERROR, "Failed to load biro.png");
}
Texture2D pipe_img = LoadTexture("Textures/pipe.png");
if (pipe_img.id == 0) {
  TraceLog(LOG_ERROR, "Failed to load pipe.png");
}

Sound point_plus = LoadSound("Assets/pointplus.wav");
Sound flappo = LoadSound("Assets/flappo.wav");

SetSoundVolume(flappo, 0.4f);
int the_num;
bool started = false;
unsigned int counter = 0;
srand(time(NULL)); 
float the_gap1 = 210.0f;
float the_gap2 = 195.0f;
int the_wait = (rand() % 30);
SetTargetFPS(60);
while (!WindowShouldClose()) {
  if ((the_gap1 >= 150.0f) || (the_gap2 >= 150.0f)) {
  the_gap1 -= (counter / 18000.0f);
  the_gap2 -= (counter / 18000.0f); 
  }
 if (counter % 5 == 0) {
    dummy_var = 0;
  }
  PipePair pipes1 = CreatePipePair(pipes1_x, pipes1_y, the_gap1);
  PipePair pipes2 = CreatePipePair(pipes2_x, pipes2_y, the_gap2);
  if (IsKeyPressed(KEY_SPACE)) {
  started = true;
  }
  if (started) { 
  //logic should go here
  if (IsKeyPressed(KEY_SPACE) || (IsKeyPressed(KEY_Z))) {
  Yspeed_biro = -11;
  PlaySound(flappo);
  }
  if (the_wait <= 0) {
  counter++;
  }
  the_num = (((counter * 31) % 201) - 100);
  the_wait--;
  //all this "redundancy" is because srand and rand is set for only when the program gets compiled and stars 
  //so i have to do it manually for each frame, and for preventing *noticeable determinism* i just use the anterior one
  biro_col.y += Yspeed_biro;
  Yspeed_biro += biro_accel;
  pipes1_x -= 5.0f + (counter / 12000.0f);
  pipes2_x -= 5.0f + (counter / 12000.0f);
  //first pipepair
  if (pipes1_x  <= -100) {
  pipes1_x = 1050;
    if (pipes1_y <= 550) {
       pipes1_y += the_num;
    } else {pipes1_y = 350.0f;}

    if (pipes1_y >= 150) {
       pipes1_y += the_num;
    } else {pipes1_y = 350.0f;}
  } 

  //second (slower) pipepair
   if (pipes2_x  <= -100) {
  pipes2_x = 1050;
    if (pipes2_y <= 550) {
       pipes2_y += the_num;
    } else {pipes2_y = 350.0f;}

    if (pipes2_y >= 150) {
       pipes2_y += the_num;
    } else {pipes2_y = 350.0f;}
  } 

  int width = GetScreenWidth();
  int heigth = GetScreenHeight();
  pointplus = (iscloseto(250, pipes1_x, 2.75f) || iscloseto(250, pipes2_x, 2.75f));
  if (pointplus) {
     dummy_var++;
    if (dummy_var <= 1) {
      points++;
      PlaySound(point_plus);
    }
    //the redundancy with the dummy bar is since even though iscloseto is meant to prevent that it somehow happens
    //so i have to basically put that if that increments more than 1 it just doesn't get added, it resets 
    //every 3 frames using modulo
  }
  }
  if ((CheckCollisionRecs(biro_col, pipes1.top) || (CheckCollisionRecs(biro_col, pipes1.bottom)))) {
    break;		  
  } 

 if ((CheckCollisionRecs(biro_col, pipes2.top) || (CheckCollisionRecs(biro_col, pipes2.bottom)))) {
    break;		  
  } 

  if ((biro_col.y >= 750) || (biro_col.y <= -150)){
  break;
  }
  char the_str[4];
  sprintf(the_str, "%d", points);
  pointplus = false;
  //render
  BeginDrawing();
  ClearBackground(BLACK);
 // DrawRectangleRec(biro_col, WHITE);
  DrawPipePair(pipes1, pipe_img);
  DrawPipePair(pipes2, pipe_img);
  DrawTexturePro(biro_img,(Rectangle){0,0, biro_img.width, biro_img.height},(Rectangle){biro_col.x, biro_col.y, biro_col.width*2, biro_col.height*2},(Vector2){biro_col.width, biro_col.height / 2}, Yspeed_biro*0.825f, WHITE);
  //DrawText("Just testing", 20, 20, 30, WHITE);
  DrawText(the_str, 500,20,30, WHITE);
  EndDrawing();
}
UnloadTexture(biro_img);
UnloadTexture(pipe_img);
UnloadSound(flappo);
UnloadSound(point_plus);
CloseAudioDevice();
CloseWindow();
return 0;
}
