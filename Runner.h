#ifndef RUNNER_H
#define RUNNER_H

#include "Config.h"
#include "Scores.h"

// Player
int runnerLane = 0; // -1 (izq), 0 (centro), 1 (der)
float runnerY = 180.0;
float runnerJumpVy = 0;
bool isJumping = false;

int runScore = 0;
bool runGameOver = false;
bool runTop5Achieved = false;
bool runShowHighScores = false;
unsigned long lastRunUpdate = 0;
bool runFirstFrame = true;

// Obstacles
#define MAX_RUN_OBSTACLES 4
struct RunObstacle {
    float y;
    int lane; // -1, 0, 1
    int type; // 0: Bloque (esquivar), 1: Bajo (saltar), 2: Moneda (agarrar)
    bool active;
};
RunObstacle runObstacles[MAX_RUN_OBSTACLES];
float runSpeed = 3.0;

// Delta variables
int oldRunnerLane = 0;
float oldRunnerY = 180.0;
int oldRunScore = -1;
RunObstacle oldRunObstacles[MAX_RUN_OBSTACLES];
bool runOverDrawn = false;

void initRunner() {
    runnerLane = 0;
    runnerY = 180.0;
    runnerJumpVy = 0;
    isJumping = false;
    runScore = 0;
    runGameOver = false;
    runTop5Achieved = false;
    runShowHighScores = false;
    runFirstFrame = true;
    runOverDrawn = false;
    runSpeed = 3.0;
    oldRunScore = -1;
    
    for(int i=0; i<MAX_RUN_OBSTACLES; i++) runObstacles[i].active = false;
    // BUG FIX #3: resetear delta vars para evitar artefactos visuales al reiniciar
    oldRunnerLane = 0;
    oldRunnerY = 180.0;
    for(int i=0; i<MAX_RUN_OBSTACLES; i++) oldRunObstacles[i].active = false;
}

void runMove(int dir) {
    if (runGameOver) return;
    runnerLane += dir;
    if (runnerLane < -1) runnerLane = -1;
    if (runnerLane > 1) runnerLane = 1;
    forceRedraw = true;
}

void runJump() {
    if (runGameOver) return;
    if (!isJumping) {
        isJumping = true;
        runnerJumpVy = -8.0;
        tone(BUZZER_PIN, 800, 50);
        forceRedraw = true;
    }
}

void updateRunner() {
    if (runGameOver) return;
    
    if (millis() - lastRunUpdate > 30) {
        lastRunUpdate = millis();
        
        oldRunnerLane = runnerLane;
        oldRunnerY = runnerY;
        for(int i=0; i<MAX_RUN_OBSTACLES; i++) oldRunObstacles[i] = runObstacles[i];
        
        // Fisicas del salto
        if (isJumping) {
            runnerJumpVy += 0.7; // Gravedad
            runnerY += runnerJumpVy;
            if (runnerY >= 180.0) {
                runnerY = 180.0;
                isJumping = false;
                runnerJumpVy = 0;
            }
        }
        
        // Aumentar dificultad
        runSpeed += 0.0015;
        if (runSpeed > 10.0) runSpeed = 10.0; // BUG FIX #2: cap para evitar colisiones saltadas
        
        // Mover Obstaculos
        int activeCount = 0;
        for(int i=0; i<MAX_RUN_OBSTACLES; i++) {
            if(runObstacles[i].active) {
                activeCount++;
                runObstacles[i].y += runSpeed;
                
                // Obstaculo esquivado
                if(runObstacles[i].y > 240) {
                    runObstacles[i].active = false;
                    if(runObstacles[i].type != 2) { 
                        runScore += 5; // Puntos por esquivar
                        tone(BUZZER_PIN, 1800, 15);
                    }
                }
                
                // AABB Exacto con tolerancia de 2px para igualar el area visual
                if (runObstacles[i].active && runObstacles[i].lane == runnerLane) {
                    float pTop = runnerY - 10 + 2;
                    float pBot = runnerY + 10 - 2;
                    float oTop, oBot;
                    if (runObstacles[i].type == 0) { oTop = runObstacles[i].y - 10 + 2; oBot = runObstacles[i].y + 10 - 2; }
                    else if (runObstacles[i].type == 1) { oTop = runObstacles[i].y - 5 + 2; oBot = runObstacles[i].y + 5 - 2; }
                    else { oTop = runObstacles[i].y - 6 + 2; oBot = runObstacles[i].y + 6 - 2; } // Moneda
                    
                    if (pTop < oBot && pBot > oTop) {
                        if (runObstacles[i].type == 2) { // Moneda: agarrar
                            runObstacles[i].active = false;
                            runScore += 10;
                            tone(BUZZER_PIN, 1200, 50);
                            gameNeoColor = strip.Color(255, 255, 0);
                            gameNeoTimer = millis() + 200;
                        } else {
                            runGameOver = true;
                        }
                    }
                }
                
                    if (runGameOver) {
                        tone(BUZZER_PIN, 200, 500);
                        gameNeoColor = strip.Color(255, 0, 0); // Rojo
                        gameNeoTimer = millis() + 1000;
                        runTop5Achieved = addScore("runner", runScore);
                        break;
                    }
            }
        }
        
        // Generar Obstaculos
        if (!runGameOver && activeCount < MAX_RUN_OBSTACLES && random(0, 100) < 5) {
            bool canSpawn = true;
            for(int i=0; i<MAX_RUN_OBSTACLES; i++) { // Evitar que aparezcan muy pegados
                if(runObstacles[i].active && runObstacles[i].y < 60) canSpawn = false;
            }
            if (canSpawn) {
                for(int i=0; i<MAX_RUN_OBSTACLES; i++) {
                    if(!runObstacles[i].active) {
                        runObstacles[i].active = true;
                        runObstacles[i].y = 0;
                        runObstacles[i].lane = random(0, 3) - 1; // -1, 0, 1
                        int r = random(0, 100);
                        if (r < 30) runObstacles[i].type = 2; // 30% Moneda
                        else if (r < 65) runObstacles[i].type = 1; // 35% Bajo (saltar)
                        else runObstacles[i].type = 0; // 35% Bloque alto
                        break;
                    }
                }
            }
        }
        forceRedraw = true;
    }
}

int getLaneX(int lane) {
    if (lane == -1) return 70;
    if (lane == 0) return 120;
    return 170;
}

void drawRunner() {
    // Rastrean exactamente lo que fue pintado en pantalla
    static int lastDrawnLane = -99;
    static int lastDrawnY    = -99;

    if (runFirstFrame) {
        display.fillScreen(COLOR_BLACK);
        runFirstFrame = false;
        lastDrawnLane = -99;
        lastDrawnY    = -99;
    }

    if (runGameOver) {
        if (runShowHighScores) {
            drawHighScoresScreen("runner", "Runner Top 5");
            return;
        }
        if (!runOverDrawn) {
            display.setTextSize(3); display.setTextColor(COLOR_RED);
            display.setCursor(120 - (9*18)/2, 80); display.print("GAME OVER");
            display.setTextSize(2); display.setTextColor(COLOR_WHITE);
            display.setCursor(120 - (10*12)/2, 120); display.print("Score: "); display.print(runScore);
            display.setTextSize(1); display.setTextColor(COLOR_YELLOW);
            display.setCursor(120 - (24*6)/2, 160); display.print("B2: Reintentar B1: Salir");
            display.setCursor(120 - (28*6)/2, 175);
            if (runTop5Achieved) display.print("TOP 5! B3 Ver");
            else display.print("B3: Mejores");
            runOverDrawn = true;
        }
        return;
    }

    // 1. Borrar jugador en su posición anterior dibujada
    if (lastDrawnLane != -99) {
        if (lastDrawnLane != runnerLane || lastDrawnY != (int)runnerY) {
            display.fillRect(getLaneX(lastDrawnLane)-12, lastDrawnY-12, 24, 24, COLOR_BLACK);
        }
    }

    // 2. Borrar obstáculos en posiciones viejas (estos sí usan oldRunObstacles correctamente)
    for(int i=0; i<MAX_RUN_OBSTACLES; i++) {
        if (oldRunObstacles[i].active) {
            int ox = getLaneX(oldRunObstacles[i].lane);
            int oy = (int)oldRunObstacles[i].y;
            if (oldRunObstacles[i].type == 0) display.fillRect(ox-15, oy-15, 30, 30, COLOR_BLACK);
            else if (oldRunObstacles[i].type == 1) display.fillRect(ox-15, oy-5, 30, 10, COLOR_BLACK);
            else display.fillCircle(ox, oy, 8, COLOR_BLACK);
        }
    }

    // 3. Repintar líneas divisoras de carril (pueden borrarse con los fillRect)
    display.drawLine(95, 0, 95, 240, 0x18E3);
    display.drawLine(145, 0, 145, 240, 0x18E3);

    // 4. Dibujar jugador en nueva posición
    display.fillRect(getLaneX(runnerLane)-12, (int)runnerY-12, 24, 24, COLOR_BLUE);
    lastDrawnLane = runnerLane;
    lastDrawnY    = (int)runnerY;

    // 5. Dibujar obstáculos actuales
    for(int i=0; i<MAX_RUN_OBSTACLES; i++) {
        if (runObstacles[i].active) {
            int x = getLaneX(runObstacles[i].lane);
            int y = (int)runObstacles[i].y;
            if (runObstacles[i].type == 0) display.fillRect(x-15, y-15, 30, 30, COLOR_RED);    // Bloque Alto
            else if (runObstacles[i].type == 1) display.fillRect(x-15, y-5, 30, 10, COLOR_ORANGE); // Bajo
            else display.fillCircle(x, y, 8, COLOR_YELLOW);                                        // Moneda
        }
    }

    // 6. Puntuación (solo si cambió)
    if (runScore != oldRunScore) {
        display.fillRect(85, 8, 75, 20, COLOR_BLACK);
        display.setTextSize(2); display.setTextColor(COLOR_WHITE);
        display.setCursor(90, 10); display.print(runScore);
        oldRunScore = runScore;
    }
}

#endif
