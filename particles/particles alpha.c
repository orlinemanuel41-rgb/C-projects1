#include<raylib.h>
#include<stdio.h>
#include<stdlib.h>
#include<raymath.h>
#include<math.h>

//#define PI 3.14159265358979323846

typedef struct {
Rectangle rect;
float Xspeed;
float Yspeed;
} Particle;

Rectangle a_particle = {350.0f, 350.0f, 5.0f, 5.0f};

Particle* CreateParticles(float originX,float originY, int amount) {
  Particle* Particles = malloc(amount * sizeof(Particle));
  float angle = 0;
  float radius = 0;
  for (int i = 0; i < amount;i++) {
    float ParticleX = originX + cos(angle * (PI / 180.0f)) * radius;
    float ParticleY = originY + sin(angle * (PI / 180.0f)) * radius;
    Particles[i].rect = (Rectangle){ParticleX, ParticleY, 10.0f, 10.0f}; //4.0f
    angle += 112.0f; //this is just aesthetics, i like that pattern
    radius += 1 / (amount * 0.006);
    }
  return Particles;
}

int the_amount = 100;
int main(void) {
InitWindow(700,700, "testing stuff");
SetTargetFPS(60);
Particle* the_particles = CreateParticles(350.0f, 350.0f, the_amount);

for(int i = 0; i < the_amount;i++) {
  the_particles[i].Xspeed = 4.854;
  the_particles[i].Yspeed = 1.6180;
}

while (!WindowShouldClose()) {
  //game logic should go here:
  for (int i = 0; i < the_amount;i++) {
     the_particles[i].rect.x += the_particles[i].Xspeed;
     the_particles[i].rect.y += the_particles[i].Yspeed;
  }
  for (int i = 0; i < the_amount;i++) {
    if ((the_particles[i].rect.x >= 700) || (the_particles[i].rect.x <= 0)) {
      the_particles[i].Xspeed *= -1;
    } else if ((the_particles[i].rect.y >= 700) || (the_particles[i].rect.y <= 0)) {
      the_particles[i].Yspeed *= -1; 
    }
    }
  //rendering
  BeginDrawing();
  ClearBackground(BLACK);
  DrawText("even more testing", 50, 20, 30, WHITE);
  for (int i = 0; i < the_amount;i++) {
  DrawRectangleRec(the_particles[i].rect, Fade(YELLOW, 0.5f));
  }
  EndDrawing();
  }
 
free(the_particles);
CloseWindow();
return 0;
}
//i think it is better instead of creating an pseudo-object for an entire array
//i just create a function for a single particle and then iterate to get particles
//future update: nah i didn't do it, although now i have a problem figuring out collisions
