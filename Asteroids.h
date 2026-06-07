#ifndef ASTEROIDS_H
#define ASTEROIDS_H

#include "Config.h"
#include "Scores.h"
#include <math.h>

#define MAX_BULLETS 5
#define MAX_ASTEROIDS 5

struct AstBullet {
    float x, y, vx, vy;
    bool active;
    int life;
};

struct Asteroid {
    float x, y, vx, vy;
    int radius;
    bool active;
};

float shipX = 120, shipY = 120, shipAngle = -1.57;
int astScore = 0;
bool astGameOver = false;
unsigned long lastAstUpdate = 0;
bool astFirstFrame = true;
bool astTop5Achieved = false;
bool astShowHighScores = false;

AstBullet bullets[MAX_BULLETS];
Asteroid asteroids[MAX_ASTEROIDS];

// Variables para Delta Drawing (anti-parpadeo)
float oldShipX, oldShipY, oldShipAngle;
Asteroid oldAsteroids[MAX_ASTEROIDS];
AstBullet oldBullets[MAX_BULLETS];
int oldAstScore = -1;
bool astOverDrawn = false;

void initAsteroids() {
    shipX = 120; shipY = 120; shipAngle = -1.57;
    astScore = 0; astGameOver = false;
    astFirstFrame = true;
    astTop5Achieved = false;
    astShowHighScores = false;
    oldAstScore = -1;
    astOverDrawn = false;
    
    for(int i=0; i<MAX_BULLETS; i++) bullets[i].active = false;
    for(int i=0; i<MAX_ASTEROIDS; i++) asteroids[i].active = false;
    
    // Generar 2 asteroides iniciales
    for(int i=0; i<2; i++) {
        asteroids[i].active = true;
        asteroids[i].x = random(0, 240);
        asteroids[i].y = random(0, 240);
        asteroids[i].vx = (random(10, 25) / 10.0) * (random(0,2)?1:-1);
        asteroids[i].vy = (random(10, 25) / 10.0) * (random(0,2)?1:-1);
        asteroids[i].radius = random(12, 22);
    }
}

void astShoot() {
    for(int i=0; i<MAX_BULLETS; i++) {
        if(!bullets[i].active) {
            bullets[i].active = true;
            bullets[i].x = shipX + cos(shipAngle)*10;
            bullets[i].y = shipY + sin(shipAngle)*10;
            bullets[i].vx = cos(shipAngle)*6.0;
            bullets[i].vy = sin(shipAngle)*6.0;
            bullets[i].life = 40;
            tone(BUZZER_PIN, 1500, 20);
            break;
        }
    }
}

void updateAsteroids(bool btn1, bool btn2) {
    if (astGameOver) return;
    
    if (millis() - lastAstUpdate > 30) {
        lastAstUpdate = millis();
        
        // Guardar estado viejo para borrar
        oldShipX = shipX; oldShipY = shipY; oldShipAngle = shipAngle;
        for(int i=0; i<MAX_BULLETS; i++) oldBullets[i] = bullets[i];
        for(int i=0; i<MAX_ASTEROIDS; i++) oldAsteroids[i] = asteroids[i];
        
        // Rotacion (Boton 1 izq, Boton 2 der)
        if (btn1) shipAngle -= 0.2;
        if (btn2) shipAngle += 0.2;
        
        // Inercia hacia adelante siempre
        shipX += cos(shipAngle) * 2.5;
        shipY += sin(shipAngle) * 2.5;
        
        // Atrevesar bordes (Wrap)
        if(shipX < -10) shipX += 260; if(shipX > 250) shipX -= 260;
        if(shipY < -10) shipY += 260; if(shipY > 250) shipY -= 260;
        
        // Balas
        for(int i=0; i<MAX_BULLETS; i++) {
            if(bullets[i].active) {
                bullets[i].x += bullets[i].vx;
                bullets[i].y += bullets[i].vy;
                bullets[i].life--;
                if(bullets[i].life <= 0) bullets[i].active = false;
                
                if(bullets[i].x < 0) bullets[i].x += 240; if(bullets[i].x > 240) bullets[i].x -= 240;
                if(bullets[i].y < 0) bullets[i].y += 240; if(bullets[i].y > 240) bullets[i].y -= 240;
            }
        }
        
        // Asteroides
        int activeCount = 0;
        for(int i=0; i<MAX_ASTEROIDS; i++) {
            if(asteroids[i].active) {
                activeCount++;
                asteroids[i].x += asteroids[i].vx;
                asteroids[i].y += asteroids[i].vy;
                
                if(asteroids[i].x < -20) asteroids[i].x += 280; if(asteroids[i].x > 260) asteroids[i].x -= 280;
                if(asteroids[i].y < -20) asteroids[i].y += 280; if(asteroids[i].y > 260) asteroids[i].y -= 280;
                
                // Colision nave (perdonar el borde de la nave, reducir hitbox a r+2)
                float dx = asteroids[i].x - shipX;
                float dy = asteroids[i].y - shipY;
                if(dx*dx + dy*dy < (asteroids[i].radius + 2)*(asteroids[i].radius + 2)) {
                    astGameOver = true;
                    tone(BUZZER_PIN, 200, 500);
                    gameNeoColor = strip.Color(255, 0, 0); // Rojo NeoPixel
                    gameNeoTimer = millis() + 1000;
                    astTop5Achieved = addScore("aster", astScore);
                }
                
                // Colision balas
                for(int b=0; b<MAX_BULLETS; b++) {
                    if(bullets[b].active) {
                        float bx = bullets[b].x - asteroids[i].x;
                        float by = bullets[b].y - asteroids[i].y;
                        if(bx*bx + by*by < asteroids[i].radius*asteroids[i].radius) {
                            bullets[b].active = false;
                            asteroids[i].active = false;
                            astScore += 10;
                            tone(BUZZER_PIN, 2000, 50);
                            gameNeoColor = strip.Color(0, 255, 0); // Verde NeoPixel
                            gameNeoTimer = millis() + 200;
                            break;
                        }
                    }
                }
            }
        }
        
        // Aparecer asteroides
        if(activeCount < MAX_ASTEROIDS && random(0, 100) < 4) {
            for(int i=0; i<MAX_ASTEROIDS; i++) {
                if(!asteroids[i].active) {
                    asteroids[i].active = true;
                    if(random(0,2)) {
                        asteroids[i].x = random(0,2) ? 0 : 240;
                        asteroids[i].y = random(0,240);
                    } else {
                        asteroids[i].x = random(0,240);
                        asteroids[i].y = random(0,2) ? 0 : 240;
                    }
                    asteroids[i].vx = (random(10, 30) / 10.0) * (random(0,2)?1:-1);
                    asteroids[i].vy = (random(10, 30) / 10.0) * (random(0,2)?1:-1);
                    asteroids[i].radius = random(12, 22);
                    break;
                }
            }
        }
        forceRedraw = true;
    }
}

void drawShip(float x, float y, float angle, uint16_t color) {
    float x1 = x + cos(angle)*10; float y1 = y + sin(angle)*10;
    float x2 = x + cos(angle + 2.5)*8; float y2 = y + sin(angle + 2.5)*8;
    float x3 = x + cos(angle - 2.5)*8; float y3 = y + sin(angle - 2.5)*8;
    display.drawTriangle((int)x1, (int)y1, (int)x2, (int)y2, (int)x3, (int)y3, color);
}

void drawAsteroids() {
    if (astFirstFrame) {
        display.fillScreen(COLOR_BLACK);
        astFirstFrame = false;
    }
    
    if (astGameOver) {
        if (astShowHighScores) {
            drawHighScoresScreen("aster", "Asteroids Top 5");
            return;
        }
        if (!astOverDrawn) {
            display.setTextSize(3); display.setTextColor(COLOR_RED);
            display.setCursor(120 - (9*18)/2, 80); display.print("GAME OVER");
            display.setTextSize(2); display.setTextColor(COLOR_WHITE);
            display.setCursor(120 - (10*12)/2, 120); display.print("Score: "); display.print(astScore);
            display.setTextSize(1); display.setTextColor(COLOR_YELLOW);
            display.setCursor(120 - (24*6)/2, 160); display.print("B2: Reintentar B1: Salir");
            display.setCursor(120 - (28*6)/2, 175);
            if (astTop5Achieved) display.print("TOP 5! B3 Ver");
            else display.print("B3: Mejores");
            astOverDrawn = true;
        }
        return;
    }
    
    // 1. Borrar viejo
    if (abs(oldShipX - shipX) > 0.1 || abs(oldShipY - shipY) > 0.1 || abs(oldShipAngle - shipAngle) > 0.01) {
        drawShip(oldShipX, oldShipY, oldShipAngle, COLOR_BLACK);
    }
    for(int i=0; i<MAX_BULLETS; i++) {
        if(oldBullets[i].active) display.fillCircle((int)oldBullets[i].x, (int)oldBullets[i].y, 2, COLOR_BLACK);
    }
    for(int i=0; i<MAX_ASTEROIDS; i++) {
        if(oldAsteroids[i].active) display.drawCircle((int)oldAsteroids[i].x, (int)oldAsteroids[i].y, oldAsteroids[i].radius, COLOR_BLACK);
    }
    
    // 2. Dibujar nuevo
    drawShip(shipX, shipY, shipAngle, COLOR_CYAN);
    for(int i=0; i<MAX_BULLETS; i++) {
        if(bullets[i].active) display.fillCircle((int)bullets[i].x, (int)bullets[i].y, 2, COLOR_YELLOW);
    }
    for(int i=0; i<MAX_ASTEROIDS; i++) {
        if(asteroids[i].active) display.drawCircle((int)asteroids[i].x, (int)asteroids[i].y, asteroids[i].radius, COLOR_WHITE);
    }
    
    // 3. Score
    if (astScore != oldAstScore) {
        display.fillRect(100, 10, 60, 20, COLOR_BLACK);
        display.setTextSize(2); display.setTextColor(COLOR_WHITE);
        display.setCursor(110, 10); display.print(astScore);
        oldAstScore = astScore;
    }
}
#endif
