#include<stdio.h>
#include<stdlib.h>
#include<raylib.h>
#include<raymath.h>
#include<math.h>
#include"blocklib.h"

double MoveSpeed = 2.5f;
player PlayerMove(player Player) {

   if (Player.DidCollideY) {
     Player.DidJump = false;
   }

  if (IsKeyDown(KEY_LEFT_SHIFT)) {
    MoveSpeed = 3.75f;
    Player.IsRunning = true;
  } else {
    MoveSpeed = 2.5f;
  }

  if (IsKeyDown(KEY_RIGHT)) {
    Player.Xaccel += 0.25f;
    if ((Player.Xspeed >= MoveSpeed)  && (!IsKeyDown(KEY_LEFT)))  {
      Player.Xaccel = 0;
      Player.Xspeed = MoveSpeed;
      }
    } 

  if (IsKeyDown(KEY_LEFT)) {
    Player.Xaccel -= 0.25f;
    if (Player.Xspeed <= -MoveSpeed) {
      Player.Xaccel = 0;
      Player.Xspeed = -MoveSpeed;
      }
    }

  if (!IsKeyDown(KEY_RIGHT) &&  !IsKeyDown(KEY_LEFT)) {
      Player.Xaccel = 0;
      Player.Xspeed = Lerp(Player.Xspeed, 0.0f, 0.2f);
  }
  //NOTE:putting acceleration with the X axis movement is an very shitty idea,
  //because of floating point error this is extremely sensitive.

  if (Player.DidCollideY) {
    Player.Yspeed = 0; 
    Player.DidCollideY = false;
  } else {  
    Player.Yspeed += Player.Yaccel; 
  }
  if ((IsKeyPressed(KEY_SPACE) && (!Player.DidJump))) {
    Player.Yspeed = -7.5;
    Player.DidJump = true;
  }
 // printf("%f\n", Player.Xspeed); printf("%f\n", Player.Xaccel);  printf("\n"); printf("%f\n", P1.Yspeed);
 // printf("%f\n", P1.Yaccel); printf("\n");printf("%f\n", P1.PlayerCol.y);
 // that's for debugging, and has my ugliness watermark so don't complain
  Player.PlayerCol.x += Player.Xspeed;
  Player.Xspeed += Player.Xaccel;
  Player.PlayerCol.y += Player.Yspeed;
 return Player; 
}
/*
TTTTT H   H EEEEE       M       M      A    IIIIII  N    N
  T   H   H E           MM     MM     A A      I    N N  N
  T   HHHHH EEEEE       M M   M M    AAAAA     I    N  N N
  T   H   H E           M  M M  M   A     A    I    N   NN
  T   H   H EEEEE       M   M   M  A       A IIIII  N    N
*/
int main(void) {
InitWindow(700, 700, "SMB ripoff"); 
SetTargetFPS(60); 
player P1;
P1.PlayerCol =(Rectangle){130.0f, 612.5f-43.75f*6, 43.75f, 43.75f};
P1.Xspeed = 0;
P1.Xaccel = 0;
P1.Yspeed = 0.0f;
P1.Yaccel = 0.156f;
P1.DidJump = true;
P1.IsRunning = false;
Camera2D camera = { 0 };
camera.target = (Vector2){0, 0};
camera.offset = (Vector2){ 0.0f, 0.0f};
camera.rotation = 0.0f;
camera.zoom = 1.0f;

Rectangle GroundBlock = (Rectangle){0.0f, 612.5f, 43.75f, 43.75f};
BlockUnit* ground1 = MakeBlockStructure(GroundBlock, (Vector2){0.0f, 612.5f}, (Vector2){700.0f, 612.5f+43.75f*2});

while (!WindowShouldClose()) {
  /*       GGGG I  CCCCC
   L   OOO G       C
   L   O O G GG I  C   
   L   O O G  G I  C
   LLL OOO GGGG I  CCCCC
  */
   //here it goes the list of collisions with the blocks and structures:
   P1.DidCollideY = CheckGridCollision(P1.PlayerCol, ground1, (Vector2){ground1[0].Block.x, ground1[0].Block.y});
  
   
  double center_POV = camera.target.x + 300.0f; //300 is a magic num, somehow it works to not make the player turn back
  printf("%f\n", center_POV);
  if ((P1.Xspeed > 0) && (P1.PlayerCol.x > center_POV)) {
  camera.target = (Vector2){P1.PlayerCol.x-300, 0.0f}; // same goes here, this just works, ok?
  }
   if (P1.PlayerCol.x < camera.target.x + (43.75f / 2)) {
    P1.PlayerCol.x = camera.target.x + (43.75f / 2.0f);
  }
  //the camera "logic" is the closest thing to nonsense i've ever made
 P1 = PlayerMove(P1);
 EndMode2D();
  BeginDrawing();
  ClearBackground(BLACK);

  BeginMode2D(camera);
  DrawText("testing nga", 10, 10, 10, WHITE);
  DrawRectangleRec(P1.PlayerCol, WHITE);

  //enemies render
  for (int i = 0; i < ground1[0].BlockStructureHeight;i++) {
    for (int j = 0; j < ground1[0].BlockStructureWidth;j++) {
      if ((j % 2 == 1) != (i % 2 == 1)){
        DrawRectangleRec(ground1[(i*ground1[0].BlockStructureWidth)+j].Block, GREEN);
      } else {
        DrawRectangleRec(ground1[(i*ground1[0].BlockStructureWidth)+j].Block, BLUE);
        } 
      } 
    }
  EndDrawing();
  }
CloseWindow();
free(ground1);
return 0;
}

//THIS IS WORK IN PROGRESS
