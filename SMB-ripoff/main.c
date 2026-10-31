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
          //if i do nothing it doesn't get drawn, that easy
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
BlockUnit* powerblockA1 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*17, BLOCKSIZE*10}, (Vector2){BLOCKSIZE*18, BLOCKSIZE*11});
BlockUnit* powerblockA2 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*23, BLOCKSIZE*6}, (Vector2){BLOCKSIZE*24, BLOCKSIZE*7});
BlockUnit* PipeColA1 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*29, BLOCKSIZE*12}, (Vector2){BLOCKSIZE*31, BLOCKSIZE*14});

BlockUnit* ground2 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*32, BLOCKSIZE*14}, (Vector2){BLOCKSIZE*71, BLOCKSIZE*17});
BlockUnit* PipeColB1 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*41, BLOCKSIZE*11}, (Vector2){BLOCKSIZE*43, BLOCKSIZE*14});
BlockUnit* PipeColB2 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*49, BLOCKSIZE*10}, (Vector2){BLOCKSIZE*51, BLOCKSIZE*14});
BlockUnit* PipeColB3 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*60, BLOCKSIZE*10}, (Vector2){BLOCKSIZE*62, BLOCKSIZE*14});
BlockUnit* hidPowBlock1 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*67, BLOCKSIZE*9}, (Vector2){BLOCKSIZE*68, BLOCKSIZE*10});

BlockUnit* ground3 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*73, BLOCKSIZE*14}, (Vector2){BLOCKSIZE*88, BLOCKSIZE*17});


BlockUnit* ground4 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*90, BLOCKSIZE*14}, (Vector2){BLOCKSIZE*138, BLOCKSIZE*17});
//here i stopped, this is basically from the third ground where there's the star to the second mini block stairs
//first bill of setting blocktypes
SBTall(ground1, GroundBlock); SBTall(platform1, BreakableBlock);
SBT(platform1, 0, 1, PowerBlock); SBT(platform1, 0, 3, PowerBlock); SBTall(powerblockA1, PowerBlock); SBTall(powerblockA2, PowerBlock); 
SBTall(PipeColA1, PipeCol); SBTall(ground2, GroundBlock); SBTall(PipeColB1, PipeCol); SBTall(PipeColB2, PipeCol); SBTall(PipeColB3, PipeCol); SBTall(hidPowBlock1, PowerBlock); SBTall(ground3, GroundBlock);

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

  //collision for general blockstruct
  P1 = ResolveCollision(P1, ground1);
  P1 = ResolveCollision(P1, platform1); 

  P1 = ResolveCollision(P1, ground2);
  P1 = ResolveCollision(P1, ground3);
  P1 = ResolveCollision(P1, ground4);
  //collision for indv blockstruct
  P1 = ResolveCollision(P1, powerblockA1); P1 = ResolveCollision(P1, powerblockA2); P1 = ResolveCollision(P1, PipeColA1);
  P1 = ResolveCollision(P1, PipeColB1); P1 = ResolveCollision(P1, PipeColB2); P1 = ResolveCollision(P1, PipeColB3); P1 = ResolveCollision(P1, hidPowBlock1);
  
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
  DrawBlockCol(ground1,1); DrawBlockCol(platform1,1); DrawBlockCol(powerblockA1,1); DrawBlockCol(powerblockA2,1); DrawBlockCol(PipeColA1,1);
  DrawBlockCol(ground2,1); DrawBlockCol(PipeColB1,1); DrawBlockCol(PipeColB2,1); DrawBlockCol(PipeColB3,1); 
  DrawBlockCol(ground3,1);DrawBlockCol(ground4,1);
  if (hidPowBlock1[0].type == PowerBlock) { } else {DrawBlockCol(hidPowBlock1, 1); }
  EndDrawing();
  EndMode2D();
  }
CloseWindow();
free(ground1); free(ground2); free(ground3); free(ground4); free(platform1); free(powerblockA1); free(powerblockA2); free(PipeColA1); free(PipeColB1); free(PipeColB2); free(PipeColB3); free(hidPowBlock1);
return 0;
}

//THIS IS WORK IN PROGRESS
