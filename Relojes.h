#ifndef RELOJES_H
#define RELOJES_H

#include "Config.h"
#include "Alarms.h"
#include <pgmspace.h>

// Numerales romanos en flash (compartidos por todos los usos)
static const char* const roman_numerals[] PROGMEM = {
  "XII", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X", "XI"
};

// ── Helpers de Dibujo Pantalla Analógica ──────────────
void drawThickLine(int x0, int y0, int x1, int y1, int thickness, uint16_t color) {
  float dx = x1 - x0;
  float dy = y1 - y0;
  float len = sqrt(dx*dx + dy*dy);
  if (len == 0) return;
  float nx = -dy / len;
  float ny = dx / len;
  for (int i = -thickness/2; i <= thickness/2; i++) {
      display.drawLine(x0 + nx*i, y0 + ny*i, x1 + nx*i, y1 + ny*i, color);
  }
}

void drawAnalogHands(int h, int m, int s, uint16_t color) {
  uint16_t colorH = (color == COLOR_WHITE) ? COLOR_WHITE : COLOR_BLACK;
  uint16_t colorM = (color == COLOR_WHITE) ? COLOR_WHITE : COLOR_BLACK;
  uint16_t colorS = (color == COLOR_WHITE) ? COLOR_WHITE : COLOR_RED; 

  // Hour
  float angle_h = (h % 12) * 30 + (m / 2.0) - 90;
  int x_h = 120 + cos(angle_h * PI / 180) * 50;
  int y_h = 120 + sin(angle_h * PI / 180) * 50;
  int tail_xh = 120 - cos(angle_h * PI / 180) * 12;
  int tail_yh = 120 - sin(angle_h * PI / 180) * 12;
  drawThickLine(tail_xh, tail_yh, x_h, y_h, 6, colorH);

  // Minute
  float angle_m = m * 6 - 90;
  int x_m = 120 + cos(angle_m * PI / 180) * 85;
  int y_m = 120 + sin(angle_m * PI / 180) * 85;
  int tail_xm = 120 - cos(angle_m * PI / 180) * 15;
  int tail_ym = 120 - sin(angle_m * PI / 180) * 15;
  drawThickLine(tail_xm, tail_ym, x_m, y_m, 4, colorM);

  // Second
  float angle_s = s * 6 - 90;
  int x_s = 120 + cos(angle_s * PI / 180) * 95;
  int y_s = 120 + sin(angle_s * PI / 180) * 95;
  int tail_xs = 120 - cos(angle_s * PI / 180) * 25;
  int tail_ys = 120 - sin(angle_s * PI / 180) * 25;
  display.drawLine(tail_xs, tail_ys, x_s, y_s, colorS);
  
  // Grosor extra en la cola del segundero
  int tail_mid_x = 120 - cos(angle_s * PI / 180) * 10;
  int tail_mid_y = 120 - sin(angle_s * PI / 180) * 10;
  drawThickLine(tail_xs, tail_ys, tail_mid_x, tail_mid_y, 3, colorS);

  // Center dot
  if (color != COLOR_WHITE) {
    display.fillCircle(120, 120, 5, COLOR_RED);
    display.fillCircle(120, 120, 2, 0xC618); // Silver/Grey
  }
}

// Q&Q Watch Face
void drawQQHands(int h, int m, int s) {
  uint16_t colorGold = 0xDE86;
  
  float angle_h = (h % 12) * 30 + (m / 2.0) - 90;
  int x_h = 120 + cos(angle_h * PI / 180) * 45;
  int y_h = 120 + sin(angle_h * PI / 180) * 45;
  drawThickLine(120, 120, x_h, y_h, 4, colorGold);

  float angle_m = m * 6 - 90;
  int x_m = 120 + cos(angle_m * PI / 180) * 80;
  int y_m = 120 + sin(angle_m * PI / 180) * 80;
  drawThickLine(120, 120, x_m, y_m, 3, colorGold);

  float angle_s = s * 6 - 90;
  int x_s = 120 + cos(angle_s * PI / 180) * 90;
  int y_s = 120 + sin(angle_s * PI / 180) * 90;
  int tail_xs = 120 - cos(angle_s * PI / 180) * 20;
  int tail_ys = 120 - sin(angle_s * PI / 180) * 20;
  display.drawLine(tail_xs, tail_ys, x_s, y_s, colorGold);

  display.fillCircle(120, 120, 3, colorGold);
}

void drawRomanHands(int h, int m, int s) {
  uint16_t colorGold = 0xDE86;
  
  float angle_h = (h % 12) * 30 + (m / 2.0) - 90;
  int x_h = 120 + cos(angle_h * PI / 180) * 55;
  int y_h = 120 + sin(angle_h * PI / 180) * 55;
  drawThickLine(120, 120, x_h, y_h, 4, colorGold);
  int cx_h = 120 + cos(angle_h * PI / 180) * 40;
  int cy_h = 120 + sin(angle_h * PI / 180) * 40;
  display.fillCircle(cx_h, cy_h, 6, colorGold);

  float angle_m = m * 6 - 90;
  int x_m = 120 + cos(angle_m * PI / 180) * 85;
  int y_m = 120 + sin(angle_m * PI / 180) * 85;
  drawThickLine(120, 120, x_m, y_m, 3, colorGold);
  int cx_m = 120 + cos(angle_m * PI / 180) * 65;
  int cy_m = 120 + sin(angle_m * PI / 180) * 65;
  display.fillCircle(cx_m, cy_m, 5, colorGold);

  float angle_s = s * 6 - 90;
  int x_s = 120 + cos(angle_s * PI / 180) * 95;
  int y_s = 120 + sin(angle_s * PI / 180) * 95;
  int tail_xs = 120 - cos(angle_s * PI / 180) * 20;
  int tail_ys = 120 - sin(angle_s * PI / 180) * 20;
  display.drawLine(tail_xs, tail_ys, x_s, y_s, colorGold);
  
  display.fillCircle(120, 120, 4, colorGold);
}

void drawSportHands(int h, int m, int s) {
  float angle_h = (h % 12) * 30 + (m / 2.0) - 90;
  int x_h = 120 + cos(angle_h * PI / 180) * 50;
  int y_h = 120 + sin(angle_h * PI / 180) * 50;
  drawThickLine(120, 120, x_h, y_h, 6, COLOR_WHITE);
  drawThickLine(120, 120, x_h, y_h, 2, COLOR_BLACK); 

  float angle_m = m * 6 - 90;
  int x_m = 120 + cos(angle_m * PI / 180) * 85;
  int y_m = 120 + sin(angle_m * PI / 180) * 85;
  drawThickLine(120, 120, x_m, y_m, 6, COLOR_WHITE);
  drawThickLine(120, 120, x_m, y_m, 2, COLOR_BLACK); 

  float angle_s = s * 6 - 90;
  int x_s = 120 + cos(angle_s * PI / 180) * 100;
  int y_s = 120 + sin(angle_s * PI / 180) * 100;
  display.drawLine(120, 120, x_s, y_s, COLOR_WHITE);
  
  display.fillCircle(120, 120, 4, COLOR_WHITE);
  display.fillCircle(120, 120, 2, COLOR_BLACK);
}

uint16_t getBackgroundColor(int code, int h) {
  bool isNight = (h >= 18 || h < 6);
  if (code == 0) return isNight ? 0x0004 : 0x44B6;
  if (code == 1) return isNight ? 0x0004 : 0x7CE0;
  if (code == 2) return isNight ? 0x0841 : 0x5AA8;
  if (code == 3) return isNight ? 0x0021 : 0x3A6B;
  if (code == 4) return isNight ? 0x0841 : 0x6B6D;
  if (code == 5) return isNight ? 0x1020 : 0x18A5;
  if (code == 6) return isNight ? 0x0021 : 0x2967;
  if (code == 7) return isNight ? 0x1004 : 0x186F;
  if (code == 8) return isNight ? 0x1004 : 0x1004;
  if (code == 9) return isNight ? 0x0821 : 0x5BF7;
  if (code == 10) return isNight ? 0x0841 : 0x2104;
  if (code == 11) return isNight ? 0x0021 : 0x2D8B;
  if (code == 12) return isNight ? 0x0841 : 0x18C7;
  if (code == 13) return isNight ? 0x1004 : 0x1004;
  return COLOR_BLACK;
}

void drawHugeWeatherIcon(int x, int y, int code, bool isNight) {
  if (code == 0) {
    if (isNight) {
      display.fillCircle(x, y, 35, COLOR_WHITE);
      display.fillCircle(x - 12, y - 8, 35, 0x0004);
    } else {
      display.fillCircle(x, y, 35, COLOR_YELLOW);
      display.fillCircle(x, y, 28, 0xFD60);
    }
  } else if (code == 1) {
    if (!isNight) display.fillCircle(x - 16, y - 16, 24, COLOR_YELLOW);
    else display.fillCircle(x - 16, y - 16, 18, COLOR_WHITE);
    display.fillCircle(x - 12, y + 4, 18, COLOR_WHITE);
    display.fillCircle(x + 12, y + 4, 18, COLOR_WHITE);
    display.fillCircle(x, y - 10, 24, COLOR_WHITE);
    display.fillRect(x - 18, y, 36, 22, COLOR_WHITE);
  } else if (code == 2) {
    if (!isNight) display.fillCircle(x - 15, y - 14, 24, COLOR_YELLOW);
    else display.fillCircle(x - 15, y - 14, 18, COLOR_WHITE);
    display.fillCircle(x - 10, y + 6, 18, COLOR_WHITE);
    display.fillCircle(x + 14, y + 6, 18, COLOR_WHITE);
    display.fillCircle(x + 4, y - 8, 20, COLOR_WHITE);
    display.fillRect(x - 18, y, 40, 22, COLOR_WHITE);
  } else if (code == 3) {
    display.fillCircle(x - 12, y + 6, 18, COLOR_WHITE);
    display.fillCircle(x + 12, y + 6, 18, COLOR_WHITE);
    display.fillCircle(x, y - 8, 22, COLOR_WHITE);
    display.fillRect(x - 20, y, 42, 24, COLOR_WHITE);
  } else if (code == 4) {
    display.drawRect(x - 20, y - 14, 40, 28, COLOR_WHITE);
    display.fillCircle(x - 8, y - 8, 10, COLOR_WHITE);
    display.fillCircle(x + 8, y - 8, 10, COLOR_WHITE);
    display.drawLine(x - 16, y + 8, x + 16, y + 8, COLOR_WHITE);
  } else if (code == 5) {
    display.fillCircle(x - 15, y + 5, 16, 0x94B2);
    display.fillCircle(x + 15, y + 5, 16, 0x94B2);
    display.fillCircle(x, y - 8, 18, 0x94B2);
    display.fillRect(x - 18, y, 36, 18, 0x94B2);
    display.drawLine(x - 4, y + 14, x - 6, y + 20, COLOR_CYAN);
    display.drawLine(x + 4, y + 14, x + 2, y + 20, COLOR_CYAN);
  } else if (code == 6) {
    display.fillCircle(x - 15, y + 5, 16, 0x5AEB);
    display.fillCircle(x + 15, y + 5, 16, 0x5AEB);
    display.fillCircle(x, y - 8, 18, 0x5AEB);
    display.fillRect(x - 18, y, 36, 18, 0x5AEB);
    for (int i = -12; i <= 12; i += 8) {
      display.drawLine(x + i, y + 14, x + i, y + 24, COLOR_CYAN);
    }
  } else if (code == 7) {
    display.fillCircle(x - 15, y + 5, 16, 0x5AEB);
    display.fillCircle(x + 15, y + 5, 16, 0x5AEB);
    display.fillCircle(x, y - 8, 18, 0x5AEB);
    display.fillRect(x - 18, y, 36, 18, 0x5AEB);
    display.drawLine(x - 4, y + 14, x - 8, y + 20, COLOR_CYAN);
    display.drawLine(x + 4, y + 14, x + 1, y + 20, COLOR_CYAN);
    display.fillTriangle(x - 2, y + 10, x + 2, y + 10, x, y + 18, COLOR_YELLOW);
  } else if (code == 8) {
    display.fillCircle(x - 14, y + 4, 16, 0x3186);
    display.fillCircle(x + 14, y + 4, 16, 0x3186);
    display.fillCircle(x, y - 10, 20, 0x3186);
    display.fillRect(x - 18, y, 36, 20, 0x3186);
    display.fillTriangle(x - 2, y + 8, x + 2, y + 8, x, y + 18, COLOR_YELLOW);
    display.fillTriangle(x, y + 18, x + 4, y + 18, x + 1, y + 26, COLOR_YELLOW);
  } else if (code == 9) {
    display.fillCircle(x - 14, y + 4, 16, 0xA514);
    display.fillCircle(x + 14, y + 4, 16, 0xA514);
    display.fillCircle(x, y - 10, 20, 0xA514);
    display.fillRect(x - 18, y, 36, 20, 0xA514);
    display.drawLine(x - 6, y + 18, x + 6, y + 18, COLOR_WHITE);
    display.drawLine(x, y + 10, x, y + 26, COLOR_WHITE);
  } else if (code == 10) {
    display.fillCircle(x - 14, y + 4, 16, 0x8410);
    display.fillCircle(x + 14, y + 4, 16, 0x8410);
    display.fillCircle(x, y - 10, 20, 0x8410);
    display.fillRect(x - 18, y, 36, 20, 0x8410);
    display.fillRect(x - 6, y + 18, 4, 4, COLOR_WHITE);
    display.fillRect(x + 2, y + 18, 4, 4, COLOR_WHITE);
  } else if (code == 11) {
    display.drawLine(x - 18, y, x + 18, y, 0x7BEF);
    display.drawLine(x - 18, y + 8, x + 14, y + 8, 0x7BEF);
    display.drawLine(x - 18, y + 16, x + 10, y + 16, 0x7BEF);
    display.drawLine(x - 18, y + 24, x + 6, y + 24, 0x7BEF);
  } else if (code == 12) {
    display.fillCircle(x - 14, y + 4, 16, 0x5AEB);
    display.fillCircle(x + 14, y + 4, 16, 0x5AEB);
    display.fillCircle(x, y - 10, 20, 0x5AEB);
    display.fillRect(x - 18, y, 36, 20, 0x5AEB);
    display.drawLine(x - 8, y + 18, x - 4, y + 28, COLOR_CYAN);
    display.drawLine(x + 2, y + 18, x - 2, y + 28, COLOR_CYAN);
    display.fillRect(x - 18, y + 24, 36, 8, 0x7BEF);
  } else if (code == 13) {
    display.fillCircle(x - 14, y + 4, 16, 0x3186);
    display.fillCircle(x + 14, y + 4, 16, 0x3186);
    display.fillCircle(x, y - 10, 20, 0x3186);
    display.fillRect(x - 18, y, 36, 20, 0x3186);
    display.fillTriangle(x - 2, y + 8, x + 2, y + 8, x, y + 18, COLOR_YELLOW);
    display.fillRect(x - 6, y + 18, 4, 4, COLOR_WHITE);
    display.fillRect(x + 2, y + 18, 4, 4, COLOR_WHITE);
  } else {
    display.fillCircle(x, y, 25, COLOR_WHITE);
  }
}

void drawSmallWeatherIcon(int x, int y, int code) {
  if (code == 0) {
    display.fillCircle(x, y, 12, COLOR_YELLOW);
  } else if (code == 1) {
    display.fillCircle(x - 6, y + 2, 8, COLOR_WHITE);
    display.fillCircle(x + 6, y + 2, 8, COLOR_WHITE);
    display.fillCircle(x, y - 4, 10, COLOR_WHITE);
    display.fillRect(x - 6, y, 12, 10, COLOR_WHITE);
  } else if (code == 2) {
    display.fillCircle(x - 5, y + 2, 7, COLOR_WHITE);
    display.fillCircle(x + 5, y + 2, 7, COLOR_WHITE);
    display.fillCircle(x, y - 4, 9, COLOR_WHITE);
    display.fillRect(x - 6, y, 12, 10, COLOR_WHITE);
  } else if (code == 3) {
    display.fillCircle(x - 5, y + 2, 7, COLOR_WHITE);
    display.fillCircle(x + 5, y + 2, 7, COLOR_WHITE);
    display.fillCircle(x, y - 4, 9, COLOR_WHITE);
    display.fillRect(x - 7, y, 14, 10, COLOR_WHITE);
  } else if (code == 4) {
    display.fillCircle(x - 6, y + 2, 8, COLOR_WHITE);
    display.fillCircle(x + 6, y + 2, 8, COLOR_WHITE);
    display.fillCircle(x, y - 4, 10, COLOR_WHITE);
    display.fillRect(x - 6, y, 12, 10, COLOR_WHITE);
    display.drawLine(x - 10, y + 12, x + 10, y + 12, 0x7BEF);
    display.drawLine(x - 10, y + 16, x + 10, y + 16, 0x7BEF);
  } else if (code == 5) {
    uint16_t c_cloud = 0x5AEB;
    display.fillCircle(x - 6, y + 2, 8, c_cloud);
    display.fillCircle(x + 6, y + 2, 8, c_cloud);
    display.fillCircle(x, y - 4, 10, c_cloud);
    display.fillRect(x - 6, y, 12, 10, c_cloud);
    display.drawLine(x - 4, y + 10, x - 6, y + 15, COLOR_CYAN);
    display.drawLine(x + 4, y + 10, x + 2, y + 15, COLOR_CYAN);
  } else if (code == 6) {
    uint16_t c_cloud = 0x03EF;
    display.fillCircle(x - 6, y + 2, 8, c_cloud);
    display.fillCircle(x + 6, y + 2, 8, c_cloud);
    display.fillCircle(x, y - 4, 10, c_cloud);
    display.fillRect(x - 6, y, 12, 10, c_cloud);
    display.drawLine(x - 4, y + 10, x - 6, y + 15, COLOR_CYAN);
    display.drawLine(x + 4, y + 10, x + 2, y + 15, COLOR_CYAN);
    display.drawLine(x, y + 10, x, y + 16, COLOR_CYAN);
  } else if (code == 7) {
    uint16_t c_cloud = 0x03EF;
    display.fillCircle(x - 6, y + 2, 8, c_cloud);
    display.fillCircle(x + 6, y + 2, 8, c_cloud);
    display.fillCircle(x, y - 4, 10, c_cloud);
    display.fillRect(x - 6, y, 12, 10, c_cloud);
    display.drawLine(x - 4, y + 10, x - 8, y + 16, COLOR_CYAN);
    display.drawLine(x + 4, y + 10, x + 1, y + 16, COLOR_CYAN);
    display.fillTriangle(x - 2, y + 6, x + 2, y + 6, x, y + 12, COLOR_YELLOW);
  } else if (code == 8) {
    uint16_t c_cloud = 0x3186;
    display.fillCircle(x - 6, y + 2, 8, c_cloud);
    display.fillCircle(x + 6, y + 2, 8, c_cloud);
    display.fillCircle(x, y - 4, 10, c_cloud);
    display.fillRect(x - 6, y, 12, 10, c_cloud);
    display.fillTriangle(x - 2, y + 6, x + 2, y + 6, x, y + 12, COLOR_YELLOW);
  } else if (code == 9) {
    display.fillCircle(x, y, 10, COLOR_WHITE);
    display.drawLine(x - 3, y - 2, x + 3, y + 2, COLOR_WHITE);
    display.drawLine(x - 3, y + 2, x + 3, y - 2, COLOR_WHITE);
  } else if (code == 10) {
    uint16_t c_cloud = 0x8410;
    display.fillCircle(x - 6, y + 2, 8, c_cloud);
    display.fillCircle(x + 6, y + 2, 8, c_cloud);
    display.fillCircle(x, y - 4, 10, c_cloud);
    display.fillRect(x - 6, y, 12, 10, c_cloud);
    display.fillRect(x - 4, y + 10, 3, 3, COLOR_WHITE);
    display.fillRect(x + 2, y + 10, 3, 3, COLOR_WHITE);
  } else if (code == 11) {
    display.drawLine(x - 8, y - 2, x + 8, y - 2, 0x7BEF);
    display.drawLine(x - 8, y + 2, x + 6, y + 2, 0x7BEF);
    display.drawLine(x - 8, y + 6, x + 4, y + 6, 0x7BEF);
  } else if (code == 12) {
    uint16_t c_cloud = 0x5AEB;
    display.fillCircle(x - 6, y + 2, 8, c_cloud);
    display.fillCircle(x + 6, y + 2, 8, c_cloud);
    display.fillCircle(x, y - 4, 10, c_cloud);
    display.fillRect(x - 6, y, 12, 10, c_cloud);
    display.drawLine(x - 6, y + 12, x + 6, y + 12, 0x7BEF);
    display.drawLine(x - 6, y + 16, x + 6, y + 16, 0x7BEF);
    display.drawLine(x - 4, y + 10, x - 6, y + 14, COLOR_CYAN);
  } else if (code == 13) {
    uint16_t c_cloud = 0x3186;
    display.fillCircle(x - 6, y + 2, 8, c_cloud);
    display.fillCircle(x + 6, y + 2, 8, c_cloud);
    display.fillCircle(x, y - 4, 10, c_cloud);
    display.fillRect(x - 6, y, 12, 10, c_cloud);
    display.fillTriangle(x - 2, y + 6, x + 2, y + 6, x, y + 12, COLOR_YELLOW);
    display.fillRect(x - 4, y + 12, 3, 3, COLOR_WHITE);
    display.fillRect(x + 1, y + 12, 3, 3, COLOR_WHITE);
  } else {
    display.fillCircle(x, y, 10, COLOR_WHITE);
  }
}

// ── Estructura para definir el tema visual del reloj según clima ────────────────────
// Paletas de Clima (Colores RGB565 de 16-bits)
struct WeatherTheme {
    uint16_t primary;    // Color principal (Anillo exterior, manecilla de hora)
    uint16_t secondary;  // Color secundario (Marcas de minutos)
    uint16_t Accent;     // Color de acento (Manecilla de segundos)
    uint16_t bgDark;     // Tono oscuro para fondos parciales
};

// Obtiene el tema basado en el código de categoría interna o el código WMO estándar de clima
WeatherTheme getThemeByCode(int code) {
    if (code == 0 || code == 1) {
        return {0xFDA0, 0xFCE0, 0xF800, 0x2100};
    } else if (code == 2 || code == 3 || code == 4) {
        return {0x94B2, 0xBDF7, 0x07FF, 0x10A2};
    } else if (code == 5 || code == 6 || code == 7 || code == 12) {
        return {0x03EF, 0x05FF, 0xFDA0, 0x0010};
    } else if (code == 8 || code == 13) {
        return {0x780F, 0xFDA0, 0xF800, 0x1002};
    } else if (code == 9 || code == 10) {
        return {0xFFFF, 0x7FFF, 0x07FF, 0x1234};
    } else if (code == 11) {
        return {0x5C4A, 0x8410, 0xF800, 0x18C7};
    }
    return {0x07FF, 0x0410, 0xFFFF, 0x0000};
}

// ──────────────────────────────────────────────────────
// Weather background engine for digital face 1
// ──────────────────────────────────────────────────────
enum WeatherParticleType : uint8_t {
    WP_CLOUD = 0,
    WP_RAIN,
    WP_SNOW,
    WP_HAIL,
    WP_STAR,
    WP_LIGHTNING,
    WP_FOG,
    WP_BIRD
};

struct Particle {
    int16_t x;
    int16_t y;
    int16_t vx;
    int16_t vy;
    uint8_t type;
    uint8_t alpha;
};

struct WeatherScene {
    uint8_t weatherCode;
    uint8_t dayPhase;
};

static Particle weatherParticles[40];
static uint8_t weatherParticleCount = 0;
static uint8_t weatherBgCode = 255;
static uint8_t weatherBgPhase = 255;
static bool weatherBackgroundReady = false;
static uint16_t weatherSkyTop = COLOR_BLACK;
static uint16_t weatherSkyBottom = COLOR_BLACK;
static uint16_t weatherHorizon = COLOR_BLACK;
static uint16_t weatherOrbColor = COLOR_WHITE;
static uint16_t weatherCloudColor = COLOR_WHITE;
static uint16_t weatherFogColor = COLOR_WHITE;
static uint16_t weatherGroundColor = COLOR_BLACK;
static int16_t weatherOrbX = 120;
static int16_t weatherOrbY = 70;
static uint8_t weatherPulse = 0;
static uint8_t weatherStarPhase = 0;
static bool lightningActive = false;
static uint8_t lightningTimer = 0;
static uint8_t lastOrbHour = 255;
static uint8_t lastOrbMinute = 255;
static int last_disp_hour = -1;
static int last_disp_min = -1;
static int last_disp_sec = -1;

uint16_t blendColor(uint16_t c1, uint16_t c2, uint8_t t) {
    uint8_t r1 = (c1 >> 11) & 0x1F;
    uint8_t g1 = (c1 >> 5) & 0x3F;
    uint8_t b1 = c1 & 0x1F;
    uint8_t r2 = (c2 >> 11) & 0x1F;
    uint8_t g2 = (c2 >> 5) & 0x3F;
    uint8_t b2 = c2 & 0x1F;
    uint8_t r = (r1 * (255 - t) + r2 * t) >> 8;
    uint8_t g = (g1 * (255 - t) + g2 * t) >> 8;
    uint8_t b = (b1 * (255 - t) + b2 * t) >> 8;
    return (r << 11) | (g << 5) | b;
}

uint8_t getDayPhase(int hour, int minute) {
    int total = hour * 60 + minute;
    if (total < 300) return 0;        // Madrugada
    if (total < 420) return 1;        // Amanecer
    if (total < 1020) return 2;       // Día
    if (total < 1200) return 3;       // Atardecer
    return 4;                         // Noche
}

void calculateOrbPosition(int hour, int minute) {
    int total = hour * 60 + minute;
    if (total < 360) {
        weatherOrbX = 40 + total / 4;
        weatherOrbY = 90;
    } else if (total < 1080) {
        int p = total - 360;
        weatherOrbX = 40 + (p * 160) / 720;
        weatherOrbY = 100 - abs(360 - p) / 4;
    } else {
        int p = total - 1080;
        weatherOrbX = 200 - (p * 20) / 120;
        weatherOrbY = 78 + p / 4;
    }
}

void setWeatherTheme(uint8_t weatherCode) {
    weatherBgCode = weatherCode;
    uint8_t phase = weatherBgPhase;
    bool isNight = (phase == 0 || phase == 4);

    if (weatherCode == 0) {
        weatherSkyTop = isNight ? 0x0841 : 0x7E0;
        weatherSkyBottom = isNight ? 0x0042 : 0x03E0;
        weatherHorizon = isNight ? 0x2104 : 0x7FE0;
        weatherOrbColor = isNight ? 0xD000 : 0xFD20;
        weatherCloudColor = isNight ? 0x8410 : 0xC618;
        weatherFogColor = 0x8410;
        weatherGroundColor = isNight ? 0x2104 : 0x39E7;
    } else if (weatherCode == 1) {
        weatherSkyTop = isNight ? 0x0841 : 0x6CE0;
        weatherSkyBottom = isNight ? 0x0021 : 0x03E7;
        weatherHorizon = isNight ? 0x2104 : 0x7D8C;
        weatherOrbColor = isNight ? 0xD000 : 0xFD20;
        weatherCloudColor = isNight ? 0x8410 : 0xBDF7;
        weatherFogColor = 0x8C51;
        weatherGroundColor = isNight ? 0x2104 : 0x528A;
    } else if (weatherCode == 2 || weatherCode == 3) {
        weatherSkyTop = isNight ? 0x0841 : 0x5AEB;
        weatherSkyBottom = isNight ? 0x0021 : 0x5AEB;
        weatherHorizon = isNight ? 0x2945 : 0x18E3;
        weatherOrbColor = isNight ? 0xD000 : 0xFD20;
        weatherCloudColor = 0xA514;
        weatherFogColor = 0x8410;
        weatherGroundColor = isNight ? 0x2104 : 0x4208;
    } else if (weatherCode == 4) {
        weatherSkyTop = isNight ? 0x0841 : 0x7D8C;
        weatherSkyBottom = isNight ? 0x0021 : 0x4A69;
        weatherHorizon = 0x6329;
        weatherOrbColor = isNight ? 0xD000 : 0xFE20;
        weatherCloudColor = 0x7BDE;
        weatherFogColor = 0x8430;
        weatherGroundColor = 0x2945;
    } else if (weatherCode == 6 || weatherCode == 9) {
        weatherSkyTop = isNight ? 0x0841 : 0x4A69;
        weatherSkyBottom = isNight ? 0x0021 : 0x39E7;
        weatherHorizon = 0x2104;
        weatherOrbColor = 0xFFFF;
        weatherCloudColor = 0x7BDE;
        weatherFogColor = 0x8430;
        weatherGroundColor = 0x2104;
    } else if (weatherCode == 5 || weatherCode == 7 || weatherCode == 8) {
        weatherSkyTop = 0x2104;
        weatherSkyBottom = 0x0000;
        weatherHorizon = 0x18E3;
        weatherOrbColor = 0xF800;
        weatherCloudColor = 0x7BDE;
        weatherFogColor = 0x6318;
        weatherGroundColor = 0x18C3;
    } else if (weatherCode == 10) {
        weatherSkyTop = 0x2104;
        weatherSkyBottom = 0x0000;
        weatherHorizon = 0x18E3;
        weatherOrbColor = 0xF800;
        weatherCloudColor = 0xC618;
        weatherFogColor = 0x8430;
        weatherGroundColor = 0x2104;
    } else {
        weatherSkyTop = 0x05F7;
        weatherSkyBottom = 0x0410;
        weatherHorizon = 0x2104;
        weatherOrbColor = 0xFD20;
        weatherCloudColor = 0xC618;
        weatherFogColor = 0x8430;
        weatherGroundColor = 0x4A69;
    }
}

void addWeatherParticle(int16_t x, int16_t y, int16_t vx, int16_t vy, uint8_t type, uint8_t alpha = 192) {
    if (weatherParticleCount >= sizeof(weatherParticles) / sizeof(weatherParticles[0])) return;
    weatherParticles[weatherParticleCount++] = {x, y, vx, vy, type, alpha};
}

void initWeatherBackground() {
    weatherParticleCount = 0;
    weatherPulse = 0;
    weatherStarPhase = 0;
    lightningActive = false;
    lightningTimer = 0;

    if (weatherBgCode == 0) {
        for (int i = 0; i < 3; i++) addWeatherParticle(random(-70, 240), random(0, 240), 0, 0, WP_CLOUD, 220);
        if (weatherBgPhase == 4 || weatherBgPhase == 0) {
            for (int i = 0; i < 12; i++) addWeatherParticle(random(10, 230), random(10, 70), 0, 0, WP_STAR, 255);
        } else {
            for (int i = 0; i < 3; i++) addWeatherParticle(random(-20, 240), random(40, 90), random(3, 5), 0, WP_BIRD, 255);
        }
    } else if (weatherBgCode == 1) {
        for (int i = 0; i < 4; i++) addWeatherParticle(random(-80, 240), random(0, 240), 0, 0, WP_CLOUD, 216);
        if (weatherBgPhase == 4 || weatherBgPhase == 0) {
            for (int i = 0; i < 10; i++) addWeatherParticle(random(10, 230), random(10, 70), 0, 0, WP_STAR, 255);
        }
    } else if (weatherBgCode == 2 || weatherBgCode == 3) {
        for (int i = 0; i < 5; i++) addWeatherParticle(random(-90, 240), random(0, 240), 0, 0, WP_CLOUD, 200);
        if (weatherBgPhase != 4 && weatherBgPhase != 0) addWeatherParticle(random(-90, 240), random(0, 240), 0, 0, WP_FOG, 100);
    } else if (weatherBgCode == 4) {
        for (int i = 0; i < 5; i++) addWeatherParticle(random(-90, 240), random(0, 240), 0, 0, WP_CLOUD, 180);
        for (int i = 0; i < 18; i++) addWeatherParticle(random(0, 240), random(-240, 0), 0, random(5, 7), WP_RAIN, 255);
        addWeatherParticle(random(20, 220), random(120, 160), 0, 0, WP_FOG, 130);
    } else if (weatherBgCode == 5 || weatherBgCode == 7 || weatherBgCode == 8) {
        for (int i = 0; i < 6; i++) addWeatherParticle(random(-100, 240), random(0, 240), 0, 0, WP_CLOUD, 200);
        for (int i = 0; i < 24; i++) addWeatherParticle(random(0, 240), random(-240, 0), 0, random(6, 10), WP_RAIN, 255);
        if (weatherBgCode == 8) lightningTimer = random(20, 60);
    } else if (weatherBgCode == 6 || weatherBgCode == 9) {
        for (int i = 0; i < 4; i++) addWeatherParticle(random(-90, 240), random(0, 240), 0, 0, WP_CLOUD, 190);
        for (int i = 0; i < 20; i++) addWeatherParticle(random(0, 240), random(-240, 0), random(-1, 1), random(1, 3), WP_SNOW, 255);
    } else if (weatherBgCode == 10) {
        for (int i = 0; i < 4; i++) addWeatherParticle(random(-90, 240), random(0, 240), 0, 0, WP_CLOUD, 190);
        for (int i = 0; i < 18; i++) addWeatherParticle(random(0, 240), random(-240, 0), random(-1, 1), random(7, 11), WP_HAIL, 255);
    } else {
        for (int i = 0; i < 3; i++) addWeatherParticle(random(-70, 240), random(0, 240), 0, 0, WP_CLOUD, 220);
    }
}

void drawSkyGradient() {
    for (int band = 0; band < 12; band++) {
        uint8_t ratio = (band * 255) / 11;
        uint16_t color = blendColor(weatherSkyTop, weatherSkyBottom, ratio);
        int y = band * 20;
        display.fillRect(0, y, 240, 20, color);
    }
}

void drawSunOrMoon() {
    if (weatherBgPhase == 4 || weatherBgPhase == 0) {
        display.fillCircle(weatherOrbX, weatherOrbY, 22, weatherOrbColor);
        display.fillCircle(weatherOrbX + 8, weatherOrbY - 4, 18, weatherSkyBottom);
        if (weatherBgPhase == 4) {
            for (int s = 0; s < 8; s++) {
                int angle = s * 45;
                int dx = cos(angle * PI / 180.0) * 28;
                int dy = sin(angle * PI / 180.0) * 28;
                display.drawLine(weatherOrbX + dx, weatherOrbY + dy, weatherOrbX + dx * 2 / 3, weatherOrbY + dy * 2 / 3, 0xC618);
            }
        }
    } else {
        uint16_t halo = blendColor(weatherOrbColor, weatherSkyTop, 180);
        display.fillCircle(weatherOrbX, weatherOrbY, 24, halo);
        display.fillCircle(weatherOrbX, weatherOrbY, 16, weatherOrbColor);
        display.drawCircle(weatherOrbX, weatherOrbY, 28, halo);
    }
}

void drawWeatherParticles() {
    for (uint8_t i = 0; i < weatherParticleCount; i++) {
        Particle &p = weatherParticles[i];
        if (p.type == WP_STAR) {
            uint16_t starColor = (weatherStarPhase % 3 == 0) ? 0xFFFF : 0xA514;
            display.fillCircle(p.x, p.y, 1, starColor);
            continue;
        }
        if (p.type == WP_BIRD) {
            display.drawLine(p.x, p.y, p.x + 5, p.y - 4, 0xF800);
            display.drawLine(p.x + 5, p.y - 4, p.x + 10, p.y, 0xF800);
            continue;
        }
        if (p.type == WP_CLOUD) {
          // No dibujar nubes por debajo de cierta altura para no tapar la UI inferior
          if (p.y > 140) continue;
          display.fillCircle(p.x, p.y, 18, weatherCloudColor);
          display.fillCircle(p.x + 22, p.y + 4, 16, weatherCloudColor);
          display.fillCircle(p.x - 20, p.y + 4, 16, weatherCloudColor);
          display.fillRect(p.x - 20, p.y + 4, 60, 18, weatherCloudColor);
            continue;
        }
        if (p.type == WP_FOG) {
            display.fillRect(0, p.y, 240, 12, weatherFogColor);
            continue;
        }
        if (p.type == WP_RAIN) {
            display.drawLine(p.x, p.y, p.x, p.y + 8, 0x05FF);
            display.drawLine(p.x + 1, p.y, p.x + 1, p.y + 8, 0x05FF);
            continue;
        }
        if (p.type == WP_SNOW) {
            display.fillCircle(p.x, p.y, 2, 0xFFFF);
            display.drawLine(p.x - 2, p.y, p.x + 2, p.y, 0xFFFF);
            display.drawLine(p.x, p.y - 2, p.x, p.y + 2, 0xFFFF);
            continue;
        }
        if (p.type == WP_HAIL) {
            display.fillRect(p.x, p.y, 2, 2, 0xFFFF);
            display.fillRect(p.x + 2, p.y + 2, 2, 2, 0xFFFF);
            continue;
        }
    }
}

void updateWeatherBackground() {
    weatherPulse++;
    weatherStarPhase++;

    for (uint8_t i = 0; i < weatherParticleCount; i++) {
        Particle &p = weatherParticles[i];
        
        // No actualizar posición para nubes estáticas
        if (p.type != WP_CLOUD && p.type != WP_STAR) {
            p.x += p.vx;
            p.y += p.vy;
        }

        if (p.type == WP_BIRD) {
            if (p.x > 280) p.x = -60;
        }
        if (p.type == WP_RAIN || p.type == WP_SNOW || p.type == WP_HAIL) {
            if (p.y > 250) {
                p.y = random(-40, -10);
                p.x = random(0, 239);
            }
            if (p.x < 0) p.x = 0;
            if (p.x > 239) p.x = 239;
        }
        if (p.type == WP_FOG) {
            if (p.y > 220) p.y = random(60, 100);
        }
        if (p.type == WP_STAR) {
            p.x = constrain(p.x, 0, 239);
            p.y = constrain(p.y, 0, 239);
        }
    }

    if (weatherBgCode == 8) {
        if (lightningActive) {
            if (lightningTimer == 0) lightningActive = false;
            else lightningTimer--;
        } else if (random(0, 80) == 0) {
            lightningActive = true;
            lightningTimer = random(2, 5);
        }
    }
}

void drawWeatherBackground() {
    drawSkyGradient();
    display.fillRect(0, 180, 240, 60, weatherGroundColor);
    drawSunOrMoon();

    if (weatherBgCode == 4 || weatherBgCode == 5 || weatherBgCode == 7 || weatherBgCode == 8) {
        display.fillRect(0, 88, 240, 12, weatherFogColor);
    }

    if (lightningActive) {
        display.fillRect(0, 0, 240, 14, 0xFC00);
        display.drawLine(160, 20, 140, 70, 0xFFFF);
        display.drawLine(140, 70, 150, 80, 0xF800);
        display.drawLine(150, 80, 130, 120, 0xFFFF);
    }

    drawWeatherParticles();

    // Terrain highlights
    if (weatherBgCode >= 4 && weatherBgCode <= 8) {
        display.drawLine(10, 190, 230, 190, 0x07FF);
        display.drawLine(25, 200, 215, 200, 0x07FF);
    }
}

void initWeatherScene(uint8_t weatherCode, uint8_t hour, uint8_t minute) {
    weatherBgPhase = getDayPhase(hour, minute);
    calculateOrbPosition(hour, minute);
    setWeatherTheme(weatherCode);
    initWeatherBackground();
    weatherBackgroundReady = true;
}


void updateWeatherScene(uint8_t hour, uint8_t minute, uint8_t weatherCode) {
    uint8_t phase = getDayPhase(hour, minute);
    if (!weatherBackgroundReady || weatherCode != weatherBgCode || phase != weatherBgPhase) {
        weatherBgPhase = phase;
        calculateOrbPosition(hour, minute);
        lastOrbHour = hour;
        lastOrbMinute = minute;
        setWeatherTheme(weatherCode);
        initWeatherBackground();
        weatherBackgroundReady = true;
    } else {
        // Solo recalcular posición del orbe si ha cambiado la hora o minuto
        if (lastOrbHour != hour || lastOrbMinute != minute) {
            calculateOrbPosition(hour, minute);
            lastOrbHour = hour;
            lastOrbMinute = minute;
        }
        updateWeatherBackground();
    }
}

// Dibujado Estático (El Fondo de la Esfera) - Se ejecuta solo cuando forceRedraw es verdadero
void drawWeatherClockStatic(int weatherCode, String temp, String weatherText) {
    WeatherTheme theme = getThemeByCode(weatherCode);
    
    // Centro de la pantalla circular GC9A01A es (120, 120)
    display.fillScreen(COLOR_BLACK);
    
    // 1. Anillo Exterior Dinámico (Color del clima)
    display.drawCircle(120, 120, 119, theme.primary);
    display.drawCircle(120, 120, 117, theme.bgDark);
    
    // 2. Dibujar marcas de las horas (Cada 30 grados)
    for (int i = 0; i < 12; i++) {
        float angle = i * 30.0 * PI / 180.0;
        int x1 = 120 + 106 * sin(angle);
        int y1 = 120 - 106 * cos(angle);
        int x2 = 120 + 115 * sin(angle);
        int y2 = 120 - 115 * cos(angle);
        
        // Resaltar las horas principales (12, 3, 6, 9)
        if (i % 3 == 0) {
            display.drawLine(x1, y1, x2, y2, theme.primary);
            // Pequeño círculo de acento interior
            display.fillCircle(120 + 100 * sin(angle), 120 - 100 * cos(angle), 2, theme.Accent);
        } else {
            display.drawLine(x1, y1, x2, y2, theme.secondary);
        }
    }
    
    // 3. UI del Clima Central-Superior (Temperatura)
    display.setTextSize(3);
    display.setTextColor(theme.primary);
    // Centrar dinámicamente el texto de la temperatura
    int tempX = 120 - ((temp.length() + 2) * 18) / 2; // Aproximación de ancho de fuente
    display.setCursor(tempX, 55);
    display.print(temp + " C"); 
    
    // Pequeño círculo indicador de grados de diseño
    display.drawCircle(tempX + (temp.length() * 18) + 4, 58, 3, theme.primary);

    // 4. UI del Clima Inferior (Texto descriptivo)
    display.setTextSize(1);
    display.setTextColor(theme.secondary);
    int textX = 120 - (weatherText.length() * 6) / 2;
    display.setCursor(textX, 165);
    display.print(weatherText);
}

// Variables para recordar la posición anterior de las manecillas y poder borrarlas
static int lastHrX = 120, lastHrY = 120;
static int lastMinX = 120, lastMinY = 120;
static int lastSecX = 120, lastSecY = 120;

// Dibujado Dinámico (Manecillas sin Parpadeo)
void drawWeatherClockDynamic(int hours, int minutes, int seconds, int weatherCode) {
    WeatherTheme theme = getThemeByCode(weatherCode);

    // 1. BORRAR manecillas anteriores dibujándolas en NEGRO
    display.drawLine(120, 120, lastSecX, lastSecY, COLOR_BLACK);
    display.drawLine(120, 120, lastMinX, lastMinY, COLOR_BLACK);
    display.drawLine(120, 120, lastHrX, lastHrY, COLOR_BLACK);

    // 2. CALCULAR Nuevas Posiciones
    // Ángulos en radianes
    float radSec = seconds * 6.0 * PI / 180.0;
    float radMin = (minutes * 6.0 + seconds * 0.1) * PI / 180.0;
    float radHr  = (hours * 30.0 + minutes * 0.5) * PI / 180.0;

    // Longitudes de manecillas optimizadas para el círculo de 240x240
    int lenHr  = 50;
    int lenMin = 75;
    int lenSec = 90;

    lastHrX  = 120 + lenHr  * sin(radHr);
    lastHrY  = 120 - lenHr  * cos(radHr);
    lastMinX = 120 + lenMin * sin(radMin);
    lastMinY = 120 - lenMin * cos(radMin);
    lastSecX = 120 + lenSec * sin(radSec);
    lastSecY = 120 - lenSec * cos(radSec);

    // 3. DIBUJAR Nuevas Manecillas con jerarquía visual
    // Hora (Gruesa/Estilizada)
    display.drawLine(120, 120, lastHrX, lastHrY, theme.primary);
    display.drawLine(121, 120, lastHrX+1, lastHrY, theme.primary); // Grosor extra artificial
    
    // Minuto (Media)
    display.drawLine(120, 120, lastMinX, lastMinY, theme.secondary);
    
    // Segundo (Fina con color de acento destacado)
    display.drawLine(120, 120, lastSecX, lastSecY, theme.Accent);

    // 4. Tapón central estético
    display.fillCircle(120, 120, 4, theme.Accent);
    display.drawCircle(120, 120, 2, COLOR_BLACK);
}

// ── Pantalla normal ────────────────────────────────────

// Variables globales para controlar el redibujado completo del reloj
static int clock_last_s = -1;
static int clock_last_m = -1;
static int clock_last_h = -1;

// Llamar esta función para forzar un redibujado completo del reloj (fondo + manecillas)
void resetClockDraw() {
  clock_last_s = -1;
  clock_last_m = -1;
  clock_last_h = -1;
  weatherBackgroundReady = false;
  lastOrbHour = 255;
  lastOrbMinute = 255;
  last_disp_hour = -1;
  last_disp_min = -1;
  last_disp_sec = -1;
}

void drawNormal() {
  struct tm t;
  if (!getLocalTime(&t)) return;

  // Usar las variables globales en lugar de las estáticas locales
  bool needFullRedraw = forceRedraw || (clock_last_s == -1);
  if (forceRedraw) forceRedraw = false; // Consumir el flag global aquí
  if (needFullRedraw) {
    if (isAnalog) {
      // ── MODO ANALÓGICO ──
      display.fillScreen(COLOR_WHITE);
      
      if (analogFace == 1) {
        // Skilcraft (Face 1)
        for (int r = 114; r < 120; r++) display.drawCircle(120, 120, r, 0x4A49);
        for (int i=0; i<60; i++) {
           float angle = i * 6 * PI / 180;
           int x = 120 + cos(angle) * 105;
           int y = 120 + sin(angle) * 105;
           if (i % 5 == 0) display.fillCircle(x, y, 3, COLOR_BLACK);
           else display.fillCircle(x, y, 1, COLOR_BLACK);
        }
      } else if (analogFace == 3) {
        // Luxury Roman (Face 3)
        uint16_t colorBurgundy = 0x3864; // Vinotinto/Borgoña oscuro
        display.fillScreen(colorBurgundy);
        uint16_t colorGold = 0xDE86;
        for (int r = 115; r < 120; r++) display.drawCircle(120, 120, r, colorGold);
        display.drawCircle(120, 120, 110, colorGold);
        
        for (int i=0; i<60; i++) {
           float angle = i * 6 * PI / 180;
           if (i % 5 != 0) {
             display.drawLine(120+cos(angle)*112, 120+sin(angle)*112, 120+cos(angle)*114, 120+sin(angle)*114, colorGold);
           } else {
             display.fillTriangle(120+cos(angle)*108, 120+sin(angle)*108,
                                  120+cos(angle-0.03)*114, 120+sin(angle-0.03)*114,
                                  120+cos(angle+0.03)*114, 120+sin(angle+0.03)*114, colorGold);
           }
        }
        display.setTextColor(colorGold);
        display.setTextSize(2);
        for (int i=0; i<12; i++) {
           const char* rn = (const char*)pgm_read_ptr(&roman_numerals[i]);
           float angle = (i * 30 - 90) * PI / 180;
           int r_num = 90;
           int x = 120 + cos(angle) * r_num;
           int y = 120 + sin(angle) * r_num;
           display.setCursor(x - (strlen(rn)*12)/2, y - 8);
           display.print(rn);
        }
      } else if (analogFace == 2) {
        // Red-Sporty (Face 2)
        display.fillScreen(COLOR_BLACK);
        
        // Ticks
        for (int i=0; i<12; i++) {
           float angle = i * 30 * PI / 180;
           drawThickLine(120+cos(angle)*102, 120+sin(angle)*102, 120+cos(angle)*118, 120+sin(angle)*118, 6, COLOR_WHITE);
           drawThickLine(120+cos(angle)*104, 120+sin(angle)*104, 120+cos(angle)*116, 120+sin(angle)*116, 4, COLOR_RED);
        }
        for (int i=0; i<60; i++) {
           if (i%5 != 0) {
             float angle = i * 6 * PI / 180;
             display.drawLine(120+cos(angle)*114, 120+sin(angle)*114, 120+cos(angle)*118, 120+sin(angle)*118, COLOR_WHITE);
           }
        }
        
        // Círculos de fondo (Right, Left, Bottom)
        display.drawCircle(65, 120, 28, COLOR_RED);
        display.drawCircle(65, 120, 29, COLOR_RED);
        display.drawCircle(175, 120, 28, COLOR_RED);
        display.drawCircle(175, 120, 29, COLOR_RED);
        display.drawCircle(120, 165, 28, 0x39E7); // Gris
      }
    } else {
      if (digitalFace == 1) {
        // ── MODO DIGITAL 1 (WIDGET MODERNO) ──
        uint16_t bgColor = getBackgroundColor(globalWeatherCode, t.tm_hour);
        display.fillScreen(bgColor);

        // Ciudad (Centrada)
        display.setTextColor(COLOR_WHITE);
        display.setTextSize(2);
        int cityWidth = 6 * 12; // "Bogota" tiene 6 letras
        display.setCursor(120 - cityWidth/2, 30);
        display.print("Bogota");

        // Fecha
        String date = getCurrentDate();
        display.setTextSize(2); // Agrandado
        display.setCursor(15, 135);
        display.print(date);

        // Clima texto
        extern void drawAdaptiveText(String text, int y, int size, uint16_t color, bool centered = true, int maxWidthOverride = 0);
        drawAdaptiveText(globalWeather, 155, 2, COLOR_WHITE, false, 140);

        // Dibujo del clima gigante a la derecha (movido para evitar colisión)
        if (globalWeatherCode >= 0) {
          bool isNight = (t.tm_hour >= 18 || t.tm_hour < 6);
          drawHugeWeatherIcon(195, 100, globalWeatherCode, isNight);
          
          // Temperatura
          display.setTextSize(3);
          int tw = globalTemp.length() * 18;
          display.setCursor(195 - tw/2, 150);
          display.setTextColor(COLOR_WHITE);
          display.print(globalTemp);
          // Humedad (si está disponible)
          display.setTextSize(2);
          String humStr = (globalHumidity >= 0) ? String(globalHumidity) + "%" : "--";
          int htw = humStr.length() * 12;
          display.setCursor(195 - htw/2, 180);
          display.print(humStr);
        }
      } else if (digitalFace == 2) {
        // ── MODO DIGITAL 2 (LED ORANGE WALL CLOCK) ──
        display.fillScreen(COLOR_BLACK);
        
        // Ticks exteriores
        for(int i=0; i<60; i++) {
           float angle = (i * 6 - 90) * PI / 180;
           int x1 = 120 + cos(angle) * 110;
           int y1 = 120 + sin(angle) * 110;
           int x2 = 120 + cos(angle) * 118;
           int y2 = 120 + sin(angle) * 118;
           display.drawLine(x1, y1, x2, y2, 0xCE59); // inactivo
        }

        // Fecha (DD - MM) Arriba
        char dbuf[10];
        sprintf(dbuf, "%02d - %02d", t.tm_mday, t.tm_mon + 1);
        display.setTextSize(2);
        display.setTextColor(COLOR_WHITE);
        display.setCursor(120 - (7*12)/2, 25);
        display.print(dbuf);

        // Días de la semana
        const char* dias[] = {"DOM", "LUN", "MAR", "MIE", "JUE", "VIE", "SAB"};
        display.setTextSize(1);
        extern int getCurrentDow();
        int dow = getCurrentDow();
        int startX = 26;
        for (int i=0; i<7; i++) {
          if (i == dow) display.setTextColor(COLOR_ORANGE);
          else display.setTextColor(0xCE59);
          display.setCursor(startX + i*28, 50);
          display.print(dias[i]);
        }
        
        // Temperatura
        display.setTextSize(3);
        display.setTextColor(COLOR_WHITE);
        display.setCursor(140, 160);
        String tempSinC = globalTemp;
        if(tempSinC.endsWith("C")) tempSinC = tempSinC.substring(0, tempSinC.length()-1);
        display.print(tempSinC);
        display.setTextSize(1);
        display.setCursor(185, 175);
        display.print("C");
      } else if (digitalFace == 3) {
        // ── MODO DIGITAL 3 (CASIO F-91W PREMIUM) ──
        display.fillScreen(COLOR_BLACK);
        
        uint16_t colorCasioBlue = 0x03FF;
        uint16_t colorCasioGold = 0xDE86;
        uint16_t colorLCD = 0xC618; 
        
        // Marco superior
        extern void drawAdaptiveText(String text, int y, int size, uint16_t color, bool centered = true, int maxWidthOverride = 0);
        drawAdaptiveText("WATER RESIST 50M", 15, 1, COLOR_WHITE);
        display.setTextSize(2);
        display.setTextColor(COLOR_WHITE);
        display.setCursor(55, 32); display.print("CASIO");
        display.setTextColor(colorCasioGold);
        display.setCursor(135, 32); display.print("F-91W");
        
        drawAdaptiveText("ALARM CHRONOGRAPH", 58, 1, colorCasioGold);
        display.drawCircle(68, 58, 6, colorCasioGold); 
        
        // Panel LCD principal (Bajado un poco para centrar mejor)
        display.fillRoundRect(22, 80, 196, 105, 15, colorLCD);
        display.drawRoundRect(22, 80, 196, 105, 15, COLOR_WHITE);
        
        // Espacio inferior liberado para Temporizador / Notificaciones
      }
    }

    // Reset de variables para forzar redibujo de manecillas en el siguiente segundo
    clock_last_s = -1;
    clock_last_m = -1;
    clock_last_h = -1;
  }

  // Refresco cada segundo
  bool notifActiva = (millis() < timerNotificacion && timerNotificacion != 0 && msgNotificacion != "");
  if (t.tm_sec != clock_last_s && !notifActiva) {
    if (isAnalog) {
      // 1. Borrar manecillas viejas pintándolas con el color de fondo
      if (clock_last_s >= 0) {
        if (analogFace == 1) drawAnalogHands(clock_last_h, clock_last_m, clock_last_s, COLOR_WHITE);
      }
      
      if (analogFace == 1) {
        // Redibujar números Skilcraft
        display.setTextColor(COLOR_BLACK);
        display.setTextSize(3);
        for (int i=1; i<=12; i++) {
           float angle = (i * 30 - 90) * PI / 180;
           int r_num = 76;
           int x = 120 + cos(angle) * r_num;
           int y = 120 + sin(angle) * r_num;
           display.setCursor(x - (String(i).length()*18)/2, y - 12);
           display.print(i);
        }
      } else if (analogFace == 2) {
        // En lugar de borrar círculos, los redibujaremos encima de las manecillas luego.
      }

      // 3. Redibujar Timer si está activo
      if (timerActivo && timerFin > millis()) {
        unsigned long sec = (timerFin - millis()) / 1000;
        int h = sec / 3600; int m = (sec % 3600) / 60; int s = sec % 60;
        char buf[12];
        if (h > 0) sprintf(buf, "%d:%02d:%02d", h, m, s);
        else sprintf(buf, "%02d:%02d", m, s);
        uint16_t bg = (analogFace == 2) ? COLOR_BLACK : (analogFace == 3 ? 0x3864 : COLOR_WHITE);
        display.setTextSize(2);
        int tw = strlen(buf) * 12;
        display.fillRect(120 - tw/2 - 2, 62, tw + 4, 18, bg);
        display.setTextColor(analogFace == 1 ? COLOR_RED : COLOR_WHITE);
        display.setCursor(120 - tw/2, 64); display.print(buf);
      }

      // 4. Dibujar manecillas nuevas
      if (analogFace == 1) {
        drawAnalogHands(t.tm_hour, t.tm_min, t.tm_sec, COLOR_BLACK);
      } else if (analogFace == 3) {
        uint16_t colorBurgundy = 0x3864;
        if (clock_last_s >= 0) {
          // Erase old Roman hands with colorBurgundy
          float ah = (clock_last_h % 12) * 30 + (clock_last_m / 2.0) - 90; drawThickLine(120,120, 120+cos(ah*PI/180)*55, 120+sin(ah*PI/180)*55, 4, colorBurgundy);
          display.fillCircle(120+cos(ah*PI/180)*40, 120+sin(ah*PI/180)*40, 6, colorBurgundy);
          float am = clock_last_m * 6 - 90; drawThickLine(120,120, 120+cos(am*PI/180)*85, 120+sin(am*PI/180)*85, 3, colorBurgundy);
          display.fillCircle(120+cos(am*PI/180)*65, 120+sin(am*PI/180)*65, 5, colorBurgundy);
          float as = clock_last_s * 6 - 90; display.drawLine(120-cos(as*PI/180)*20, 120-sin(as*PI/180)*20, 120+cos(as*PI/180)*95, 120+sin(as*PI/180)*95, colorBurgundy);
          display.fillCircle(120, 120, 4, colorBurgundy);
          
          // Redraw Roman numerals that might have been erased
          uint16_t colorGold = 0xDE86;
          display.setTextColor(colorGold);
          display.setTextSize(2);
          for (int i=0; i<12; i++) {
             const char* rn = (const char*)pgm_read_ptr(&roman_numerals[i]);
             float angle = (i * 30 - 90) * PI / 180;
             int r_num = 90;
             int x = 120 + cos(angle) * r_num;
             int y = 120 + sin(angle) * r_num;
             display.setCursor(x - (strlen(rn)*12)/2, y - 8);
             display.print(rn);
          }
        }
        drawRomanHands(t.tm_hour, t.tm_min, t.tm_sec);
      } else {
        if(clock_last_s >= 0) {
          // Erase old Sport hands with BLACK
          float ah = (clock_last_h % 12) * 30 + (clock_last_m / 2.0) - 90; drawThickLine(120,120, 120+cos(ah*PI/180)*50, 120+sin(ah*PI/180)*50, 6, COLOR_BLACK);
          float am = clock_last_m * 6 - 90; drawThickLine(120,120, 120+cos(am*PI/180)*85, 120+sin(am*PI/180)*85, 6, COLOR_BLACK);
          float as = clock_last_s * 6 - 90; display.drawLine(120, 120, 120+cos(as*PI/180)*100, 120+sin(as*PI/180)*100, COLOR_BLACK);
        }
        drawSportHands(t.tm_hour, t.tm_min, t.tm_sec);
        
        // Redraw content that might have been erased by hands
        // 1. Temperature (Left)
        display.fillRect(65 - 15, 120 - 8, 30, 16, COLOR_BLACK); // Clean bg
        display.setTextColor(COLOR_WHITE); display.setTextSize(2);
        display.setCursor(65 - (globalTemp.length()*12)/2, 112); display.print(globalTemp);
        
        // 2. Weather (Right)
        display.fillRect(175 - 12, 120 - 12, 24, 24, COLOR_BLACK);
        drawSmallWeatherIcon(175, 120, globalWeatherCode);
        
        // 3. Date (Top) - Movida un poco más arriba para dar espacio al timer
        extern String getDowAbrev(int dow);
        extern int getCurrentDow();
        extern String getCurrentDate();
        String dStr = getDowAbrev(getCurrentDow()) + " " + getCurrentDate().substring(8, 10);
        display.fillRect(120 - 36, 50 - 8, 72, 16, COLOR_BLACK);
        display.setTextColor(COLOR_WHITE); display.setTextSize(2);
        display.setCursor(120 - (dStr.length()*12)/2, 42); display.print(dStr);
        
        // 4. Time (Bottom)
        char tbuf[6]; sprintf(tbuf, "%02d:%02d", t.tm_hour, t.tm_min);
        display.fillRect(120 - 30, 165 - 8, 60, 16, COLOR_BLACK);
        display.setTextColor(COLOR_WHITE); display.setTextSize(2);
        display.setCursor(120 - (5*12)/2, 157); display.print(tbuf);
      }

    } else {
      if (digitalFace == 1) {
        // ── MODO DIGITAL 1 ──
        uint16_t bgColor = getBackgroundColor(globalWeatherCode, t.tm_hour);
        char tbuf[6];
        sprintf(tbuf, "%02d:%02d", t.tm_hour, t.tm_min);
        char sbuf[3];
        sprintf(sbuf, "%02d", t.tm_sec);
        
        display.fillRect(10, 80, 155, 35, bgColor);
        
        display.setTextSize(4);
        display.setTextColor(COLOR_WHITE);
        display.setCursor(15, 85);
        display.print(tbuf);
        
        display.setTextSize(2);
        display.setCursor(135, 101); // Segundos movidos a la izquierda
        display.print(sbuf);
        
        if (timerActivo && timerFin > millis()) {
          unsigned long sec = (timerFin - millis()) / 1000;
          int h = sec / 3600; int m = (sec % 3600) / 60; int s = sec % 60;
          char abuf[10];
          if (h > 0) sprintf(abuf, "%d:%02d:%02d", h, m, s);
          else sprintf(abuf, "%02d:%02d", m, s);
          display.fillRect(15, 173, 110, 20, bgColor);
          display.setTextColor(COLOR_ORANGE); display.setTextSize(2);
          display.setCursor(18, 176); display.print(abuf);
        }
      } else if (digitalFace == 2) {
        // ── MODO DIGITAL 2 ──
        char tbuf[6];
        sprintf(tbuf, "%02d:%02d", t.tm_hour, t.tm_min);
        
        // Hora gigante (si cambió)
        if (clock_last_m != t.tm_min || clock_last_h != t.tm_hour || needFullRedraw) {
          display.fillRect(20, 75, 200, 50, COLOR_BLACK); // borrar
          display.setTextSize(6); // 5 chars * 36px = 180px
          display.setTextColor(COLOR_ORANGE);
          display.setCursor(120 - (5*36)/2, 80);
          display.print(tbuf);
        }

        // Segundos (abajo a la izquierda)
        char sbuf[3];
        sprintf(sbuf, "%02d", t.tm_sec);
        display.fillRect(45, 155, 60, 25, COLOR_BLACK);
        display.setTextSize(3);
        display.setTextColor(COLOR_WHITE);
        display.setCursor(50, 160);
        display.print(sbuf);
        display.setTextSize(1);
        display.setCursor(95, 175);
        display.print("SEG");

        // Actualizar tick de los segundos
        static int last_drawn_sec = -1;
        if (last_drawn_sec != t.tm_sec || needFullRedraw) {
          for(int i=0; i<60; i++) {
             float angle = (i * 6 - 90) * PI / 180;
             int x1 = 120 + cos(angle) * 110;
             int y1 = 120 + sin(angle) * 110;
             int x2 = 120 + cos(angle) * 118;
             int y2 = 120 + sin(angle) * 118;
             uint16_t color = (i <= t.tm_sec) ? COLOR_WHITE : 0xCE59; 
             display.drawLine(x1, y1, x2, y2, color);
          }
          last_drawn_sec = t.tm_sec;
        }

        if (timerActivo && timerFin > millis()) {
          unsigned long sec = (timerFin - millis()) / 1000;
          int h = sec / 3600;
          int m = (sec % 3600) / 60;
          int s = sec % 60;
          char abuf[10];
          if (h > 0) sprintf(abuf, "%d:%02d:%02d", h, m, s);
          else sprintf(abuf, "%02d:%02d", m, s);
          int w = String(abuf).length() * 12;
          display.fillRect(50, 195, 140, 20, COLOR_BLACK);
          display.setTextColor(COLOR_ORANGE);
          display.setTextSize(2);
          display.setCursor(120 - w/2, 195);
          display.print(abuf);
        } else {
          display.fillRect(50, 195, 140, 20, COLOR_BLACK);
        }
      } else if (digitalFace == 3) {
        // ── ACTUALIZAR CONTENIDO LCD CASIO ──
        uint16_t colorLCD = 0xC618;
        
        // Limpiar el fondo del panel LCD antes de redibujar
        display.fillRoundRect(22, 80, 196, 105, 15, colorLCD);
        display.drawRoundRect(22, 80, 196, 105, 15, COLOR_WHITE);

        // 1. Dias de la semana (Arriba del LCD)
        const char* diasS[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
        int dow = t.tm_wday; 
        display.setTextSize(1);
        for(int i=0; i<7; i++) {
          int dx = 35 + i*25;
          display.setCursor(dx, 90);
          if (i == dow) {
            display.setTextColor(COLOR_RED);
            display.drawLine(dx, 88, dx+18, 88, COLOR_RED);
          } else {
            display.setTextColor(COLOR_BLACK);
          }
          display.print(diasS[i]);
        }
        
        // 2. Clima y Fecha
        display.setTextColor(COLOR_BLACK);
        display.setTextSize(2);
        char dbuf[10]; sprintf(dbuf, "%02d-%02d", t.tm_mday, t.tm_mon + 1);
        display.setCursor(135, 110); display.print(dbuf);
        
        drawSmallWeatherIcon(55, 115, globalWeatherCode);
        display.setCursor(75, 110); display.print(globalTemp.substring(0, globalTemp.length()-1) + "o");

        // 3. Hora y Segundos
        char tbuf[6]; sprintf(tbuf, "%02d:%02d", t.tm_hour, t.tm_min);
        char sbuf[3]; sprintf(sbuf, "%02d", t.tm_sec);
        
        display.setTextSize(5); 
        display.setCursor(35, 135); 
        display.print(tbuf);
        
        display.setTextSize(3); 
        display.setCursor(185, 147); 
        display.print(sbuf);

        if (timerActivo && timerFin > millis()) {
          unsigned long sec = (timerFin - millis()) / 1000;
          int h = sec / 3600; int m = (sec % 3600) / 60; int s = sec % 60;
          char abuf[10];
          if (h > 0) sprintf(abuf, "%d:%02d:%02d", h, m, s);
          else sprintf(abuf, "%02d:%02d", m, s);
          int w = String(abuf).length() * 12;
          // Movido al área de "WATER RESIST" para evitar choque con la hora gigante
          display.fillRect(60, 185, 120, 20, COLOR_BLACK); 
          display.setTextColor(COLOR_RED); display.setTextSize(2);
          display.setCursor(120 - w/2, 188); display.print(abuf);
        }
      }
    }
    
    clock_last_h = t.tm_hour;
    clock_last_m = t.tm_min;
    clock_last_s = t.tm_sec;
  }
}

#endif
