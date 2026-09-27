#include<raylib.h>
#include<stdlib.h>
#include<stdio.h>
#include<raymath.h>
#include<math.h>

float pitch = 0;
float yaw = 0;
Vector3 direction;


Vector3 Add3DVectors(Vector3 V1, Vector3 V2) {
  float R[3];
  R[0] = V1.x + V2.x;
  R[1] = V1.y + V2.y;
  R[2] = V1.z + V2.z;
  return (Vector3){R[0], R[1], R[2]};
}

  Vector3 InvertVec(Vector3 Vector) {
  float R[3];
  R[0] = Vector.x * -1;
  R[1] = Vector.y * -1;
  R[2] = Vector.z * -1;

  return (Vector3){R[0], R[1], R[2]};
}
 

Vector3 ScalateVec(Vector3 Vector, float num) {
  float R[3];
  R[0] = Vector.x * num;
  R[1] = Vector.y * num;
  R[2] = Vector.z * num;

  return (Vector3){R[0], R[1], R[2]};
}

int main(void) {
InitWindow(700, 700, "testing 3D");
SetTargetFPS(60);

Camera3D camera = { 0 };
camera.position = (Vector3){10.0f, 10.0f, 10.0f};
camera.target = (Vector3){0.0f, 0.0f, 0.0f };
camera.up = (Vector3){0.0f, 1.0f, 0.0f };
camera.fovy = 45.0f;
camera.projection = CAMERA_PERSPECTIVE;
while (!WindowShouldClose()) {
  //logic:
  direction.x = cosf(pitch) * sinf(yaw);
  direction.y = sinf(pitch);
  direction.z = cosf(pitch) * cosf(yaw);
   
  Vector3 Forward = (Vector3){sinf(yaw), 0, cosf(yaw)};
  Vector3 Right = (Vector3){cosf(yaw), 0, -sinf(yaw)};
 
  printf("%f\n", camera.position.x);
  printf("%f\n", camera.position.y);
  printf("%f\n", camera.position.z);
  printf("\n");
  printf("%f\n", Forward.x);

  //"Super mario 64"-like camera controls TBH
  if (IsKeyDown(KEY_UP)) {
    camera.position = Add3DVectors(camera.position, ScalateVec(Forward, 0.2f));
  } if (IsKeyDown(KEY_DOWN)) {
    camera.position = Add3DVectors(camera.position, InvertVec(ScalateVec(Forward, 0.2f)));

  } if (IsKeyDown(KEY_RIGHT)) {
    camera.position = Add3DVectors(camera.position, ScalateVec(Right, -0.2f)); //it is inverted? Just mult by neg and done
  } if (IsKeyDown(KEY_LEFT)) {
    camera.position = Add3DVectors(camera.position, InvertVec(ScalateVec(Right, -0.2f)));
  } 

  if (IsKeyDown(KEY_RIGHT_SHIFT)) {
    camera.position.y -= 0.2f; 
  } 
  if (IsKeyDown(KEY_RIGHT_CONTROL)) {
    camera.position.y += 0.2f;
  }

  if (IsKeyDown(KEY_W)) {
    pitch -= 0.01;
  } if (IsKeyDown(KEY_S)) {
    pitch += 0.01;
  } if (IsKeyDown(KEY_D)) {
    yaw -= 0.01;
  } if (IsKeyDown(KEY_A)) {
    yaw += 0.01;
  }
  
  camera.target = Vector3Add(camera.position, direction);
  //i'm so smart that already existing a func i make one by my own
  //at least i'll leave it like it to remind myself of not to do so

  //render
  BeginDrawing();
  BeginMode3D(camera); //basically setting the POV  
  ClearBackground(RAYWHITE);
  DrawCube((Vector3){2.5f, 0.0f, 0.0f}, 6.0f, 1.0f, 1.0f, RED);
  DrawCube((Vector3){0.0f, 2.5f, 0.0f}, 1.0f, 6.0f, 1.0f, BLUE);
  DrawCube((Vector3){0.0f, 0.0f, 2.5f}, 1.0f, 1.0f, 6.0f, GREEN);
  DrawCube((Vector3){0.0f, 0.0f, 0.0f}, 1.5f, 1.5f, 1.5f, GRAY); 
  DrawGrid(100, 1.0f);
   
  EndMode3D();
  EndDrawing();
  }
CloseWindow();
}

