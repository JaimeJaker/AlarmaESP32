#ifndef INTERFAZ_H
#define INTERFAZ_H

#include "Config.h"
#include "Relojes.h"
// QRGen cambiado por la libreria estandar de C (qrcode)
#include "qrcode.h"

// QRCode library removed — using text-only URL display

void wakeUp() { lastInteraction = millis(); }

void animateTransition() {
  // Efecto de barrido circular (Circular Wipe) MUCHO MÁS RÁPIDO
  for (int r = 0; r <= 150; r += 25) {
    display.drawCircle(120, 120, r, COLOR_BLACK);
    display.drawCircle(120, 120, r+1, COLOR_BLACK);
    display.drawCircle(120, 120, r+2, COLOR_BLACK);
    display.drawCircle(120, 120, r+3, COLOR_BLACK);
    display.drawCircle(120, 120, r+4, COLOR_BLACK);
    // Sin delay para máxima respuesta
  }
  display.fillScreen(COLOR_BLACK);
}

void drawSplashScreen() {
  unsigned long start = millis();
  const unsigned long maxDuration = 10000; // 10 segundos

  // Primera fase: animación rápida (círculos + texto + barra)
  display.fillScreen(COLOR_BLACK);
  for (int r = 120; r > 0; r -= 20) {
    uint16_t color = (r % 40 == 0) ? COLOR_BLUE : 0x0010;
    display.drawCircle(120, 120, r, color);
    // Comprueba si ya pasó el tiempo máximo o si el reloj está listo
    if (millis() - start >= maxDuration) return;
    delay(20);
  }

  display.setTextColor(COLOR_CYAN);
  display.setTextSize(2);
  String t1 = "ALARMA";
  display.setCursor(120 - (t1.length()*12)/2, 80);
  display.print(t1);

  display.setTextSize(3);
  display.setTextColor(COLOR_WHITE);
  String t2 = "ESP32 V5.1";
  display.setCursor(120 - (t2.length()*18)/2, 110);
  display.print(t2);

  display.setTextSize(1);
  display.setTextColor(COLOR_DARKGREY);
  display.setCursor(120 - (15*6)/2, 150);
  display.print("SISTEMA INICIADO");

  // Barra de progreso animada
  for (int w = 0; w <= 120; w += 10) {
    display.fillRect(60, 170, w, 4, COLOR_CYAN);
    if (millis() - start >= maxDuration) return;
    delay(20);
  }

  // Espera hasta que el reloj esté sincronizado (getLocalTime) o hasta 10s
  struct tm t;
  unsigned long waitStart = millis();
  while (millis() - waitStart < maxDuration) {
    // Si el usuario presiona cualquier botón, salimos antes
    if (digitalRead(BUTTON_PIN) == LOW || digitalRead(BUTTON2_PIN) == LOW || digitalRead(BUTTON3_PIN) == LOW) break;
    // Si la hora local ya está disponible, salimos
    if (getLocalTime(&t)) break;
    delay(100);
  }

  // Pequeña pausa y transición
  delay(200);
  animateTransition();
}

void drawAdaptiveText(String text, int y, int size, uint16_t color, bool centered = true, int maxWidthOverride = 0) {
  display.setTextSize(size);
  int charW = 6 * size;
  int tw = text.length() * charW;
  int dy = abs(y - 120);
  int maxWidth = 0;
  if (dy < 120) maxWidth = 2 * sqrt(14400 - (dy * dy)) - 10;
  if (maxWidthOverride > 0 && maxWidthOverride < maxWidth) maxWidth = maxWidthOverride;
  if (tw > maxWidth && size > 1) { size--; display.setTextSize(size); charW = 6 * size; tw = text.length() * charW; }
  if (tw > maxWidth) { int maxChars = maxWidth / charW; if (maxChars > 0) text = text.substring(0, maxChars); tw = text.length() * charW; }
  display.setTextColor(color);
  int x = centered ? (120 - tw/2) : 25;
  display.setCursor(x, y);
  display.print(text);
}

void wrapText(String text, int maxChars, String lines[], int &lineCount) {
  lineCount = 0;
  text.replace("\r", "");
  text.trim();
  while (text.length() > 0 && lineCount < 10) {
    int nextLine = text.indexOf('\n');
    String segment = (nextLine >= 0) ? text.substring(0, nextLine) : text;
    segment.trim();
    text = (nextLine >= 0) ? text.substring(nextLine + 1) : "";
    while (segment.length() > 0) {
      if (segment.length() <= maxChars) {
        lines[lineCount++] = segment;
        break;
      }
      int splitAt = segment.lastIndexOf(' ', maxChars);
      if (splitAt <= 0) splitAt = maxChars;
      lines[lineCount++] = segment.substring(0, splitAt);
      if (lineCount >= 10) break;
      segment = segment.substring(splitAt);
      segment.trim();
    }
    if (lineCount >= 10) break;
    if (text.length() > 0 && lineCount < 10) continue;
  }
}

void notify(String m, int ms = 8000) {
  msgNotificacion = m;
  timerNotificacion = millis() + ms;
  forceRedraw = true;
}

void drawGlobalNotification(bool fullScreen) {
  if (millis() < timerNotificacion && timerNotificacion != 0 && msgNotificacion != "") {
    if (fullScreen) {
      display.fillScreen(COLOR_BLACK);
      drawAdaptiveText("AVISO", 45, 2, COLOR_YELLOW);
      display.drawLine(40, 65, 200, 65, COLOR_YELLOW);
    }

    int boxY = fullScreen ? 75 : 90;
    int boxH = fullScreen ? 110 : 70;
    int boxX = 20;
    int boxW = 200;
    display.fillRect(boxX, boxY, boxW, boxH, COLOR_RED);
    display.drawRect(boxX, boxY, boxW, boxH, COLOR_WHITE);

    int maxTextWidth = boxW - 16;
    String lines[6];
    int lineCount = 0;

    int chosenSize = 1;
    for (int size = 3; size >= 1; size--) {
      int maxChars = maxTextWidth / (6 * size);
      if (maxChars < 4) continue;
      int testCount = 0;
      wrapText(msgNotificacion, maxChars, lines, testCount);
      int lineHeight = 10 * size;
      if (testCount <= 4 && testCount * lineHeight <= boxH - 18) {
        chosenSize = size;
        break;
      }
    }

    int maxChars = maxTextWidth / (6 * chosenSize);
    lineCount = 0;
    wrapText(msgNotificacion, maxChars, lines, lineCount);
    if (lineCount > 4) {
      String visible = lines[3];
      if (visible.length() > 4) {
        visible = visible.substring(0, maxChars - 4);
        visible.trim();
        visible += "...";
      }
      lines[3] = visible;
      lineCount = 4;
    }

    int lineHeight = 10 * chosenSize;
    int totalHeight = lineCount * lineHeight;
    int currentY = boxY + ((boxH - totalHeight) / 2);

    display.setTextColor(COLOR_WHITE);
    display.setTextSize(chosenSize);
    for (int i = 0; i < lineCount; i++) {
      String line = lines[i];
      line.trim();
      int textWidth = line.length() * 6 * chosenSize;
      int x = boxX + (boxW - textWidth) / 2;
      display.setCursor(x, currentY);
      display.print(line);
      currentY += lineHeight;
    }

    if (fullScreen) {
      if (alarmaActiva >= 0 || timerSonando) {
        display.setTextSize(1);
        display.setTextColor(COLOR_WHITE);
        display.setCursor(120 - (32*6)/2, boxY + boxH + 10);
        display.print("B1: Detener   B3: Posponer 5m");
      }
    }
  }
}

// ── Menú Principal ─────────────────────────────────────
void drawMainMenu() {
  display.fillScreen(COLOR_BLACK);
  int y = 20;
  drawAdaptiveText("1.Relojes", y, 2, mainMenuIndex == 0 ? COLOR_GREEN : COLOR_WHITE);
  drawAdaptiveText("2.Cronometro", y+26, 2, mainMenuIndex == 1 ? COLOR_GREEN : COLOR_WHITE);
  drawAdaptiveText("3.Timer", y+52, 2, mainMenuIndex == 2 ? COLOR_GREEN : COLOR_WHITE);
  drawAdaptiveText("4.Alarma", y+78, 2, mainMenuIndex == 3 ? COLOR_GREEN : COLOR_WHITE);
  drawAdaptiveText("5.Linterna", y+104, 2, mainMenuIndex == 4 ? (linternaActiva ? COLOR_YELLOW : COLOR_GREEN) : (linternaActiva ? COLOR_YELLOW : COLOR_WHITE));
  drawAdaptiveText("6.Juegos", y+130, 2, mainMenuIndex == 5 ? COLOR_GREEN : COLOR_WHITE);
  drawAdaptiveText("7.Panel Web", y+156, 2, mainMenuIndex == 6 ? COLOR_GREEN : COLOR_WHITE);
  
  display.setTextSize(1); display.setTextColor(0x39E7);
  display.setCursor(120 - (23*6)/2, 220); display.print("B1:Atras B2:Abajo B3:Ok");
}

// ── Selección de Minijuego ─────────────────────────────
void drawSeleccionJuego() {
  display.fillScreen(COLOR_BLACK);
  display.setTextColor(COLOR_WHITE); display.setTextSize(2);
  display.setCursor(120 - (10*12)/2, 40); display.print("Minijuegos");
  display.drawLine(40, 60, 200, 60, COLOR_CYAN);
  
  String juegos[] = {"1. Flappy", "2. Asteroids", "3. Runner"};
  for (int i=0; i<3; i++) {
    display.setCursor(30, 90 + i*25);
    if (i == gameIndex) { display.setTextColor(COLOR_GREEN); display.print("> " + juegos[i]); }
    else { display.setTextColor(COLOR_WHITE); display.print("  " + juegos[i]); }
  }
  display.setTextSize(1); display.setTextColor(COLOR_DARKGREY);
  display.setCursor(120 - (22*6)/2, 200); display.print("B1:Atras B2:Nav B3:Sel");
}

// Panel Web: QR + IP + URL (Ajustado para pantalla circular)
void drawQR() {
  display.fillScreen(COLOR_BLACK);
  String panelIp = WiFi.status() == WL_CONNECTED ? WiFi.localIP().toString() : WiFi.softAPIP().toString();
  String panelUrl = "http://" + panelIp + ":81";
  String portalUrl = "http://192.168.4.1";
  
  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(2)];
  qrcode_initText(&qrcode, qrcodeData, 2, 0, panelUrl.c_str()); // Version 2, ECC LOW (0)
  
  int sz = qrcode.size;
  if (sz > 0) {
    const int mod = 4;                   // Forzar 4px para que quepa bien en el circulo
    const int qrPx = sz * mod;
    const int qz = mod * 3;              // Zona blanca de 3 modulos
    const int qrX  = (240 - qrPx) / 2;
    const int qrY  = 30;                 // Bajarlo un poco mas hacia el centro
    
    // Fondo blanco grande que incluye el Quiet Zone
    display.fillRect(qrX - qz, qrY - qz, qrPx + 2*qz, qrPx + 2*qz, COLOR_WHITE);
    
    for (int y = 0; y < sz; y++)
      for (int x = 0; x < sz; x++)
        if (qrcode_getModule(&qrcode, x, y)) 
          display.fillRect(qrX+x*mod, qrY+y*mod, mod, mod, COLOR_BLACK);
          
    int ty = qrY + qrPx + qz + 2;
    display.setTextColor(COLOR_CYAN); display.setTextSize(1);
    display.setCursor(120-(int)(panelUrl.length()*6)/2, ty); display.print("Panel Web:");
    display.setCursor(120-(int)(panelUrl.length()*6)/2, ty+12); display.print(panelUrl);
    display.setTextColor(COLOR_YELLOW); display.setCursor(120-(int)(portalUrl.length()*6)/2, ty+26); display.print("Portal Wifi:");
    display.setCursor(120-(int)(portalUrl.length()*6)/2, ty+38); display.print(portalUrl);
  } else {
    // Fallback texto si QR falla
    display.setTextColor(COLOR_CYAN); display.setTextSize(2);
    display.setCursor(120-(int)(panelIp.length()*12)/2, 90); display.print(panelIp);
    display.setTextColor(COLOR_YELLOW); display.setTextSize(1);
    display.setCursor(120-(int)(panelUrl.length()*6)/2,125); display.print(panelUrl);
  }
}

// ── Submenú Alarma ─────────────────────────────────────
void drawAlarmSubmenu() {
  display.fillScreen(COLOR_BLACK);
  drawAdaptiveText("Menu Alarma", 25, 2, COLOR_WHITE);
  display.drawLine(30, 47, 210, 47, COLOR_CYAN);
  drawAdaptiveText("1.Nueva Alarma", 70, 2, menuOpcion == 0 ? COLOR_GREEN : COLOR_WHITE);
  drawAdaptiveText("2.Ver Lista", 105, 2, menuOpcion == 1 ? COLOR_GREEN : COLOR_WHITE);
  drawAdaptiveText("3.Fecha", 140, 2, menuOpcion == 2 ? COLOR_GREEN : COLOR_WHITE);
  drawAdaptiveText("Especifica", 165, 2, menuOpcion == 2 ? COLOR_GREEN : COLOR_WHITE);
}

// ── Tipo de Diseño ────────────────────────────────────
void drawDisenoTipo() {
  display.fillScreen(COLOR_BLACK);
  display.setTextColor(COLOR_WHITE); display.setTextSize(2);
  display.setCursor(120 - (8*12)/2, 20); display.print("Fondo de");
  display.setCursor(120 - (8*12)/2, 45); display.print("Pantalla");
  String opts[2] = {"1.Analogo", "2.Digital"};
  for (int i=0; i<2; i++) {
    display.setCursor(120 - (opts[i].length()*12)/2, 100 + i*30);
    if (i == disenoTipoIndex) display.setTextColor(COLOR_GREEN);
    else display.setTextColor(COLOR_WHITE);
    display.print(opts[i]);
  }
}

// ── Selector de Cara ──────────────────────────────────
void drawFaceMenu(String title) {
  display.fillScreen(COLOR_BLACK);
  display.setTextColor(COLOR_WHITE); display.setTextSize(2);
  // Línea 1: "Fondo de"
  display.setCursor(120 - (8*12)/2, 18); display.print("Fondo de");
  // Línea 2: "Pantalla"
  display.setCursor(120 - (8*12)/2, 42); display.print("Pantalla");
  // Línea 3: "Analogo" o "Digital" — centrado independiente
  String tipo = "";
  if (title.startsWith("Fondo de Pantalla ")) tipo = title.substring(18);
  else tipo = title;
  display.setCursor(120 - (tipo.length()*12)/2, 66);
  display.setTextColor(COLOR_CYAN);
  display.print(tipo);
  String opts[3] = {"Opcion 1", "Opcion 2", "Opcion 3"};
  for (int i=0; i<3; i++) {
    display.setCursor(120 - (opts[i].length()*12)/2, 105 + i*35);
    if (i == faceMenuIndex) display.setTextColor(COLOR_GREEN);
    else display.setTextColor(COLOR_WHITE);
    display.print(opts[i]);
  }
}

// ── Cronómetro ────────────────────────────────────────
void drawCronometro() {
  display.fillScreen(COLOR_BLACK);
  display.setTextColor(COLOR_WHITE); display.setTextSize(2);
  display.setCursor(120 - (10*12)/2, 40); display.print("Cronometro");
  display.setTextSize(1); display.setTextColor(0x39E7);
  display.setCursor(120 - (20*6)/2, 185); display.print("B2:Rst B3:Play/Pausa");

  unsigned long t = cronoElapsed;
  if (cronoRunning) t += (millis() - cronoStart);
  int ms = (t % 1000) / 10, s = (t / 1000) % 60, m = (t / 60000) % 60, h = (t / 3600000);
  char buf[16];
  if (h > 0) sprintf(buf, "%02d:%02d:%02d", h, m, s);
  else sprintf(buf, "%02d:%02d.%02d", m, s, ms);
  display.fillRect(25, 90, 190, 32, COLOR_BLACK);
  display.setTextSize(3); display.setTextColor(COLOR_CYAN);
  int len = 8;
  display.setCursor(120 - (len*18)/2, 95);
  display.print(buf);

  display.setTextSize(1);
  display.setTextColor(cronoRunning ? COLOR_GREEN : COLOR_YELLOW);
  display.setCursor(120 - (9*6)/2, 140);
  display.print(cronoRunning ? "CORRIENDO" : "PAUSADO  ");
}

// ── Configurar Temporizador ───────────────────────────
void drawTempoSet() {
  display.fillScreen(COLOR_BLACK);
  display.setTextColor(COLOR_WHITE); display.setTextSize(2);
  display.setCursor(120 - (12*12)/2, 40); display.print("Temporizador");
  char buf[12];
  sprintf(buf, "%02d:%02d:%02d", tempoSetHour, tempoSetMin, tempoSetSec);
  display.setTextSize(3);
  int x = 120 - (8*18)/2;
  display.setCursor(x, 100);
  display.setTextColor(tempoStage == 0 ? COLOR_GREEN : COLOR_WHITE);
  display.print(buf[0]); display.print(buf[1]);
  display.setTextColor(COLOR_WHITE); display.print(":");
  display.setTextColor(tempoStage == 1 ? COLOR_GREEN : COLOR_WHITE);
  display.print(buf[3]); display.print(buf[4]);
  display.setTextColor(COLOR_WHITE); display.print(":");
  display.setTextColor(tempoStage == 2 ? COLOR_GREEN : COLOR_WHITE);
  display.print(buf[6]); display.print(buf[7]);
  display.setTextSize(1); display.setTextColor(0x39E7);
  display.setCursor(120 - (29*6)/2, 180);
  display.print("B1:- B2:+ B3:Sig  B1+B2:Atras");
}

// ── Lista de Alarmas ──────────────────────────────────
void drawListaAlarmas(int selectIdx) {
  display.fillScreen(COLOR_BLACK);
  if (numAlarmas == 0) { drawAdaptiveText("Sin alarmas!", 110, 2, COLOR_WHITE); forceRedraw = true; return; }
  drawAdaptiveText("Mis Alarmas", 20, 2, COLOR_WHITE);
  display.drawLine(30, 42, 210, 42, COLOR_CYAN);
  int startIdx = 0;
  if (selectIdx > 2) startIdx = selectIdx - 2;
  if (startIdx + 4 > numAlarmas) startIdx = max(0, numAlarmas - 4);
  for (int i = 0; i < 4 && (startIdx + i) < numAlarmas; i++) {
    int idx = startIdx + i; int y = 55 + i * 35;
    if (idx == selectIdx) { display.fillRect(10, y - 5, 220, 30, 0x18E3); display.setTextColor(COLOR_GREEN); }
    else display.setTextColor(COLOR_WHITE);
    display.setTextSize(2); display.setCursor(20, y); display.print(alarmas[idx].hora);
    display.setTextSize(1); display.setCursor(95, y + 4);
    String d = alarmas[idx].dias; if (d.length() > 15) d = d.substring(0, 15) + ".";
    display.print(d);
  }
  display.setTextSize(1); display.setTextColor(0x39E7);
  display.setCursor(120 - (24*6)/2, 205); display.print("B1:Atras B2:Abajo B3:Sel");
  // No poner forceRedraw = true aqui: causa que drawNormal() se dibuje encima
}

// ── Opciones de Alarma ────────────────────────────────
void drawOpciones(int idx) {
  String opciones[] = {"Borrar", "Cambiar hora"};
  display.fillScreen(COLOR_BLACK);
  display.setTextColor(COLOR_WHITE); display.setTextSize(2);
  display.setCursor(120 - (8*12)/2, 40); display.print("Opciones");
  display.drawLine(40, 60, 200, 60, COLOR_CYAN);
  for (int i = 0; i < 2; i++) {
    display.setCursor(40, 90 + i * 35);
    if (i == menuOpcion) { display.setTextColor(COLOR_GREEN); display.print("> " + opciones[i]); }
    else { display.setTextColor(COLOR_WHITE); display.print("  " + opciones[i]); }
  }
  display.setTextSize(1); display.setCursor(120 - (22*6)/2, 190);
  display.setTextColor(COLOR_DARKGREY); display.print("B1:Atras B2:Nav B3:Sel");
}

// ── Modificar Hora ────────────────────────────────────
void drawModificar(int idx, bool creating = false) {
  display.fillScreen(COLOR_BLACK);
  display.setTextColor(COLOR_WHITE); display.setTextSize(2);
  if (creating) { display.setCursor(120 - (4*12)/2, 40); display.print("Hora"); }
  else { display.setCursor(120 - (9*12)/2, 40); display.print("Modificar"); }
  display.drawLine(40, 60, 200, 60, COLOR_CYAN);
  display.setTextSize(4); display.setCursor(120 - (5*24)/2, 100);
  display.setTextColor(COLOR_YELLOW); display.print(alarmas[idx].hora);
  display.setTextSize(1); display.setCursor(120 - (19*6)/2, 190);
  display.setTextColor(COLOR_DARKGREY); display.print("B1:Atras B2:+ B3:OK");
}

// ── Selección de Tono ────────────────────────────────
void drawAlarmAddTone(int wTone, bool preview) {
  display.fillScreen(COLOR_BLACK);
  display.setTextColor(COLOR_WHITE); display.setTextSize(2);
  display.setCursor(120 - (5*12)/2, 25); display.print("Tonos");
  display.drawLine(30, 47, 210, 47, COLOR_CYAN);
  String tones[] = {"StarWars", "Imperial", "Beep Beep", "Sirena", "Rapido"};
  for (int i=0; i<5; i++) {
    display.setCursor(45, 60 + i*28);
    if (i == wTone) { display.setTextColor(COLOR_GREEN); display.print("> " + tones[i]); }
    else { display.setTextColor(COLOR_WHITE); display.print("  " + tones[i]); }
  }
  display.setTextSize(1); display.setTextColor(0x39E7);
  display.setCursor(120 - (25*6)/2, 205); display.print("B2:Nav B3:Ok B2+B1:Probar");
}

// ── Repetición de Alarma ─────────────────────────────
void drawAlarmAddRepeat(int wRepeat) {
  display.fillScreen(COLOR_BLACK);
  display.setTextColor(COLOR_WHITE); display.setTextSize(2);
  display.setCursor(120 - (7*12)/2, 25); display.print("Repetir");
  display.drawLine(30, 47, 210, 47, COLOR_CYAN);
  String opts[] = {"Una vez", "Lun-Vie", "Personal."};
  for (int i=0; i<3; i++) {
    display.setCursor(45, 75 + i*35);
    if (i == wRepeat) { display.setTextColor(COLOR_GREEN); display.print("> " + opts[i]); }
    else { display.setTextColor(COLOR_WHITE); display.print("  " + opts[i]); }
  }
  display.setTextSize(1); display.setTextColor(0x39E7);
  display.setCursor(120 - (23*6)/2, 205); display.print("B1:Atras B2:Abajo B3:Ok");
}

// ── Días Personalizados ──────────────────────────────
void drawAlarmAddCustom(bool* wDays) {
  display.fillScreen(COLOR_BLACK);
  display.setTextColor(COLOR_WHITE); display.setTextSize(2);
  display.setCursor(120 - (11*12)/2, 25); display.print("Dias Custom");
  display.drawLine(30, 47, 210, 47, COLOR_CYAN);
  const char* d[] = {"Lun","Mar","Mie","Jue","Vie","Sab","Dom"};
  for (int i=0; i<7; i++) {
    int x = 30 + (i%4)*52, y = 65 + (i/4)*45;
    if (i == menuIndice) display.drawRect(x-3, y-3, 46, 28, COLOR_CYAN);
    if (wDays[i]) display.setTextColor(COLOR_GREEN);
    else display.setTextColor(COLOR_DARKGREY);
    display.setCursor(x, y); display.print(d[i]);
  }
  display.setTextSize(1); display.setTextColor(COLOR_WHITE);
  display.setCursor(120 - (23*6)/2, 195); display.print("B3:Sel  B1+B3:Guardar");
}

// ── Fecha Específica ─────────────────────────────────
void drawAlarmAddDate(int year, int month, int day, int hour, int min, int stage) {
  display.fillScreen(COLOR_BLACK);
  display.setTextColor(COLOR_WHITE); display.setTextSize(2);
  display.setCursor(120 - (11*12)/2, 25); display.print("Elije Fecha");
  display.drawLine(30, 47, 210, 47, COLOR_CYAN);

  display.setCursor(30, 65);
  display.setTextColor(stage == 0 ? COLOR_CYAN : COLOR_DARKGREY); display.print("Anio:");
  display.setTextColor(stage == 0 ? COLOR_GREEN : COLOR_WHITE); display.print(String(year));

  display.setCursor(30, 95);
  display.setTextColor(stage == 1 ? COLOR_CYAN : COLOR_DARKGREY); display.print("Mes: ");
  display.setTextColor(stage == 1 ? COLOR_GREEN : COLOR_WHITE);
  display.print(String(month < 10 ? "0" : "") + String(month));

  display.setCursor(30, 125);
  display.setTextColor(stage == 2 ? COLOR_CYAN : COLOR_DARKGREY); display.print("Dia: ");
  display.setTextColor(stage == 2 ? COLOR_GREEN : COLOR_WHITE);
  display.print(String(day < 10 ? "0" : "") + String(day));

  display.setCursor(30, 155);
  display.setTextColor(stage == 3 ? COLOR_CYAN : COLOR_DARKGREY); display.print("Hora:");
  display.setTextColor(stage == 3 ? COLOR_GREEN : COLOR_WHITE);
  display.print(String(hour < 10 ? "0" : "") + String(hour) + ":");
  display.setTextColor(stage == 4 ? COLOR_GREEN : COLOR_WHITE);
  display.print(String(min < 10 ? "0" : "") + String(min));

  display.setTextSize(1); display.setTextColor(0x39E7);
  display.setCursor(120 - (27*6)/2, 200); display.print("B1:Atras B2:Cambiar B3:Sig.");
}

// ── Auto-borrar al sonar ─────────────────────────────
void drawAlarmDeleteOnFire(int opcion) {
  display.fillScreen(COLOR_BLACK);
  display.setTextColor(COLOR_WHITE); display.setTextSize(2);
  display.setCursor(120 - (12*12)/2, 25); display.print("Auto-borrar?");
  display.drawLine(30, 47, 210, 47, COLOR_CYAN);
  display.setCursor(120 - (13*12)/2, 80); display.setTextColor(COLOR_WHITE); display.print("Borrar alarma");
  display.setCursor(120 - (9*12)/2, 100); display.print("al sonar?");
  display.setCursor(35, 140);
  if (opcion == 0) display.setTextColor(COLOR_GREEN); else display.setTextColor(COLOR_WHITE);
  display.print("> Si, borrar");
  display.setCursor(35, 170);
  if (opcion == 1) display.setTextColor(COLOR_GREEN); else display.setTextColor(COLOR_WHITE);
  display.print("> No, guardar");
  display.setTextSize(1); display.setTextColor(0x39E7);
  display.setCursor(120 - (27*6)/2, 205); display.print("B1/B2:Cambiar  B3:Confirmar");
}

// ── Animación de Alarma ──────────────────────────────
void drawAlarmAnimation(String nombre, String hora) {
  static bool toggle = false; static unsigned long lastAnim = 0;
  if (millis() - lastAnim < 400) return;
  toggle = !toggle; lastAnim = millis();
  
  display.fillScreen(toggle ? COLOR_RED : COLOR_BLACK);
  display.setTextColor(toggle ? COLOR_WHITE : COLOR_RED);
  
  drawAdaptiveText("ALARMA", 30, 3, toggle ? COLOR_WHITE : COLOR_RED);
  drawAdaptiveText(hora, 90, 5, toggle ? COLOR_WHITE : COLOR_YELLOW);
  drawAdaptiveText(nombre, 150, 2, toggle ? COLOR_WHITE : COLOR_RED);
  drawAdaptiveText("B1+B2: DETENER", 195, 1, toggle ? COLOR_WHITE : COLOR_RED);
  forceRedraw = true;
}

// ── Animación de Timer ───────────────────────────────
void drawTimerAnimation() {
  static bool toggle = false; static unsigned long lastAnim = 0;
  if (millis() - lastAnim < 500) return;
  toggle = !toggle; lastAnim = millis();
  
  display.fillScreen(toggle ? COLOR_BLUE : COLOR_BLACK);
  display.setTextColor(toggle ? COLOR_WHITE : COLOR_CYAN);
  
  drawAdaptiveText("TIMER", 60, 3, toggle ? COLOR_WHITE : COLOR_CYAN);
  drawAdaptiveText("Completado!", 120, 2, toggle ? COLOR_WHITE : COLOR_WHITE);
  drawAdaptiveText("B1+B2: DETENER", 180, 1, toggle ? COLOR_WHITE : COLOR_CYAN);
  forceRedraw = true;
}

#endif
