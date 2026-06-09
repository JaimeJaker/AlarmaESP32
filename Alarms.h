#ifndef ALARMS_H
#define ALARMS_H

#include "Config.h"

String normalizarDias(String dias) {
  dias.toUpperCase();
  dias.replace("LUM", "LUN");
  dias.replace("LUNES",   "LUN");
  dias.replace("MARTES",  "MAR");
  dias.replace("MIERCOLES", "MIE");
  dias.replace("MIÉR",    "MIE");
  dias.replace("JUEVES",  "JUE");
  dias.replace("VIERNES", "VIE");
  dias.replace("SABADO",  "SAB");
  dias.replace("SÁB",     "SAB");
  dias.replace("DOMINGO", "DOM");
  return dias;
}

String getCurrentTime() {
  struct tm t;
  if (!getLocalTime(&t)) return "00:00";
  char buf[6];
  strftime(buf, sizeof(buf), "%H:%M", &t);
  return String(buf);
}

String getCurrentDate() {
  struct tm t;
  if (!getLocalTime(&t)) return "2000-01-01";
  char buf[11];
  strftime(buf, sizeof(buf), "%Y-%m-%d", &t);
  return String(buf);
}

int getCurrentDow() {
  struct tm t;
  if (!getLocalTime(&t)) return 0;
  return t.tm_wday;
}

String getDowAbrev(int dow) {
  const char* a[] = {"DOM","LUN","MAR","MIE","JUE","VIE","SAB"};
  return String(a[dow]);
}

bool alarmaHoyValida(Alarma& a) {
  if (a.esFecha) return getCurrentDate() == a.dias;
  return a.dias.indexOf(getDowAbrev(getCurrentDow())) >= 0;
}

bool parsearAlarma(String msg, Alarma& out) {
  int p1 = msg.indexOf('|');
  if (p1 < 0) return false;
  int p2 = msg.indexOf('|', p1 + 1);
  if (p2 < 0) return false;
  out.nombre    = msg.substring(0, p1);
  out.dias      = normalizarDias(msg.substring(p1 + 1, p2));

  int p3 = msg.indexOf('|', p2 + 1);
  int p4 = (p3 >= 0) ? msg.indexOf('|', p3 + 1) : -1;

  if (p3 >= 0) {
    out.hora = msg.substring(p2 + 1, p3);
    if (p4 >= 0) {
      out.tono = msg.substring(p3 + 1, p4).toInt();
      out.deleteOnFire = (msg.substring(p4 + 1).toInt() == 1);
    } else {
      out.tono = msg.substring(p3 + 1).toInt();
      out.deleteOnFire = false;
    }
  } else {
    out.hora = msg.substring(p2 + 1);
    out.tono = 0;
    out.deleteOnFire = false;
  }
  out.nombre.trim();
  out.hora.trim();
  out.activa    = true;
  out.lastFired = "";
  out.esFecha   = (out.dias.indexOf('-') >= 0);
  return (out.hora.length() == 5 && out.hora[2] == ':');
}

void saveAlarmas() {
  preferences.begin("alarmas", false);
  preferences.putInt("numAlarmas", numAlarmas);
  for (int i = 0; i < numAlarmas; i++) {
    String key = "a" + String(i);
    String data = alarmas[i].nombre + "|" + alarmas[i].hora + "|" + alarmas[i].dias
                + "|" + String(alarmas[i].esFecha ? 1 : 0)
                + "|" + String(alarmas[i].tono)
                + "|" + String(alarmas[i].deleteOnFire ? 1 : 0);
    preferences.putString(key.c_str(), data);
  }
  preferences.end();
}

void loadAlarmas() {
  preferences.begin("alarmas", true);
  numAlarmas = preferences.getInt("numAlarmas", 0);
  for (int i = 0; i < numAlarmas && i < MAX_ALARMAS; i++) {
    String key = "a" + String(i);
    String data = preferences.getString(key.c_str(), "");
    if (data != "") {
      int p1 = data.indexOf('|');
      int p2 = data.indexOf('|', p1 + 1);
      int p3 = data.indexOf('|', p2 + 1);
      alarmas[i].nombre    = data.substring(0, p1);    alarmas[i].nombre.trim();
      alarmas[i].hora      = data.substring(p1 + 1, p2); alarmas[i].hora.trim();
      alarmas[i].dias      = normalizarDias(data.substring(p2 + 1, p3));
      alarmas[i].esFecha   = (data.substring(p3 + 1, data.indexOf('|', p3+1)).toInt() == 1);
      int p4 = data.indexOf('|', p3 + 1);
      int p5 = (p4 >= 0) ? data.indexOf('|', p4 + 1) : -1;
      if (p4 >= 0) alarmas[i].tono = data.substring(p4 + 1, p5 >= 0 ? p5 : data.length()).toInt();
      else alarmas[i].tono = 0;
      if (p5 >= 0) alarmas[i].deleteOnFire = (data.substring(p5 + 1).toInt() == 1);
      else alarmas[i].deleteOnFire = false;
      alarmas[i].activa    = true;
      alarmas[i].lastFired = "";
    }
  }
  preferences.end();
}

#endif
