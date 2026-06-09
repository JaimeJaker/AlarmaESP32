#ifndef LOGICA_H
#define LOGICA_H

#include "Config.h"
#include "Alarms.h"
#include "Interfaz.h"
#include "Clima.h"
#include "Relojes.h"
#include "Flapply_Bird.h"
#include "Asteroids.h"
#include "Runner.h"

void stopAlarm(bool snooze = false) {
  noTone(BUZZER_PIN); digitalWrite(BUZZER_PIN, LOW);
  strip.clear(); strip.show();

  if (timerSonando) {
    if (snooze) {
      timerSonando = false;
      timerActivo = true;
      timerFin = millis() + 5UL * 60UL * 1000UL;
      forceRedraw = true;
      alarmaActiva = -1;
      mqttClient.publish(topicStatus, "Timer pospuesto 5min");
      notify("Timer pospuesto\n5 minutos", 3000);
      return;
    }
    timerSonando = false; timerActivo = false; forceRedraw = true; alarmaActiva = -1;
    mqttClient.publish(topicStatus, "Timer detenido"); return;
  }
  
  timerSonando = false; forceRedraw = true;
  
  if (snooze && alarmaActiva >= 0) {
    struct tm t; getLocalTime(&t); t.tm_min += 5; mktime(&t);
    char buf[6]; strftime(buf, sizeof(buf), "%H:%M", &t);
    alarmas[alarmaActiva].hora = String(buf); alarmas[alarmaActiva].lastFired = "";
    mqttClient.publish(topicStatus, ("SNOOZE 5min -> " + String(buf)).c_str());
    notify("Alarma pospuesta\n5 minutos", 3000);
  } else if (alarmaActiva >= 0) {
    alarmas[alarmaActiva].lastFired = alarmas[alarmaActiva].hora;
    mqttClient.publish(topicStatus, ("APAGADA: " + alarmas[alarmaActiva].nombre).c_str());
    if (alarmas[alarmaActiva].deleteOnFire) {
      String n = alarmas[alarmaActiva].nombre;
      for (int i = alarmaActiva; i < numAlarmas - 1; i++) alarmas[i] = alarmas[i+1];
      numAlarmas--; saveAlarmas(); notify("Borrada: " + n);
    }
  } else if (timerActivo) {
    timerActivo = false; timerSonando = false;
    mqttClient.publish(topicStatus, "Timer detenido"); notify("Timer\ndetenido", 2000);
  } else {
    mqttClient.publish(topicStatus, "No hay alarmas activas"); notify("No hay alarmas\nactivas", 3000);
  }
  alarmaActiva = -1;
}

bool deleteAlarma(String nombre) {
  for (int i = 0; i < numAlarmas; i++) {
    if (alarmas[i].nombre == nombre) {
      for (int j = i; j < numAlarmas - 1; j++) alarmas[j] = alarmas[j+1];
      numAlarmas--; saveAlarmas(); notify("Eliminada: " + nombre); return true;
    }
  }
  return false;
}

bool modificarAlarma(String msg) {
  int p1 = msg.indexOf('|'), p2 = (p1 >= 0) ? msg.indexOf('|', p1 + 1) : -1;
  if (p1 < 0 || p2 < 0) return false;
  String nombre = msg.substring(0, p1); nombre.trim();
  String diasNuev = msg.substring(p1+1, p2); diasNuev.trim();
  String horaNuev = msg.substring(p2+1); horaNuev.trim();
  for (int i = 0; i < numAlarmas; i++) {
    if (alarmas[i].nombre.equalsIgnoreCase(nombre)) {
      if (diasNuev != "*") { alarmas[i].dias = normalizarDias(diasNuev); alarmas[i].esFecha = (alarmas[i].dias.indexOf('-') >= 0); }
      if (horaNuev != "*" && horaNuev.length() == 5) alarmas[i].hora = horaNuev;
      alarmas[i].lastFired = ""; saveAlarmas(); notify("Modificada: " + alarmas[i].nombre); return true;
    }
  }
  return false;
}

extern void publishList();

void procesarBotones() {
  // ── Apagar alarma presionando AMBOS (B1 + B2) ────────
  if ((alarmaActiva >= 0 || timerSonando) &&
      digitalRead(BUTTON_PIN) == LOW && digitalRead(BUTTON2_PIN) == LOW) {
    delay(50);
    if (digitalRead(BUTTON_PIN) == LOW && digitalRead(BUTTON2_PIN) == LOW) {
      stopAlarm(false);
      buttonPressed = false;
      btn2Pressed = false;
      noTone(BUZZER_PIN);
      forceRedraw = true;
      delay(300);
      return;
    }
  }

  // ── Controles de Juego: respuesta INMEDIATA al presionar ──────────────
  // Solo se activan mientras el juego está en curso (no en Game Over).
  // Al terminar el juego, la lógica normal de "release" maneja salir/reiniciar.

  if (menuEstado == MENU_RUNNER && !runGameOver) {
    if (btnImm1) { btnImm1 = false; buttonPressed = false; runMove(-1); wakeUp(); }
    if (btnImm2) { btnImm2 = false; btn2Pressed = false; runMove(1);  wakeUp(); }
    if (btnImm3) { btnImm3 = false; btn3Pressed = false; runJump();   wakeUp(); }
    // Limpiar cualquier flag residual y salir (no procesar lógica de menú)
    btnImm1 = btnImm2 = btnImm3 = false;
    return;
  }

  if (menuEstado == MENU_MINIJUEGO && !gameOver) {
    if (btnImm3) { btnImm3 = false; btn3Pressed = false; birdVelocity = -6.0; tone(BUZZER_PIN, 600, 50); wakeUp(); }
    btnImm1 = btnImm2 = btnImm3 = false;
    return;
  }

  // Asteroids usa polling directo (digitalRead), así que solo limpiamos flags (excepto disparo)
  if (menuEstado == MENU_ASTEROIDS && !astGameOver) {
    if (btnImm3) { btnImm3 = false; btn3Pressed = false; astShoot(); wakeUp(); }
    btnImm1 = btnImm2 = btnImm3 = false;
    return;
  }

  // Solo consumir controles inmediatos de juego cuando realmente estamos EN MODO PLAY
  // Cooldown anti-doble-pulsación para menús: 250ms entre acciones
  static unsigned long lastMenuAction = 0;
  if (millis() - lastMenuAction < 250) {
    btnImm1 = btnImm2 = btnImm3 = false;
    return;
  }

  // ── Botón 1: Retroceder / Salir ──
  if (btnImm1) {
    btnImm1 = false; lastMenuAction = millis();
    if (alarmaActiva >= 0 || timerSonando) {
      stopAlarm(false); forceRedraw = true;
    } else {
      forceRedraw = true; wakeUp();
      if (menuEstado == MENU_NORMAL) { menuEstado = MENU_MAIN; mainMenuIndex = 0; linternaActiva = false; }
      else if (menuEstado == MENU_MAIN) { menuEstado = MENU_NORMAL; lastDisplay = 0; resetClockDraw(); linternaActiva = false; }
      else if (menuEstado == MENU_DISENO_TIPO || menuEstado == MENU_CRONOMETRO || menuEstado == MENU_ALARM_SUBMENU || menuEstado == MENU_SELECCION_JUEGO || menuEstado == MENU_QR) { menuEstado = MENU_MAIN; forceRedraw = true; linternaActiva = false; }
      else if (menuEstado == MENU_DISENO_ANALOGO || menuEstado == MENU_DISENO_DIGITAL) { menuEstado = MENU_DISENO_TIPO; forceRedraw = true; }
      else if (menuEstado == MENU_ALARM_ADD_TONE) {
        if (digitalRead(BUTTON2_PIN) == LOW) { previewActivo = !previewActivo; if (!previewActivo) noTone(BUZZER_PIN); forceRedraw = true; }
        else { menuEstado = MENU_ALARM_SUBMENU; menuOpcion = 0; previewActivo = false; noTone(BUZZER_PIN); forceRedraw = true; }
      }
      else if (menuEstado == MENU_ALARM_ADD_REPEAT) { menuEstado = MENU_ALARM_ADD_TONE; forceRedraw = true; }
      else if (menuEstado == MENU_ALARM_ADD_CUSTOM) { menuEstado = MENU_ALARM_ADD_REPEAT; forceRedraw = true; }
      else if (menuEstado == MENU_ALARM_ADD_DATE) { if (wizardDateStage > 0) { wizardDateStage--; forceRedraw = true; } else { menuEstado = MENU_ALARM_ADD_REPEAT; forceRedraw = true; } }
      else if (menuEstado == MENU_LISTA) { menuEstado = MENU_ALARM_SUBMENU; menuOpcion = 0; forceRedraw = true; }
      else if (menuEstado == MENU_OPCIONES) { menuEstado = MENU_LISTA; forceRedraw = true; }
      else if (menuEstado == MENU_MODIFICAR) {
        if (digitalRead(BUTTON2_PIN) == LOW || digitalRead(BUTTON3_PIN) == LOW) {
          if (isCreatingAlarm) numAlarmas--;
          menuEstado = MENU_ALARM_SUBMENU; forceRedraw = true;
        } else {
          int h = alarmas[menuIndice].hora.substring(0,2).toInt();
          int m = alarmas[menuIndice].hora.substring(3,5).toInt();
          m -= alarmStep; if (m < 0) { m += 60; h = (h - 1 + 24) % 24; }
          char buf[6]; sprintf(buf, "%02d:%02d", h, m); alarmas[menuIndice].hora = String(buf);
          forceRedraw = true;
        }
      }
      else if (menuEstado == MENU_TEMPO_SET) {
        if (digitalRead(BUTTON2_PIN) == LOW || digitalRead(BUTTON3_PIN) == LOW) {
          if (tempoStage > 0) { tempoStage--; forceRedraw = true; }
          else { menuEstado = MENU_MAIN; lastDisplay = 0; forceRedraw = true; }
        } else {
          if (tempoStage == 0) tempoSetHour = (tempoSetHour - 1 + 24) % 24;
          else if (tempoStage == 1) tempoSetMin = (tempoSetMin - 1 + 60) % 60;
          else if (tempoStage == 2) tempoSetSec = (tempoSetSec - 1 + 60) % 60;
          forceRedraw = true;
        }
      }
      else if (menuEstado == MENU_MINIJUEGO) { menuEstado = MENU_SELECCION_JUEGO; forceRedraw = true; }
      else if (menuEstado == MENU_ASTEROIDS) { if (astGameOver) { menuEstado = MENU_SELECCION_JUEGO; forceRedraw = true; } }
      else if (menuEstado == MENU_RUNNER) { if (runGameOver) { menuEstado = MENU_SELECCION_JUEGO; forceRedraw = true; } else { runMove(-1); } }
      else { menuEstado = MENU_NORMAL; lastDisplay = 0; forceRedraw = true; }
    }
  }

  // ── Botón 2: Siguiente / Bajar ──
  if (btnImm2) {
    btnImm2 = false;
    forceRedraw = true; wakeUp();
    if (menuEstado == MENU_MAIN) mainMenuIndex = (mainMenuIndex + 1) % 7;
    else if (menuEstado == MENU_DISENO_TIPO) disenoTipoIndex = (disenoTipoIndex + 1) % 2;
    else if (menuEstado == MENU_DISENO_ANALOGO || menuEstado == MENU_DISENO_DIGITAL) faceMenuIndex = (faceMenuIndex + 1) % 3;
    else if (menuEstado == MENU_ALARM_SUBMENU) menuOpcion = (menuOpcion + 1) % 3;
    else if (menuEstado == MENU_ALARM_ADD_TONE) { wizardTone = (wizardTone + 1) % 5; previewActivo = false; noTone(BUZZER_PIN); }
    else if (menuEstado == MENU_ALARM_ADD_REPEAT) wizardRepeat = (wizardRepeat + 1) % 3;
    else if (menuEstado == MENU_ALARM_ADD_CUSTOM) menuIndice = (menuIndice + 1) % 7;
    else if (menuEstado == MENU_ALARM_ADD_DATE) {
      if (wizardDateStage == 0) wizardYear++;
      else if (wizardDateStage == 1) { wizardMonth++; if (wizardMonth > 12) wizardMonth = 1; }
      else if (wizardDateStage == 2) { wizardDay++; if (wizardDay > 31) wizardDay = 1; }
      else if (wizardDateStage == 3) wizardHour = (wizardHour + 1) % 24;
      else wizardMin = (wizardMin + 5) % 60;
    }
    else if (menuEstado == MENU_ALARM_DELETE_FIRE) menuOpcion = (menuOpcion + 1) % 2;
    else if (menuEstado == MENU_CRONOMETRO) {
      cronoStart = millis(); cronoElapsed = 0; cronoRunning = false;
      forceRedraw = true;
    }
    else if (menuEstado == MENU_TEMPO_SET) {
      if (tempoStage == 0) tempoSetHour = (tempoSetHour + 1) % 24;
      else if (tempoStage == 1) tempoSetMin = (tempoSetMin + 1) % 60;
      else if (tempoStage == 2) tempoSetSec = (tempoSetSec + 1) % 60;
      forceRedraw = true;
    }
    else if (menuEstado == MENU_LISTA && numAlarmas > 0) menuIndice = (menuIndice + 1) % numAlarmas;
    else if (menuEstado == MENU_OPCIONES) menuOpcion = (menuOpcion + 1) % 2;
    else if (menuEstado == MENU_MODIFICAR) {
      int h = alarmas[menuIndice].hora.substring(0,2).toInt();
      int m = alarmas[menuIndice].hora.substring(3,5).toInt();
      m += alarmStep; if (m >= 60) { m -= 60; h = (h + 1) % 24; }
      char buf[6]; sprintf(buf, "%02d:%02d", h, m); alarmas[menuIndice].hora = String(buf);
    }
    else if (menuEstado == MENU_NORMAL) { menuOpcion = 0; menuEstado = MENU_ALARM_SUBMENU; }
    else if (menuEstado == MENU_MINIJUEGO) { if (gameOver) initMinijuego(); }
    else if (menuEstado == MENU_ASTEROIDS) { if (astGameOver) initAsteroids(); }
    else if (menuEstado == MENU_RUNNER) { if (runGameOver) initRunner(); else runMove(1); }
    else if (menuEstado == MENU_SELECCION_JUEGO) { gameIndex = (gameIndex + 1) % 3; } 
  }

  // ── Botón 3: Acción (Enter/OK) ──
  if (btnImm3) {
    btnImm3 = false;
    forceRedraw = true; wakeUp();
    if (alarmaActiva >= 0 || timerSonando) {
      stopAlarm(true); forceRedraw = true;
    } else if (menuEstado == MENU_NORMAL) { isAnalog = !isAnalog; lastDisplay = 0; resetClockDraw(); }
    else if (menuEstado == MENU_MAIN) {
      if (mainMenuIndex == 0) { menuEstado = MENU_DISENO_TIPO; disenoTipoIndex = 0; }
      else if (mainMenuIndex == 1) menuEstado = MENU_CRONOMETRO;
      else if (mainMenuIndex == 2) { menuEstado = MENU_TEMPO_SET; tempoSetHour = 0; tempoSetMin = 5; tempoSetSec = 0; tempoStage = 0; }
      else if (mainMenuIndex == 3) { menuEstado = MENU_ALARM_SUBMENU; menuOpcion = 0; }
      else if (mainMenuIndex == 4) { linternaActiva = !linternaActiva; }
      else if (mainMenuIndex == 5) { menuEstado = MENU_SELECCION_JUEGO; gameIndex = 0; }
      else if (mainMenuIndex == 6) { menuEstado = MENU_QR; }
    }
    else if (menuEstado == MENU_ALARM_SUBMENU) {
      if (menuOpcion == 1) { menuIndice = 0; menuEstado = MENU_LISTA; }
      else if (numAlarmas < MAX_ALARMAS) {
         isCreatingAlarm = true; wizardFromDate = (menuOpcion == 2);
         if (wizardFromDate) menuEstado = MENU_ALARM_ADD_DATE; else menuEstado = MENU_MODIFICAR;
      }
    }
    else if (menuEstado == MENU_MODIFICAR) { saveAlarmas(); menuEstado = MENU_ALARM_ADD_TONE; wizardTone = 0; previewActivo = false; }
    else if (menuEstado == MENU_ALARM_ADD_TONE) { alarmas[menuIndice].tono = wizardTone; previewActivo = false; noTone(BUZZER_PIN); saveAlarmas(); menuEstado = MENU_ALARM_ADD_REPEAT; }
    else if (menuEstado == MENU_ALARM_ADD_REPEAT) {
      if (wizardRepeat == 0) { alarmas[menuIndice].dias = getDowAbrev(getCurrentDow()); alarmas[menuIndice].esFecha = false; }
      else if (wizardRepeat == 1) { alarmas[menuIndice].dias = "LUN,MAR,MIE,JUE,VIE"; alarmas[menuIndice].esFecha = false; }
      if (wizardRepeat == 2) menuEstado = MENU_ALARM_ADD_CUSTOM;
      else { menuEstado = MENU_ALARM_DELETE_FIRE; menuOpcion = 0; }
    }
    else if (menuEstado == MENU_ALARM_ADD_CUSTOM) {
      if (digitalRead(BUTTON_PIN) == LOW || digitalRead(BUTTON2_PIN) == LOW) { 
        String dStr = ""; const char* d[] = {"LUN","MAR","MIE","JUE","VIE","SAB","DOM"};
        for(int i=0; i<7; i++) if(wizardDays[i]) { if(dStr!="") dStr+=","; dStr+=d[i]; }
        if(dStr=="") dStr="DOM";
        alarmas[menuIndice].dias = dStr; alarmas[menuIndice].esFecha = false;
        menuEstado = MENU_ALARM_DELETE_FIRE; menuOpcion = 0; 
      }
      else wizardDays[menuIndice % 7] = !wizardDays[menuIndice % 7];
    }
    else if (menuEstado == MENU_ALARM_ADD_DATE) { 
      if (wizardDateStage < 4) wizardDateStage++; 
      else { 
        char buf[12]; sprintf(buf, "%04d-%02d-%02d", wizardYear, wizardMonth, wizardDay);
        alarmas[menuIndice].dias = String(buf);
        char hbuf[6]; sprintf(hbuf, "%02d:%02d", wizardHour, wizardMin);
        alarmas[menuIndice].hora = String(hbuf);
        alarmas[menuIndice].esFecha = true;
        if (wizardFromDate) { wizardTone = 0; previewActivo = false; menuEstado = MENU_ALARM_ADD_TONE; }
        else { menuOpcion = 1; menuEstado = MENU_ALARM_DELETE_FIRE; }
      } 
    }
    else if (menuEstado == MENU_ALARM_DELETE_FIRE) { alarmas[menuIndice].deleteOnFire = (menuOpcion == 0); saveAlarmas(); menuEstado = MENU_NORMAL; lastDisplay = 0; notify("Alarma Lista!"); }
    else if (menuEstado == MENU_DISENO_TIPO) { menuEstado = (disenoTipoIndex == 0) ? MENU_DISENO_ANALOGO : MENU_DISENO_DIGITAL; faceMenuIndex = 0; }
    else if (menuEstado == MENU_DISENO_ANALOGO) { analogFace = faceMenuIndex + 1; isAnalog = true; menuEstado = MENU_NORMAL; lastDisplay = 0; }
    else if (menuEstado == MENU_DISENO_DIGITAL) { digitalFace = faceMenuIndex + 1; isAnalog = false; menuEstado = MENU_NORMAL; lastDisplay = 0; }
    else if (menuEstado == MENU_CRONOMETRO) { if (cronoRunning) { cronoElapsed += (millis() - cronoStart); cronoRunning = false; } else { cronoStart = millis(); cronoRunning = true; } }
    else if (menuEstado == MENU_TEMPO_SET) { if (tempoStage < 2) tempoStage++; else { timerActivo = true; timerFin = millis() + (tempoSetHour*3600UL + tempoSetMin*60UL + tempoSetSec)*1000UL; menuEstado = MENU_NORMAL; } }
    else if (menuEstado == MENU_LISTA) { if (numAlarmas > 0) { menuOpcion = 0; menuEstado = MENU_OPCIONES; } }
    else if (menuEstado == MENU_OPCIONES) { if (menuOpcion == 0) { deleteAlarma(alarmas[menuIndice].nombre); menuIndice = 0; menuEstado = MENU_LISTA; } else menuEstado = MENU_MODIFICAR; }
    else if (menuEstado == MENU_SELECCION_JUEGO) {
      if (gameIndex == 0) { menuEstado = MENU_MINIJUEGO; initMinijuego(); }
      else if (gameIndex == 1) { menuEstado = MENU_ASTEROIDS; initAsteroids(); }
      else if (gameIndex == 2) { menuEstado = MENU_RUNNER; initRunner(); }
    }
    else if (menuEstado == MENU_MINIJUEGO) {
      if (gameOver) {
        showHighScores = !showHighScores;
        gameOverDrawn = false; // forzar redibujado al alternar
      } else { birdVelocity = -6.0; tone(BUZZER_PIN, 600, 50); }
    }
    else if (menuEstado == MENU_ASTEROIDS) {
      if (astGameOver) {
        astShowHighScores = !astShowHighScores;
        astOverDrawn = false;
      } else { astShoot(); }
    }
    else if (menuEstado == MENU_RUNNER) {
      if (runGameOver) {
        runShowHighScores = !runShowHighScores;
        runOverDrawn = false;
      } else { runJump(); }
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg = ""; for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  msg.trim();
  
  if (String(topic) == topicSet) {
    if (numAlarmas >= MAX_ALARMAS) { mqttClient.publish(topicStatus, "Error: maximo 10 alarmas"); return; }
    Alarma nueva;
    if (parsearAlarma(msg, nueva)) {
      for (int i = 0; i < numAlarmas; i++) {
        if (alarmas[i].nombre.equalsIgnoreCase(nueva.nombre)) {
          mqttClient.publish(topicStatus, "Error: El nombre ya existe"); notify("Nombre repetido"); return;
        }
      }
      alarmas[numAlarmas++] = nueva; saveAlarmas();
      String ok = String(nueva.esFecha ? "Recordatorio" : "Alarma") + " OK: " + nueva.nombre + " | " + nueva.dias + " | " + nueva.hora + " | Tono: " + String(nueva.tono);
      mqttClient.publish(topicStatus, ok.c_str()); notify("Programada: " + nueva.nombre);
    } else {
      mqttClient.publish(topicStatus, "Error Formato. Usa: NOMBRE|DIAS|HH:MM|TONO (Tono 0-4 opcional)");
    }
  }
  else if (String(topic) == topicStop) { stopAlarm(msg == "SNOOZE"); mqttClient.publish(topicStatus, "Alarma/Timer Detenido"); }
  else if (String(topic) == topicList) { publishList(); menuEstado = MENU_LISTA; menuIndice = 0; forceRedraw = true; wakeUp(); }
  else if (String(topic) == topicDelete) {
    if (deleteAlarma(msg)) mqttClient.publish(topicStatus, ("Eliminada: " + msg).c_str());
    else mqttClient.publish(topicStatus, ("Error: No existe " + msg).c_str());
  }
  else if (String(topic) == topicModify) {
    if (modificarAlarma(msg)) mqttClient.publish(topicStatus, ("Modificada: " + msg).c_str());
    else mqttClient.publish(topicStatus, "Error Formato. Usa: NOMBRE|NUEVOS_DIAS|NUEVA_HORA (usa * para no cambiar)");
  }
  else if (String(topic) == topicTimer) { 
    int mins = msg.toInt(); 
    if (mins > 0 && mins <= 999) { 
      timerFin = millis() + (unsigned long)mins * 60000UL; timerActivo = true; timerSonando = false;
      mqttClient.publish(topicStatus, ("Timer: " + String(mins) + " min").c_str()); notify("Timer: " + String(mins) + " min"); 
    } else {
      mqttClient.publish(topicStatus, "Error Formato. Envia solo los MINUTOS (ej: 15)");
    }
  }
  else if (String(topic) == topicFace) { 
    msg.toUpperCase();
    if (msg.startsWith("ANALOG")) { 
      int f = msg.substring(6).toInt(); if (f >= 1 && f <= 3) { isAnalog = true; analogFace = f; }
    } else if (msg.startsWith("DIGITAL")) { 
      int f = msg.substring(7).toInt(); if (f >= 1 && f <= 3) { isAnalog = false; digitalFace = f; }
    } else {
      mqttClient.publish(topicStatus, "Error Formato. Usa: ANALOG1..3 o DIGITAL1..3"); return;
    }
    forceRedraw = true; lastDisplay = 0; mqttClient.publish(topicStatus, ("Face set to: " + msg).c_str()); 
  }
  else if (String(topic) == topicCrono) {
    msg.toUpperCase(); String report = "";
    if (msg == "START") { if (!cronoRunning) { cronoStart = millis(); cronoRunning = true; report = "Crono INICIO"; } else report = "Crono ya activo"; }
    else if (msg == "PAUSE") { if (cronoRunning) { cronoElapsed += (millis() - cronoStart); cronoRunning = false; report = "Crono PAUSA"; } else report = "Crono ya pausado"; }
    else if (msg == "RESET") { cronoElapsed = 0; cronoRunning = false; report = "Crono RESET"; }
    else if (msg == "GET") { 
      unsigned long t = cronoElapsed + (cronoRunning ? (millis() - cronoStart) : 0);
      int s = (t/1000)%60, m = (t/60000)%60, h = t/3600000;
      char b[16]; sprintf(b, "Tiempo: %02d:%02d:%02d", h, m, s); report = String(b);
    } else { mqttClient.publish(topicStatus, "Error Formato. Usa: START, PAUSE, RESET o GET"); return; }
    notify(report, 2000); mqttClient.publish(topicStatus, report.c_str());
  }
  else if (String(topic) == topicMessage) {
    // Aviso de Mensaje MQTT Remoto
    notify(msg, 5000); // Mostrar por 5 segundos
    mqttClient.publish(topicStatus, "Mensaje mostrado en pantalla");
  }
  else if (String(topic) == topicWeather) {
    if (WiFi.status() != WL_CONNECTED) {
      mqttClient.publish(topicStatus, "WiFi no conectado");
    } else {
      updateWeather();
      forceRedraw = true;
      String statusMsg = "Clima: " + globalTemp + " " + globalWeather;
      String displayMsg = globalTemp + "\n" + globalWeather;
      mqttClient.publish(topicStatus, statusMsg.c_str());
      notify(displayMsg, 3000);
    }
  }
  else if (String(topic) == topicInfo) {
    String info = "IP:" + WiFi.localIP().toString() + " | RSSI:" + String(WiFi.RSSI()) + "dBm | Uptime:" + String(millis()/60000) + "m";
    mqttClient.publish(topicStatus, info.c_str());
  }
}

#endif
