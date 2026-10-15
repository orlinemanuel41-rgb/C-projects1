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
ColBlock,
Powerblock
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
  ColBool Collision;
  Collision.boolean = false;
  Collision.normal = (Vector2){ 0.0f, 0.0f };
  Collision.penetration = 0.0;

  if (!structure) {
    return Collision;
  }

  float blockWidth = structure[0].Block.width;
  float blockHeight = structure[0].Block.height;
  int gridWidth = structure[0].BlockStructureWidth;
  int gridHeight = structure[0].BlockStructureHeight;

  int minCol = (int)((target.x - start.x) / blockWidth);
  int maxCol = (int)((target.x + target.width - start.x) / blockWidth);
  int minRow = (int)((target.y - start.y) / blockHeight);
  int maxRow = (int)((target.y + target.height - start.y) / blockHeight);

  if (minCol < 0) minCol = 0;
  if (maxCol >= gridWidth) maxCol = gridWidth - 1;
  if (minRow < 0) minRow = 0;
  if (maxRow >= gridHeight) maxRow = gridHeight - 1;

  if (minCol > maxCol || minRow > maxRow) {
    return Collision;
  }

  float minPenetration = 1e9f;
  bool foundCollision = false;
  Vector2 bestNormal = { 0.0f, 0.0f };

  for (int i = minRow; i <= maxRow; i++) {
    for (int j = minCol; j <= maxCol; j++) {
      int index = (i * gridWidth) + j;
      Rectangle block = structure[index].Block;

      if (CheckCollisionRecs(target, block)) {
        float overlapX = fminf(target.x + target.width, block.x + block.width) - fmaxf(target.x, block.x);
        float overlapY = fminf(target.y + target.height, block.y + block.height) - fmaxf(target.y, block.y);

        if (overlapX > 0.0f && overlapY > 0.0f) {
          float currentPenetration;
          Vector2 currentNormal = { 0.0f, 0.0f };

        if (overlapX < overlapY) {
          currentPenetration = overlapX;
          currentNormal.x = (target.x + target.width / 2.0f < block.x + block.width / 2.0f) ? -1.0f : 1.0f;
        } else {
          currentPenetration = overlapY;
          currentNormal.y = (target.y + target.height / 2.0f < block.y + block.height / 2.0f) ? -1.0f : 1.0f;
        }

        if (!foundCollision || currentPenetration < minPenetration) {
          minPenetration = currentPenetration;
          bestNormal = currentNormal;
          foundCollision = true;
          }
        }
      }
    }
  }

  if (foundCollision) {
    Collision.boolean = true;
    Collision.penetration = (double)minPenetration;
    Collision.normal = bestNormal;
  }

  return Collision;
}
//this was (partially) made by AI, no wonder why it is so clean, though the thing is that some problems would be because of it,
//also atleast the logic was idea of mine, the implementation was by the clanker

