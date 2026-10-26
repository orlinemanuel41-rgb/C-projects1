#include<stdlib.h>
#include<stdio.h>
#include<raylib.h>
#include<raymath.h>
#include<math.h>


typedef enum {
NORMAL,
BIG,
POWER
} PlayerState;

typedef enum {
ColBlock = 0, //only used for collision, while drawing irrelevant
PowerBlock,
NoPowerBlock,
GroundBlock,
BreakableBlock,
PipeCol, //i am NOT about to make that the pipe leads to another scenario since this would be just a single scenario
	 //although honestly i can do that easily with a couple if statements but i'm lazy
EmptyBlock
} BlockType;

typedef struct {
Rectangle PlayerCol;
bool DidJump;
double Xspeed;
double Yspeed;
double Xaccel;
double Yaccel;
bool DidCollideX;
bool DidCollideY;
bool IsRunning;
bool DidFunc; //func is basically to break breakable blocks and to release the power up or transform in non-powerblocks
PlayerState state;
} player;

typedef struct {
Rectangle Block;
int BlockStructureWidth;
int BlockStructureHeight;
BlockType type;
} BlockUnit;

typedef struct {
bool boolean;
Vector2 normal;
double penetration;
int BlockIndex;
} ColBool;

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
ColBool CheckGridCollision(Rectangle target, BlockUnit* structure, Vector2 start) {
//since this was made by the clanker i will fill it with explanatory comments from what i understood by reading

  //initialization
  ColBool Collision;
  Collision.boolean = false;
  Collision.normal = (Vector2){ 0.0f, 0.0f };
  Collision.penetration = 0.0;

  //if there is none to check obviously you do nothing
  if (!structure) {
    return Collision;
  }

  //doing the shorthands for the attributes
  float blockWidth = structure[0].Block.width;
  float blockHeight = structure[0].Block.height;
  int gridWidth = structure[0].BlockStructureWidth;
  int gridHeight = structure[0].BlockStructureHeight;

  //basically turning the target's  (player and entities) coordinate position into grid position 
  //and as i'm truncating also turning it into the gridblocks the player intersects
  //for instance if player at (90,90) it intersects gridblocks (1,1), (1,2), (2,1), (2,2) since the range would be x from ((90-43.75) = 46.25) / 43.75
  //truncated 1 to 90 / 43.75, 90 since width is added, or basically truncated 2, exact same with y so x from 1 to 2, y from 1 to 2
  int minCol = (int)((target.x - start.x) / blockWidth);
  int maxCol = (int)((target.x + target.width - start.x) / blockWidth);
  int minRow = (int)((target.y - start.y) / blockHeight);
  int maxRow = (int)((target.y + target.height - start.y) / blockHeight);

  //if the player is colliding negatively with a block (penetrating) this turns 0 
  if (minCol < 0) minCol = 0;
  if (maxCol >= gridWidth) maxCol = gridWidth - 1;
  if (minRow < 0) minRow = 0;
  if (maxRow >= gridHeight) maxRow = gridHeight - 1;

  // if min more than max on both, contradiction so nothing happens
  if (minCol > maxCol || minRow > maxRow) {
    return Collision;
  }

  //minimal penetration cuz if not then float error, also init foundcol
  float minPenetration = 1e9f;
  bool foundCollision = false;
  Vector2 bestNormal = { 0.0f, 0.0f };
  int bestIndex = -1; 
  //the check loop, basically checks if in the grid there is already something in the range the cube overlaps, 
  //with the last example since it is <= not < the overlap are registeded as (1,1), (2,1), (1,2) and (2,2) which are the correct ones
  for (int i = minRow; i <= maxRow; i++) {
    for (int j = minCol; j <= maxCol; j++) {

      //translation so can get into my pseudo 2D matrix
      int index = (i * gridWidth) + j;

      if (structure[index].type == EmptyBlock) { continue; }
      Rectangle block = structure[index].Block;

      //our protagonist Check collisions rect as an if statement since we also want penetration and normal
      if (CheckCollisionRecs(target, block)) {

	//when detects collision with block the smallest (if between the player and block) gets subtracted from the one the furthest 
	//if either the player or block, so ignores penetration direction, kinda abstract to think why but works  
        float overlapX = fminf(target.x + target.width, block.x + block.width) - fmaxf(target.x, block.x);
        float overlapY = fminf(target.y + target.height, block.y + block.height) - fmaxf(target.y, block.y);

	//if there IS overlap outputs the normal and initialices penetration
        if (overlapX > 0.0f && overlapY > 0.0f) {
          float currentPenetration;
          Vector2 currentNormal = { 0.0f, 0.0f };

	//if horizontal overlap bigger than vertical, outputs only the horizontal penetration and viceversa, so whe always get the direct normal
        if (overlapX < overlapY) {
          currentPenetration = overlapX;
          currentNormal.x = (target.x + target.width / 2.0f < block.x + block.width / 2.0f) ? -1.0f : 1.0f;
        } else {
          currentPenetration = overlapY;
          currentNormal.y = (target.y + target.height / 2.0f < block.y + block.height / 2.0f) ? -1.0f : 1.0f;
        } //idk why clanker did it like it if with both works just fine but whatever, also ternation (? condition_if_true : this_happens) because idk i       it just can't be negative, it would be simpler penetration magnifies normal (and the normal from entities not block)
       //and get penetration with pythagoras but who knows

        //if it hasn't found a collision or the penetration is less than the min the min is the current and the best normal is the current 
	//kind of redundant but who knows
        if (!foundCollision || currentPenetration < minPenetration) {
          minPenetration = currentPenetration;
          bestNormal = currentNormal;
	  bestIndex = index;
          foundCollision = true;
          }
        }
      }
    }
  }
  //if there is collision (after all big ol' if statement, we set that there is, indeed a collision, the penetration from min, normal and the index
  if (foundCollision) {
    Collision.boolean = true;
    Collision.penetration = (double)minPenetration;
    Collision.normal = bestNormal;
    Collision.BlockIndex = bestIndex;
  }

  return Collision;
}
//this was (partially) made by AI, no wonder why it is so clean, though the thing is that some problems would be because of it,
//also atleast the logic was idea of mine, the implementation was by the clanker


