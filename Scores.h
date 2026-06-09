#ifndef SCORES_H
#define SCORES_H

#include "Config.h"

#define HIGHSCORE_COUNT 5

void loadHighScores(const char* key, int scores[]) {
  // Inicializar a 0
  for (int i = 0; i < HIGHSCORE_COUNT; i++) scores[i] = 0;
  Preferences p;
  if (p.begin("scores", true)) {
    size_t need = sizeof(int) * HIGHSCORE_COUNT;
    size_t got = p.getBytes(key, scores, need);
    (void)got; // si no existe, queda el array en 0s
    p.end();
  }
}

void saveHighScores(const char* key, int scores[]) {
  Preferences p;
  if (p.begin("scores", false)) {
    p.putBytes(key, scores, sizeof(int) * HIGHSCORE_COUNT);
    p.end();
  }
}

bool addScore(const char* key, int newScore) {
  int s[HIGHSCORE_COUNT];
  loadHighScores(key, s);
  // Insertar nuevo score en orden descendente
  for (int i = 0; i < HIGHSCORE_COUNT; i++) {
    if (newScore > s[i]) {
      for (int j = HIGHSCORE_COUNT - 1; j > i; j--) s[j] = s[j-1];
      s[i] = newScore;
      saveHighScores(key, s);
      return true;
    }
  }
  // Si no entró, no cambiar
  return false;
}

void drawHighScoresScreen(const char* key, const char* title) {
  int s[HIGHSCORE_COUNT];
  loadHighScores(key, s);
  display.fillScreen(COLOR_BLACK);
  display.setTextSize(2);
  display.setTextColor(COLOR_WHITE);
  display.setCursor(120 - (strlen(title) * 6) / 2, 20);
  display.print(title);

  display.setTextSize(1);
  display.setTextColor(COLOR_YELLOW);
  for (int i = 0; i < HIGHSCORE_COUNT; i++) {
    display.setCursor(20, 50 + i * 22);
    display.print(String(i + 1) + ". " + s[i]);
  }

  display.setTextSize(1);
  display.setTextColor(COLOR_CYAN);
  display.setCursor(20, 180);
  display.print("B2: Reintentar  B1: Salir");
  display.setCursor(20, 200);
  display.print("B3: Volver");
}

#endif
