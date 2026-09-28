#include<stdio.h>
#include<stdlib.h>
#include<raylib.h>
#include<raymath.h>
#include<math.h>


typedef enum {
NORMAL,
BIG,
POWER
} PlayerState;

typedef struct {
Rectangle PlayerCol;
bool DidJump;
double Xspeed;
double Yspeed;
double Xaccel;
double Yaccel;
bool DidCollideX;
bool DidCollideY;
PlayerState state;
} player;

typedef struct {
Rectangle Block;
int BlockStructureWidth;
int BlockStructureHeight;
} BlockUnit;

BlockUnit* MakeBlockStructure(Rectangle Block, Vector2 Start, Vector2 End) {
  int BlockStructureWidth = (int)((End.x - Start.x) / Block.width);
  int BlockStructureHeight  = (int)((End.y - Start.y) / Block.height); 

  BlockUnit* BlockStructure = (BlockUnit*)malloc(sizeof(BlockUnit)*((BlockStructureWidth*BlockStructureHeight)));

  for (int i = 0; i < BlockStructureHeight;i++) { 
    for (int j = 0; j < BlockStructureWidth;j++) {
      BlockStructure[(i*BlockStructureWidth)+j].Block = (Rectangle){(float)Start.x + (j*Block.width),(float)Start.y + (i*Block.height),Block.width,Block.height}; //line too large TBH
      BlockStructure[(i*BlockStructureWidth)+j].BlockStructureWidth = BlockStructureWidth;
      BlockStructure[(i*BlockStructureWidth)+j].BlockStructureHeight = BlockStructureHeight;

    }
  }
  return BlockStructure;
} 

bool CheckGridCollision(Rectangle target, BlockUnit* structure, Vector2 start) {
  if (!structure) {
    return false;
  }

    float blockWidth = structure[0].Block.width;
    float blockHeight = structure[0].Block.height;
    int gridWidth = structure[0].BlockStructureWidth;
    int gridHeight = structure[0].BlockStructureHeight;

    int minCol = (int)((target.x - start.x) / blockWidth);
    int maxCol = (int)((target.x + target.width - start.x) / blockWidth);
    int minRow = (int)((target.y - start.y) / blockHeight);
    int maxRow = (int)((target.y + target.height - start.y) / blockHeight);

    if (minCol < 0) {
      minCol = 0;
    }
    if (maxCol >= gridWidth) {
      maxCol = gridWidth - 1;
    }
    if (minRow < 0) {
      minRow = 0;
    }
    if (maxRow >= gridHeight){
      maxRow = gridHeight - 1;
    }

    if (minCol > maxCol || minRow > maxRow) {
      return false;
    }

    for (int i = minRow; i <= maxRow; i++) {
      for (int j = minCol; j <= maxCol; j++) {
        int index = (i * gridWidth) + j;
        if (CheckCollisionRecs(target, structure[index].Block)) {
          return true;
        }
      }
    }

    return false;
//this was made by AI, no wonder why it is so clean, though the thing is that some problems would be because of it
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


   if (P1.DidCollideY) {
     P1.DidJump = false;
   }
  if (IsKeyDown(KEY_RIGHT)) {
    P1.Xaccel += 0.25f;
    if ((P1.Xspeed >= 2.5f)  && (!IsKeyDown(KEY_LEFT)))  {
      P1.Xaccel = 0;
      P1.Xspeed = 2.5f;
      }
    } else {
    if (P1.Xspeed > 0) {
      P1.Xaccel = 0;
      P1.Xspeed -= 0.25f;
    }
  } 

  if (IsKeyDown(KEY_LEFT)) {
    P1.Xaccel -= 0.25f;
    if (P1.Xspeed <= -2.5f) {
      P1.Xaccel = 0;
      P1.Xspeed = -2.5f;
      }
    } else {
    if (P1.Xspeed < 0) {
      P1.Xaccel = 0;
      P1.Xspeed += 0.25f;
    }
  }
  //NOTE:putting acceleration with the X axis movement is an very shitty idea,
  //because of floating point error this is extremely sensitive.

  if (P1.DidCollideY) {
    P1.Yspeed = 0; 
    P1.DidCollideY = false;
  } else {  
    P1.Yspeed += P1.Yaccel; 
  }
  if ((IsKeyPressed(KEY_SPACE) && (!P1.DidJump))) {
    P1.Yspeed = -7.5;
    P1.DidJump = true;
  }
  printf("%f\n", P1.Xspeed);
  printf("%f\n", P1.Xaccel); 
  printf("\n");
  printf("%f\n", P1.Yspeed);
  printf("%f\n", P1.Yaccel); 
  printf("\n");
  printf("%f\n", P1.PlayerCol.y);

  P1.PlayerCol.x += P1.Xspeed;
  P1.Xspeed += P1.Xaccel;
  P1.PlayerCol.y += P1.Yspeed;
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
