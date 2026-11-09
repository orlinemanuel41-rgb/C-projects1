#include<stdio.h>
#include<stdlib.h>
#include<raylib.h>
#include<raymath.h>
#include<math.h>
#include"blocklib.h"

#define BLOCKSIZE 43.75f
Color BRICKBROWN = { 146, 75, 20, 255 };
Color HARDBROWN = { 123, 63, 0, 255 };
double MoveSpeed = 2.5f;
double JumpingStrength = 7.5f;
player PlayerMove(player Player) {

   if (Player.DidCollideY) {
     Player.DidJump = false;
   }

  if (IsKeyDown(KEY_LEFT_SHIFT)) {
    MoveSpeed = 3.75f;
    Player.IsRunning = true;
    //this would MERELY be a thing as long as i'm debugging and building the game, inmideately after that i will eliminate this "godspeed cheat"
  } else if (IsKeyDown(KEY_LEFT_CONTROL)) {
    MoveSpeed = 11.25f;
    JumpingStrength = 10.5f;
    Player.IsRunning = true;
  } else {
    MoveSpeed = 2.5f;
    JumpingStrength = 7.5f;
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
    Player.Yspeed = -JumpingStrength;
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
	} else if (BlockStruct[(i*BlockStruct[0].BlockStructureWidth)+j].type == HardBlock) {
          DrawRectangleRec(BlockStruct[(i*BlockStruct[0].BlockStructureWidth)+j].Block, HARDBROWN);
        } else if (BlockStruct[(i*BlockStruct[0].BlockStructureWidth)+j].type == EmptyBlock) {
          //if i do nothing it doesn't get drawn, that easy
        } else { DrawRectangleRec(BlockStruct[(i*BlockStruct[0].BlockStructureWidth)+j].Block, MAGENTA); }
      } 
    }  
  }  
  //here it must be the renderer of the sprites  in the mode 2,  
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
P1.Yaccel = 0.156f; //magic num, well no, this is just set by the thumb 
P1.DidJump = true;
P1.IsRunning = false;
Camera2D camera = { 0 };
camera.target = (Vector2){0, 0};
camera.offset = (Vector2){ 0.0f, 0.0f};
camera.rotation = 0.0f;
camera.zoom = 1.0f;


//LEVEL GEN
Rectangle the_Block = (Rectangle){0.0f, 612.5f, BLOCKSIZE, BLOCKSIZE};

//first part (start)
BlockUnit* ground1 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*0, BLOCKSIZE*14}, (Vector2){BLOCKSIZE*32, BLOCKSIZE*17});
BlockUnit* platformA1 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*21, BLOCKSIZE*10}, (Vector2){BLOCKSIZE*26, BLOCKSIZE*11});
BlockUnit* powerblockA1 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*17, BLOCKSIZE*10}, (Vector2){BLOCKSIZE*18, BLOCKSIZE*11});
BlockUnit* powerblockA2 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*23, BLOCKSIZE*6}, (Vector2){BLOCKSIZE*24, BLOCKSIZE*7});
BlockUnit* PipeColA1 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*29, BLOCKSIZE*12}, (Vector2){BLOCKSIZE*31, BLOCKSIZE*14});


//second (pipes)
BlockUnit* ground2 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*32, BLOCKSIZE*14}, (Vector2){BLOCKSIZE*71, BLOCKSIZE*17});
BlockUnit* PipeColB1 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*41, BLOCKSIZE*11}, (Vector2){BLOCKSIZE*43, BLOCKSIZE*14});
BlockUnit* PipeColB2 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*49, BLOCKSIZE*10}, (Vector2){BLOCKSIZE*51, BLOCKSIZE*14});
BlockUnit* PipeColB3 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*59, BLOCKSIZE*10}, (Vector2){BLOCKSIZE*61, BLOCKSIZE*14});
BlockUnit* hidPowBlock1 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*67, BLOCKSIZE*9}, (Vector2){BLOCKSIZE*68, BLOCKSIZE*10});


//third (midlevel 1)
BlockUnit* ground3 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*73, BLOCKSIZE*14}, (Vector2){BLOCKSIZE*88, BLOCKSIZE*17});
BlockUnit* platformB1 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*80, BLOCKSIZE*10}, (Vector2){BLOCKSIZE*83, BLOCKSIZE*11});
BlockUnit* platformB2 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*83, BLOCKSIZE*6}, (Vector2){BLOCKSIZE*92, BLOCKSIZE*7});
BlockUnit* platformB3 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*95, BLOCKSIZE*6}, (Vector2){BLOCKSIZE*99, BLOCKSIZE*7});


//fourth (midlevel 2)
BlockUnit* ground4 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*90, BLOCKSIZE*14}, (Vector2){BLOCKSIZE*156, BLOCKSIZE*17});
BlockUnit* IndvBlock1 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*98, BLOCKSIZE*10}, (Vector2){BLOCKSIZE*99, BLOCKSIZE*11});
BlockUnit* platformC1 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*104, BLOCKSIZE*10}, (Vector2){BLOCKSIZE*106, BLOCKSIZE*11});
BlockUnit* PowBlockA1 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*110, BLOCKSIZE*10}, (Vector2){BLOCKSIZE*111, BLOCKSIZE*11});
BlockUnit* PowBlockA2 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*113, BLOCKSIZE*10}, (Vector2){BLOCKSIZE*114, BLOCKSIZE*11});
BlockUnit* PowBlockA3 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*113, BLOCKSIZE*6}, (Vector2){BLOCKSIZE*114, BLOCKSIZE*7});
BlockUnit* PowBlockA4 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*116, BLOCKSIZE*10}, (Vector2){BLOCKSIZE*117, BLOCKSIZE*11});

BlockUnit* IndvBlock2 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*121, BLOCKSIZE*10}, (Vector2){BLOCKSIZE*122, BLOCKSIZE*11});
BlockUnit* platformC2 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*124, BLOCKSIZE*6}, (Vector2){BLOCKSIZE*126, BLOCKSIZE*7});
BlockUnit* platformC3 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*130, BLOCKSIZE*6}, (Vector2){BLOCKSIZE*134, BLOCKSIZE*7});
BlockUnit* platformC4 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*131, BLOCKSIZE*10}, (Vector2){BLOCKSIZE*133, BLOCKSIZE*11});

BlockUnit* StairA1 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*137, BLOCKSIZE*13}, (Vector2){BLOCKSIZE*141, BLOCKSIZE*14});
BlockUnit* StairA2 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*138, BLOCKSIZE*12}, (Vector2){BLOCKSIZE*141, BLOCKSIZE*13});
BlockUnit* StairA3 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*139, BLOCKSIZE*11}, (Vector2){BLOCKSIZE*141, BLOCKSIZE*12});
BlockUnit* StairA4 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*140, BLOCKSIZE*10}, (Vector2){BLOCKSIZE*141, BLOCKSIZE*11});

BlockUnit* StairB1 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*143, BLOCKSIZE*13}, (Vector2){BLOCKSIZE*147, BLOCKSIZE*14});
BlockUnit* StairB2 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*143, BLOCKSIZE*12}, (Vector2){BLOCKSIZE*146, BLOCKSIZE*13});
BlockUnit* StairB3 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*143, BLOCKSIZE*11}, (Vector2){BLOCKSIZE*145, BLOCKSIZE*12});
BlockUnit* StairB4 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*143, BLOCKSIZE*10}, (Vector2){BLOCKSIZE*144, BLOCKSIZE*11});

BlockUnit* StairC1 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*151, BLOCKSIZE*13}, (Vector2){BLOCKSIZE*156, BLOCKSIZE*14});
BlockUnit* StairC2 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*152, BLOCKSIZE*12}, (Vector2){BLOCKSIZE*156, BLOCKSIZE*13});
BlockUnit* StairC3 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*153, BLOCKSIZE*11}, (Vector2){BLOCKSIZE*156, BLOCKSIZE*12});
BlockUnit* StairC4 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*154, BLOCKSIZE*10}, (Vector2){BLOCKSIZE*156, BLOCKSIZE*11});


//end of the level
BlockUnit* ground5 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*158, BLOCKSIZE*14},  (Vector2){BLOCKSIZE*211, BLOCKSIZE*17});
BlockUnit* StairD1 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*158, BLOCKSIZE*13}, (Vector2){BLOCKSIZE*162, BLOCKSIZE*14});
BlockUnit* StairD2 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*158, BLOCKSIZE*12}, (Vector2){BLOCKSIZE*161, BLOCKSIZE*13});
BlockUnit* StairD3 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*158, BLOCKSIZE*11}, (Vector2){BLOCKSIZE*160, BLOCKSIZE*12});
BlockUnit* StairD4 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*158, BLOCKSIZE*10}, (Vector2){BLOCKSIZE*159, BLOCKSIZE*11});

BlockUnit* PipeColC1 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*166, BLOCKSIZE*12}, (Vector2){BLOCKSIZE*168, BLOCKSIZE*14});
BlockUnit* platformD1 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*171, BLOCKSIZE*10}, (Vector2){BLOCKSIZE*175, BLOCKSIZE*11});
BlockUnit* PipeColC2 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*183, BLOCKSIZE*12}, (Vector2){BLOCKSIZE*185, BLOCKSIZE*14});

BlockUnit* StairE1 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*185, BLOCKSIZE*13}, (Vector2){BLOCKSIZE*195, BLOCKSIZE*14});
BlockUnit* StairE2 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*186, BLOCKSIZE*12}, (Vector2){BLOCKSIZE*195, BLOCKSIZE*13});
BlockUnit* StairE3 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*187, BLOCKSIZE*11}, (Vector2){BLOCKSIZE*195, BLOCKSIZE*12});
BlockUnit* StairE4 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*188, BLOCKSIZE*10}, (Vector2){BLOCKSIZE*195, BLOCKSIZE*11});
BlockUnit* StairE5 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*189, BLOCKSIZE*9}, (Vector2){BLOCKSIZE*195, BLOCKSIZE*10});
BlockUnit* StairE6 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*190, BLOCKSIZE*8}, (Vector2){BLOCKSIZE*195, BLOCKSIZE*9});
BlockUnit* StairE7 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*191, BLOCKSIZE*7}, (Vector2){BLOCKSIZE*195, BLOCKSIZE*8});
BlockUnit* StairE8 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*192, BLOCKSIZE*6}, (Vector2){BLOCKSIZE*195, BLOCKSIZE*7});
BlockUnit* StairE9 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*193, BLOCKSIZE*5}, (Vector2){BLOCKSIZE*195, BLOCKSIZE*6});

//the pole hardblock
BlockUnit* IndvBlock3 = MakeBlockStructure(the_Block, (Vector2){BLOCKSIZE*204, BLOCKSIZE*13}, (Vector2){BLOCKSIZE*205, BLOCKSIZE*14});

//LEVEEEEELL BLOCKS DONEEEEEEE!!!!!!!! After almost 3 weeks doing nothing but writing it
//55  block structures for the whole level TBH :(
//NOTE: since this is meant to be the top left corner for setting the measurements of each blockstructure the x coordinate must go ascending and the y coordinate descending, if not then it would be instant crash

//first bill of setting blocktype
SBTall(ground1, GroundBlock); SBTall(platformA1, BreakableBlock);
SBT(platformA1, 0, 1, PowerBlock); SBT(platformA1, 0, 3, PowerBlock); SBTall(powerblockA1, PowerBlock); SBTall(powerblockA2, PowerBlock); 
SBTall(PipeColA1, PipeCol); SBTall(ground2, GroundBlock); SBTall(PipeColB1, PipeCol); SBTall(PipeColB2, PipeCol); SBTall(PipeColB3, PipeCol); SBTall(hidPowBlock1, PowerBlock); SBTall(ground3, GroundBlock); SBTall(ground4, GroundBlock); SBTall(platformB1, BreakableBlock); SBT(platformB1, 0, 1, PowerBlock); SBTall(platformB2, BreakableBlock); SBTall(platformB3, BreakableBlock); SBT(platformB3, 0, 3, PowerBlock); SBTall(IndvBlock1, BreakableBlock); SBTall(PowBlockA1, PowerBlock); SBTall(PowBlockA2, PowerBlock); SBTall(PowBlockA3, PowerBlock); SBTall(PowBlockA4, PowerBlock); SBTall(IndvBlock2, BreakableBlock); SBTall(platformC2, BreakableBlock); SBTall(platformC3, BreakableBlock); SBT(platformC3, 0, 1, PowerBlock); SBT(platformC3, 0, 2, PowerBlock);
SBTall(platformC4, BreakableBlock); SBTall(platformC1, BreakableBlock); SBTall(ground5, GroundBlock); SBTall(PipeColC1, PipeCol); 
SBTall(platformD1, BreakableBlock); SBT(platformD1, 0, 2, PowerBlock); SBTall(PipeColC2, PipeCol); SBTall(IndvBlock3, HardBlock);

//Stairs blockstructs must be separated from others, why? Waaaay too much of the same, code turns into nonsense
SBTall(StairA1, HardBlock); SBTall(StairA2, HardBlock);  SBTall(StairA3, HardBlock); SBTall(StairA4, HardBlock); SBTall(StairB1, HardBlock); SBTall(StairB2, HardBlock);  SBTall(StairB3, HardBlock); SBTall(StairB4, HardBlock); SBTall(StairC1, HardBlock); SBTall(StairC2, HardBlock);  SBTall(StairC3, HardBlock); SBTall(StairC4, HardBlock);
SBTall(StairD1, HardBlock); SBTall(StairD2, HardBlock);  SBTall(StairD3, HardBlock); SBTall(StairD4, HardBlock);
SBTall(StairE1, HardBlock); SBTall(StairE2, HardBlock); SBTall(StairE3, HardBlock); SBTall(StairE4, HardBlock);
SBTall(StairE5, HardBlock); SBTall(StairE6, HardBlock); SBTall(StairE7, HardBlock); SBTall(StairE8, HardBlock); SBTall(StairE9, HardBlock);

  while (!WindowShouldClose()) {
  /*       GGGG I  CCCC
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
  P1 = ResolveCollision(P1, platformA1); 

  P1 = ResolveCollision(P1, ground2);
  P1 = ResolveCollision(P1, ground3);
  P1 = ResolveCollision(P1, ground4);
  //collision for indv blockstruct
  P1 = ResolveCollision(P1, powerblockA1); P1 = ResolveCollision(P1, powerblockA2); P1 = ResolveCollision(P1, PipeColA1);
  P1 = ResolveCollision(P1, PipeColB1); P1 = ResolveCollision(P1, PipeColB2); P1 = ResolveCollision(P1, PipeColB3);
  P1 = ResolveCollision(P1, hidPowBlock1); P1 = ResolveCollision(P1, platformB1); P1 = ResolveCollision(P1, platformB2); 
  P1 = ResolveCollision(P1, platformB3); P1 = ResolveCollision(P1, IndvBlock1); P1 = ResolveCollision(P1, PowBlockA1);
  P1 = ResolveCollision(P1, platformC1); P1 = ResolveCollision(P1, PowBlockA2); P1 = ResolveCollision(P1, PowBlockA3);
  P1 = ResolveCollision(P1, PowBlockA4); P1 = ResolveCollision(P1, IndvBlock2); P1 = ResolveCollision(P1, platformC2);
  P1 = ResolveCollision(P1, platformC3); P1 = ResolveCollision(P1, platformC4); P1 = ResolveCollision(P1, StairA1);
  P1 = ResolveCollision(P1, StairA2);    P1 = ResolveCollision(P1, StairA3);    P1 = ResolveCollision(P1, StairA4);
  P1 = ResolveCollision(P1, StairB1);    P1 = ResolveCollision(P1, StairB2);    P1 = ResolveCollision(P1, StairB3);  
  P1 = ResolveCollision(P1, StairB4);    P1 = ResolveCollision(P1, StairC1);    P1 = ResolveCollision(P1, StairC2); 
  P1 = ResolveCollision(P1, StairC3);    P1 = ResolveCollision(P1, StairC4);    P1 = ResolveCollision(P1, ground5);
  P1 = ResolveCollision(P1, StairD1);    P1 = ResolveCollision(P1, StairD2);    P1 = ResolveCollision(P1, StairD3); 
  P1 = ResolveCollision(P1, StairD4);    P1 = ResolveCollision(P1, PipeColC1);  P1 = ResolveCollision(P1, platformD1);
  P1 = ResolveCollision(P1, PipeColC2);  P1 = ResolveCollision(P1, IndvBlock3);


  P1 = ResolveCollision(P1, StairE1); P1 = ResolveCollision(P1, StairE2); P1 = ResolveCollision(P1, StairE3);
  P1 = ResolveCollision(P1, StairE4); P1 = ResolveCollision(P1, StairE5); P1 = ResolveCollision(P1, StairE6);
  P1 = ResolveCollision(P1, StairE7); P1 = ResolveCollision(P1, StairE8); P1 = ResolveCollision(P1, StairE9);

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
  DrawText("testing", 10, 10, 10, WHITE);
  DrawRectangleRec(P1.PlayerCol, WHITE);

  //Block render
  DrawBlockCol(ground1,1); DrawBlockCol(platformA1,1); DrawBlockCol(powerblockA1,1); DrawBlockCol(powerblockA2,1); DrawBlockCol(PipeColA1,1);
  DrawBlockCol(ground2,1); DrawBlockCol(PipeColB1,1); DrawBlockCol(PipeColB2,1); DrawBlockCol(PipeColB3,1); 
  DrawBlockCol(ground3,1);DrawBlockCol(ground4,1); DrawBlockCol(platformB1, 1); DrawBlockCol(platformB2, 1); DrawBlockCol(platformB3, 1);
  DrawBlockCol(IndvBlock1,1);DrawBlockCol(platformC1,1); DrawBlockCol(PowBlockA1,1); DrawBlockCol(PowBlockA2,1);DrawBlockCol(PowBlockA3,1);
  DrawBlockCol(PowBlockA4,1);DrawBlockCol(IndvBlock2,1); DrawBlockCol(platformC2,1);DrawBlockCol(platformC3,1);DrawBlockCol(platformC4,1); 
  DrawBlockCol(StairA1,1); DrawBlockCol(StairA2,1); DrawBlockCol(StairA3,1); DrawBlockCol(StairA4,1);DrawBlockCol(StairB1,1); 
  DrawBlockCol(StairB2,1); DrawBlockCol(StairB3,1); DrawBlockCol(StairB4,1); DrawBlockCol(StairC1,1); DrawBlockCol(StairC2,1); 
  DrawBlockCol(StairC3,1); DrawBlockCol(StairC4,1); DrawBlockCol(ground5,1); DrawBlockCol(StairD1,1);
  DrawBlockCol(StairD2,1); DrawBlockCol(StairD3,1); DrawBlockCol(StairD4,1); DrawBlockCol(PipeColC1,1); DrawBlockCol(platformD1,1); 
  DrawBlockCol(PipeColC2,1); DrawBlockCol(StairE1,1); DrawBlockCol(StairE2,1); DrawBlockCol(StairE3,1); DrawBlockCol(StairE4,1);
  DrawBlockCol(StairE5,1); DrawBlockCol(StairE6,1); DrawBlockCol(StairE7,1); DrawBlockCol(StairE8,1); DrawBlockCol(StairE9,1);
  DrawBlockCol(IndvBlock3,1);
  if (hidPowBlock1[0].type == PowerBlock) { } else {DrawBlockCol(hidPowBlock1, 1); }
  EndDrawing();
  EndMode2D();
  }
CloseWindow();
free(ground1); free(ground2); free(ground3); free(ground4); free(platformA1); free(powerblockA1); free(powerblockA2); free(PipeColA1);
free(PipeColB1); free(PipeColB2); free(PipeColB3); free(hidPowBlock1); free(platformB1); free(platformB2); free(platformB3); free(IndvBlock1);
free(platformC1); free(PowBlockA1); free(PowBlockA2); free(PowBlockA3); free(PowBlockA4); free(IndvBlock2); free(platformC2); free(platformC3);
free(platformC4);
free(StairA1); free(StairA2); free(StairA3); free(StairA4); free(StairB1); free(StairB2); free(StairB3); free(StairB4);
free(StairC1); free(StairC2); free(StairC3); free(StairC4); free(StairD1); free(StairD2); free(StairD3); free(StairD4);
free(StairE1); free(StairE2); free(StairE3); free(StairE4); free(StairE5); free(StairE6); free(StairE7); free(StairE8);
free(StairE9); free(PipeColC1); free(platformD1); free(PipeColC2); free(IndvBlock3);  
return 0;
}

//THIS IS WORK IN PROGRESS
