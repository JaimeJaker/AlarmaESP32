#ifndef MELODIES_H
#define MELODIES_H

#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_D3  147
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_G3  196
#define NOTE_A3  220
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_G5  784
#define NOTE_A5  880
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_D6  1175
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_G6  1568
#define NOTE_A6  1760
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_D7  2349
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_G7  3136
#define NOTE_A7  3520
#define NOTE_B7  3951
#define NOTE_C8  4186

// ─────────────────────────────────────────────────────────────────
// 0: Star Wars Main Theme — frase completa ~20s
// ─────────────────────────────────────────────────────────────────
const int m0[] PROGMEM = {
  // Frase 1 — "May the Force..."
  NOTE_D6, NOTE_D6, NOTE_D6,
  NOTE_G6, NOTE_D7,
  NOTE_C7, NOTE_B6, NOTE_A6, NOTE_G7,
  NOTE_D7,
  NOTE_C7, NOTE_B6, NOTE_A6, NOTE_G7,
  NOTE_D7,
  NOTE_C7, NOTE_B6, NOTE_C7, NOTE_A6,
  // Pausa corta
  0,
  // Frase 2 — puente heroico
  NOTE_D6, NOTE_D6,
  NOTE_F6, NOTE_D6, NOTE_C6,
  NOTE_B5, NOTE_A5, NOTE_B5,
  NOTE_E6,
  NOTE_G6, NOTE_D6, NOTE_C6,
  NOTE_B5, NOTE_A5, NOTE_B5,
  NOTE_D6, NOTE_D7,
  NOTE_D7, NOTE_C7
};
const int b0[] PROGMEM = {
  150, 150, 150,
  650, 650,
  170, 170, 170, 650,
  320,
  170, 170, 170, 650,
  320,
  170, 170, 170, 700,
  200,
  170, 170,
  650, 170, 170,
  170, 170, 650,
  650,
  170, 170, 170,
  170, 170, 650,
  170, 320,
  650, 700
};
const int n0 = 40;

// ─────────────────────────────────────────────────────────────────
// 1: Imperial March — marcha completa ~20s
// ─────────────────────────────────────────────────────────────────
const int m1[] PROGMEM = {
  // Estrofa 1
  NOTE_A5, NOTE_A5, NOTE_A5,
  NOTE_F5, NOTE_C6,
  NOTE_A5, NOTE_F5, NOTE_C6, NOTE_A5,
  0,
  // Estrofa 2
  NOTE_E6, NOTE_E6, NOTE_E6,
  NOTE_F6, NOTE_C6,
  NOTE_G5, NOTE_F5, NOTE_C6, NOTE_A5,
  0,
  // Puente descendente
  NOTE_A6, NOTE_A5, NOTE_A5,
  NOTE_A6, NOTE_G6, NOTE_F6, NOTE_E6,
  NOTE_D6, NOTE_E6,
  NOTE_B5, NOTE_E6, NOTE_G6,
  NOTE_A6, NOTE_E6, NOTE_G6, NOTE_A6,
  0,
  // Cierre
  NOTE_A5, NOTE_A5, NOTE_A5,
  NOTE_F5, NOTE_C6,
  NOTE_A5, NOTE_F5, NOTE_C6, NOTE_A5
};
const int b1[] PROGMEM = {
  520, 520, 520,
  380, 130,
  520, 380, 130, 1100,
  120,
  520, 520, 520,
  380, 130,
  520, 380, 130, 1100,
  120,
  380, 130, 130,
  380, 260, 130, 260,
  260, 380,
  130, 380, 260,
  520, 260, 260, 520,
  120,
  520, 520, 520,
  380, 130,
  520, 380, 130, 1100
};
const int n1 = 49;

// ─────────────────────────────────────────────────────────────────
// 2: Alarma Urgente — pitidos escalonados en volumen ~20s
// El buzzer pasivo sube de tono en cada ráfaga para sentirse más fuerte
// ─────────────────────────────────────────────────────────────────
const int m2[] PROGMEM = {
  // Ráfaga 1 — tono bajo
  NOTE_C6, 0, NOTE_C6, 0, NOTE_C6, 0,
  // Ráfaga 2 — sube
  NOTE_E6, 0, NOTE_E6, 0, NOTE_E6, 0,
  // Ráfaga 3 — más alto
  NOTE_G6, 0, NOTE_G6, 0, NOTE_G6, 0,
  // Ráfaga 4 — clímax
  NOTE_C7, 0, NOTE_C7, 0, NOTE_C7, 0,
  // Ráfaga 5 — bajada rápida
  NOTE_G6, 0, NOTE_E6, 0, NOTE_C6, 0,
  // Remate doble
  NOTE_C7, 0, NOTE_C7, 0
};
const int b2[] PROGMEM = {
  220, 120, 220, 120, 220, 600,
  220, 120, 220, 120, 220, 600,
  220, 120, 220, 120, 220, 600,
  300, 100, 300, 100, 300, 700,
  200, 100, 200, 100, 200, 600,
  400, 150, 400, 800
};
const int n2 = 34;

// ─────────────────────────────────────────────────────────────────
// 3: Sirena Dual — oscilación larga con aceleraciones ~20s
// ─────────────────────────────────────────────────────────────────
const int m3[] PROGMEM = {
  // Ciclo lento × 3
  NOTE_G5, NOTE_D7, NOTE_G5, NOTE_D7, NOTE_G5, NOTE_D7,
  // Aceleración × 4
  NOTE_G5, NOTE_D7, NOTE_G5, NOTE_D7,
  NOTE_G5, NOTE_D7, NOTE_G5, NOTE_D7,
  // Ciclo rápido × 6
  NOTE_A5, NOTE_C7, NOTE_A5, NOTE_C7,
  NOTE_A5, NOTE_C7, NOTE_A5, NOTE_C7,
  NOTE_A5, NOTE_C7, NOTE_A5, NOTE_C7,
  // Clímax sostenido
  NOTE_B5, NOTE_E7, NOTE_B5, NOTE_E7,
  NOTE_B5, NOTE_E7
};
const int b3[] PROGMEM = {
  500, 500, 500, 500, 500, 500,
  300, 300, 300, 300,
  300, 300, 300, 300,
  200, 200, 200, 200,
  200, 200, 200, 200,
  200, 200, 200, 200,
  350, 350, 350, 350,
  600, 600
};
const int n3 = 32;

// ─────────────────────────────────────────────────────────────────
// 4: Alarma Extrema — ráfagas cortas con máxima frecuencia ~20s
// Usa las notas más altas del buzzer para máxima penetración auditiva
// ─────────────────────────────────────────────────────────────────
const int m4[] PROGMEM = {
  // Bloque 1: triples rápidos
  NOTE_G7, 0, NOTE_G7, 0, NOTE_G7, 0,
  NOTE_G7, 0, NOTE_G7, 0, NOTE_G7, 0,
  // Bloque 2: dobles con bajada brusca
  NOTE_G7, 0, NOTE_D6, 0,
  NOTE_G7, 0, NOTE_D6, 0,
  NOTE_G7, 0, NOTE_D6, 0,
  // Bloque 3: acelerado total
  NOTE_A7, 0, NOTE_A7, 0, NOTE_A7, 0, NOTE_A7, 0,
  NOTE_A7, 0, NOTE_A7, 0, NOTE_A7, 0, NOTE_A7, 0,
  // Remate largo
  NOTE_C8, 0, NOTE_C8, 0,
  NOTE_C8
};
const int b4[] PROGMEM = {
  120, 80, 120, 80, 120, 500,
  120, 80, 120, 80, 120, 500,
  200, 100, 200, 600,
  200, 100, 200, 600,
  200, 100, 200, 600,
  100, 60, 100, 60, 100, 60, 100, 400,
  100, 60, 100, 60, 100, 60, 100, 400,
  300, 150, 300, 150,
  800
};
const int n4 = 45;

#endif
