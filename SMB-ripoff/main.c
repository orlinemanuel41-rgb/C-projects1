#include<stdio.h>
#include<stdlib.h>
#include<raylib.h>
#include<raymath.h>
#include<math.h>
#include"blocklib.h"

#define BLOCKSIZE 43.75f
Color BRICKBROWN = { 146, 75, 20, 255 }; 
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
    if (Player.Xaccel > 1) {
      Player.Xaccel = 0;
    }
    Player.Xaccel += 0.25f;
    if ((Player.Xspeed >= MoveSpeed)  && (!IsKeyDown(KEY_LEFT)))  {
      Player.Xaccel = 0;
      Player.Xspeed = MoveSpeed;
      }
    } 

  if (IsKeyDown(KEY_LEFT)) {
    if (Player.Xaccel < -1) {
      Player.Xaccel = 0;
    }
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

int DrawBlockCol(BlockUnit* BlockStruct, int mode) {
  	
  //this one no matter the type it just draws a checker pattern to differentiate stuff
  if (mode == 0) {
    for (int i = 0; i < BlockStruct[0].BlockStructureHeight;i++) {
      for (int j = 0; j < BlockStruct[0].BlockStructureWidth;j++) {
        if ((j % 2 == 1) != (i % 2 == 1)){
          DrawRectangleRec(BlockStruct[(i*BlockStruct[0].BlockStructureWidth)+j].Block, GREEN);
        } else {
          DrawRectangleRec(BlockStruct[(i*BlockStruct[0].BlockStructureWidth)+j].Block, BLUE);
          } 
        } 
      }
  //this one based on blocktype assigns a different color
  } else if (mode == 1) {
    for (int i = 0; i < BlockStruct[0].BlockStructureHeight;i++) {
      for (int j = 0; j < BlockStruct[0].BlockStructureWidth;j++) {
        if (BlockStruct[(i*BlockStruct[0].BlockStructureWidth)+j].type == GroundBlock) {
          DrawRectangleRec(BlockStruct[(i*BlockStruct[0].BlockStructureWidth)+j].Block, BRICKBROWN);
        } else if (BlockStruct[(i*BlockStruct[0].BlockStructureWidth)+j].type == PowerBlock) {
          DrawRectangleRec(BlockStruct[(i*BlockStruct[0].BlockStructureWidth)+j].Block, YELLOW);
	} else if (BlockStruct[(i*BlockStruct[0].BlockStructureWidth)+j].type == NoPowerBlock) {
          DrawRectangleRec(BlockStruct[(i*BlockStruct[0].BlockStructureWidth)+j].Block, Fade(YELLOW, 0.75));
	} else if (BlockStruct[(i*BlockStruct[0].BlockStructureWidth)+j].type == BreakableBlock) {
          DrawRectangleRec(BlockStruct[(i*BlockStruct[0].BlockStructureWidth)+j].Block, BROWN);
	} else if (BlockStruct[(i*BlockStruct[0].BlockStructureWidth)+j].type == PipeCol) {
          DrawRectangleRec(BlockStruct[(i*BlockStruct[0].BlockStructureWidth)+j].Block, GREEN);
        } else if (BlockStruct[(i*BlockStruct[0].BlockStructureWidth)+j].type == EmptyBlock) {
       
        } else { DrawRectangleRec(BlockStruct[(i*BlockStruct[0].BlockStructureWidth)+j].Block, MAGENTA); }
      } 
    }  
  }   
  return 0;
}

player ResolveCollision(player P, BlockUnit* structure) {
  if (!structure) return P;

  Vector2 startPos = { structure[0].Block.x, structure[0].Block.y };
  ColBool col = CheckGridCollision(P.PlayerCol, structure, startPos);

  if (col.boolean) {
    // If we hit a breakable block then destroy it
    if (structure[col.BlockIndex].type == BreakableBlock && col.normal.y > 0) {
      structure[col.BlockIndex].type = EmptyBlock;
    }
    if (structure[col.BlockIndex].type == PowerBlock && col.normal.y > 0) {
      structure[col.BlockIndex].type = NoPowerBlock;
    }

    // Resolve the collision physics
    P.PlayerCol.x += col.normal.x * (float)col.penetration;
    P.PlayerCol.y += col.normal.y * (float)col.penetration;
    
    if (col.normal.y < 0) {
      P.DidCollideY = true;
      P.Yspeed = 0;
    } else if (col.normal.y > 0) {
      P.Yspeed = 0;
    }
    
    if (col.normal.x != 0) {
      P.Xspeed = 0;
    }
  }

  return P;
}
//shorthand for "SetBlockType"
void SBT(BlockUnit BlockStruct[], int Row, int Col, BlockType the_type) {
  //all this is pure laziness TBH
  BlockStruct[Row * BlockStruct[0].BlockStructureWidth + Col].type = the_type;
}

void SBTall(BlockUnit BlockStruct[], BlockType the_type) { 
  for(int i = 0; i < (BlockStruct[0].BlockStructureWidth*BlockStruct[0].BlockStructureHeight);i++) {
     BlockStruct[i].type = the_type;
  }
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
P1.PlayerCol = (Rectangle){130.0f, BLOCKSIZE*8, BLOCKSIZE, BLOCKSIZE};
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


//LEVEL GEN
Rectangle the_Block = (Rectangle){0.0f, 612.5f, BLOCKSIZE, BLOCKSIZE};

BlockUnit* ground1 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*0, BLOCKSIZE*14}, (Vector2){BLOCKSIZE*32, BLOCKSIZE*17});
BlockUnit* platform1 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*21, BLOCKSIZE*10}, (Vector2){BLOCKSIZE*26, BLOCKSIZE*11});
for (int i = 0; i < (ground1[0].BlockStructureWidth*ground1[0].BlockStructureHeight)-1;i++) {
  ground1[i].type = GroundBlock; //the_Block a the boilerplate of ANY BlockStructure
} 

//first bill of setting blocktypes
SBTall(platform1, BreakableBlock);
SBT(platform1, 0, 1, PowerBlock); SBT(platform1, 0, 3, PowerBlock);
ColBool ground1CB; ColBool platform1CB;
while (!WindowShouldClose()) {
  /*       GGGG I  CCCCC
   L   OOO G       C
   L   O O G GG I  C   
   L   O O G  G I  C
   LLL OOO GGGG I  CCCCC
  */
   //here it goes the list of collisions with the blocks and structures:
     //since i need collision normal and somehow it didn't came up to my mind doing a single dynamic array of arrays for the whole level 
     //now i have to do a collision per blockstruct, shitty 
  P1 = PlayerMove(P1);

  P1 = ResolveCollision(P1, ground1);
  P1 = ResolveCollision(P1, platform1); 
  printf("%f\n", P1.Xaccel);
  double center_POV = camera.target.x + 300.0f; //300 is a magic num, somehow it works to not make the player turn back
  printf("%f\n", center_POV);
  if ((P1.Xspeed > 0) && (P1.PlayerCol.x > center_POV)) {
  camera.target = (Vector2){P1.PlayerCol.x-300, 0.0f}; // same goes here, this just works, ok?
  }
   if (P1.PlayerCol.x < camera.target.x + (43.75f / 2)) {
    P1.PlayerCol.x = camera.target.x + (43.75f / 2.0f);
  }
  //the camera "logic" is the closest thing to nonsense i've ever made
  BeginDrawing();
  ClearBackground(BLACK);

  BeginMode2D(camera); 
  DrawText("testing nga", 10, 10, 10, WHITE);
  DrawRectangleRec(P1.PlayerCol, WHITE);

  //Block render
  DrawBlockCol(ground1, 1); DrawBlockCol(platform1, 1);
  EndDrawing();
  EndMode2D();
  }
CloseWindow();
free(ground1);
return 0;
}

//THIS IS WORK IN PROGRESS
