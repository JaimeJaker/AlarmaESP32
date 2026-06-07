 #ifndef FLAPPLY_BIRD_H
 #define FLAPPLY_BIRD_H

#include "Config.h"
#include "Scores.h"

float birdY = 120.0;
float birdVelocity = 0.0;
float obstacleX = 240.0;
int obstacleGapY = 120;
int score = 0;
bool gameOver = false;
bool top5Achieved = false;
bool showHighScores = false;
unsigned long lastGameUpdate = 0;

// Variables para evitar parpadeo (dibujado delta)
float oldBirdY = 120.0;
float oldObstacleX = 240.0;
int oldScore = -1;
bool firstFrame = true;
bool gameOverDrawn = false;

void initMinijuego() {
  birdY = 120.0;
  birdVelocity = 0.0;
  obstacleX = 240.0;
  obstacleGapY = random(60, 180);
  score = 0;
  gameOver = false;
  top5Achieved = false;
  showHighScores = false;
  
  oldBirdY = 120.0;
  oldObstacleX = 240.0;
  oldScore = -1;
  firstFrame = true;
  gameOverDrawn = false;
}

void updateMinijuego() {
  if (gameOver) return;
  
  if (millis() - lastGameUpdate > 30) { // ~33fps
    lastGameUpdate = millis();
    
    oldBirdY = birdY;
    oldObstacleX = obstacleX;
    
    birdVelocity += 0.5; // Gravedad
    birdY += birdVelocity;
    
    obstacleX -= 3.0; // Velocidad del obstaculo
    
    if (obstacleX < -20) {
      obstacleX = 240.0;
      obstacleGapY = random(60, 180);
      score++;
      tone(BUZZER_PIN, 1000, 50); // Sonido de punto
      gameNeoColor = strip.Color(0, 255, 0); // Verde NeoPixel
      gameNeoTimer = millis() + 200;
    }
    
    // Colisiones: hacer la hitbox invisible ligeramente mas pequena que el tubo real para ser justo
    bool hitGround = (birdY > 230 || birdY < 10);
    bool hitPipe = false;
    // Tubo en X desde obstacleX hasta obstacleX+20. Pajaro en X de 112 a 128 (r=8)
    // Perdonar 2 pixeles horizontales
    if (obstacleX < 126 && obstacleX > 94) { 
      // El hueco visual es desde obstacleGapY-35 hasta obstacleGapY+35. Pajaro r=8.
      // Perdonar 4 pixeles verticales
      if (birdY < obstacleGapY - 31 || birdY > obstacleGapY + 31) {
        hitPipe = true;
      }
    }
    
    if (hitGround || hitPipe) {
      gameOver = true;
      tone(BUZZER_PIN, 200, 500); // Sonido de choque
      gameNeoColor = strip.Color(255, 0, 0); // Rojo NeoPixel
      gameNeoTimer = millis() + 1000;
      top5Achieved = addScore("flappy", score);
    }
    forceRedraw = true; // Forzar actualizacion de pantalla
  }
}

void drawMinijuego() {
  if (firstFrame) {
    display.fillScreen(COLOR_BLACK);
    firstFrame = false;
  }
  
  if (gameOver) {
    if (showHighScores) {
      drawHighScoresScreen("flappy", "Flappy Top 5");
      return;
    }
    if (!gameOverDrawn) {
      display.setTextSize(3);
      display.setTextColor(COLOR_RED);
      display.setCursor(120 - (9*18)/2, 80);
      display.print("GAME OVER");
      
      display.setTextSize(2);
      display.setTextColor(COLOR_WHITE);
      display.setCursor(120 - (10*12)/2, 120);
      display.print("Score: "); display.print(score);
      
      display.setTextSize(1);
      display.setTextColor(COLOR_YELLOW);
      display.setCursor(120 - (24*6)/2, 160);
      display.print("B2: Reintentar B1: Salir");
      display.setCursor(120 - (28*6)/2, 175);
      if (top5Achieved) display.print("TOP 5! B3 Ver");
      else display.print("B3: Mejores");
      gameOverDrawn = true;
    }
    return;
  }
  
  int gapSize = 35; // Espacio para pasar
  
  // 1. Borrar posiciones viejas
  if ((int)oldBirdY != (int)birdY) {
    display.fillCircle(120, (int)oldBirdY, 8, COLOR_BLACK);
  }
  
  if (obstacleX > oldObstacleX) {
    // El tubo dio la vuelta (reinició a 240), borramos el tubo viejo que quedó al final izquierdo
    display.fillRect((int)oldObstacleX, 0, 20, 240, COLOR_BLACK);
  } else {
    // Borrar solo la "estela" o cola del tubo viejo para no hacer clear de toda la pantalla
    int eraseWidth = oldObstacleX - obstacleX;
    if (eraseWidth > 0 && eraseWidth <= 5) {
      display.fillRect((int)oldObstacleX + 20 - eraseWidth, 0, eraseWidth, obstacleGapY - gapSize, COLOR_BLACK);
      display.fillRect((int)oldObstacleX + 20 - eraseWidth, obstacleGapY + gapSize, eraseWidth, 240 - (obstacleGapY + gapSize), COLOR_BLACK);
    }
  }

  // 2. Dibujar posiciones nuevas
  // Dibujar Tubos
  display.fillRect((int)obstacleX, 0, 20, obstacleGapY - gapSize, COLOR_GREEN); // Tubo superior
  display.fillRect((int)obstacleX, obstacleGapY + gapSize, 20, 240 - (obstacleGapY + gapSize), COLOR_GREEN); // Tubo inferior
  
  // Dibujar Pajaro (sobre los tubos si pasan por debajo)
  display.fillCircle(120, (int)birdY, 8, COLOR_YELLOW);
  
  // 3. Dibujar Puntuacion solo si cambia
  if (score != oldScore) {
    display.fillRect(100, 15, 40, 20, COLOR_BLACK);
    display.setTextSize(2);
    display.setTextColor(COLOR_WHITE);
    display.setCursor(110, 20);
    display.print(score);
    oldScore = score;
  }
}

#endif
