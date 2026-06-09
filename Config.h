#ifndef CONFIG_H
#define CONFIG_H

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include <Preferences.h>
#include <time.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include <Adafruit_NeoPixel.h>

#define NEO_PIN 13
#define NEO_NUM 16
extern Adafruit_NeoPixel strip;

// ── Red y MQTT ────────────────────────────────────────
extern WiFiClient wifiClient;
extern PubSubClient mqttClient;
extern WebServer server;

// ── Almacenamiento ─────────────────────
extern Preferences preferences;

// ── Configuración Red ──────────────────────────────────
extern const char* ssid;
extern const char* password;
extern const char* mqttServer;
extern const int   mqttPort;
#define TIMEZONE_OFFSET_HOURS -5

// Tópicos
extern const char* topicSet;
extern const char* topicStop;
extern const char* topicStatus;
extern const char* topicTimer;
extern const char* topicList;
extern const char* topicDelete;
extern const char* topicModify;
extern const char* topicFace;
extern const char* topicCrono;
extern const char* topicMessage;
extern const char* topicInfo;
extern const char* topicWeather;
extern const char* topicWeatherOut;

// ── Clima ─────────────────────────────────────────────
extern String globalTemp;
extern String globalWeather;
extern int globalWeatherCode;
extern int globalHumidity;
extern bool newWeatherData;
const float tempOffset = 0.0;

// ── Estructura Alarma ──────────────────────────────────
#define MAX_ALARMAS 10
struct Alarma {
  String nombre;
  String hora;
  String dias;
  bool   esFecha;
  bool   activa;
  String lastFired;
  int    tono;
  bool   deleteOnFire;
};

extern Alarma alarmas[MAX_ALARMAS];
extern int    numAlarmas;
extern int    alarmaActiva;

// ── Pines ──────────────────────────────────────────────
#define BUZZER_PIN   25
#define BUTTON_PIN   26
#define BUTTON2_PIN  27
#define BUTTON3_PIN  14

// ── Pantalla ──────────────────────────────────────────
#define TFT_MOSI   23
#define TFT_SCLK   18
#define TFT_DC      2
#define TFT_CS      5
#define TFT_RST     4

#define COLOR_BLACK   0x0000
#define COLOR_WHITE   0xFFFF
#define COLOR_RED     0xF800
#define COLOR_GREEN   0x07E0
#define COLOR_BLUE    0x001F
#define COLOR_CYAN    0x07FF
#define COLOR_YELLOW  0xFFE0
#define COLOR_ORANGE  0xFD20
#define COLOR_DARKGREY 0x39E7

extern Adafruit_GC9A01A display;
extern bool forceRedraw;
extern bool isAnalog;

// ── Estados y Menús ────────────────────────────────────
enum MenuEstado {
  MENU_NORMAL, MENU_LISTA, MENU_OPCIONES, MENU_MODIFICAR, MENU_MAIN,
  MENU_DISENO_TIPO, MENU_DISENO_ANALOGO, MENU_DISENO_DIGITAL,
  MENU_CRONOMETRO, MENU_TEMPO_SET, MENU_ALARM_SUBMENU,
  MENU_ALARM_ADD_TONE, MENU_ALARM_ADD_REPEAT, MENU_ALARM_ADD_CUSTOM,
  MENU_ALARM_ADD_DATE, MENU_ALARM_DELETE_FIRE, MENU_SELECCION_JUEGO, MENU_MINIJUEGO, MENU_ASTEROIDS, MENU_RUNNER, MENU_QR
};
extern MenuEstado menuEstado;
extern int menuIndice;
extern int analogFace;
extern int digitalFace;
extern int mainMenuIndex;
extern int disenoTipoIndex;
extern int faceMenuIndex;

// ── Cronómetro y Timer ────────────────────────────────
extern unsigned long cronoStart;
extern unsigned long cronoElapsed;
extern bool cronoRunning;
extern bool timerActivo;
extern bool timerSonando;
extern unsigned long timerFin;
extern int tempoSetHour, tempoSetMin, tempoSetSec, tempoStage;

// ── Botones ───────────────────────────────────────────
extern volatile bool buttonPressed;
extern volatile unsigned long buttonPressStart;
extern volatile bool btn2Pressed;
extern volatile unsigned long btn2PressStart;
extern volatile bool btn3Pressed;
extern volatile unsigned long btn3PressStart;
// Flags inmediatos para control de juegos (disparo al presionar)
extern volatile bool btnImm1, btnImm2, btnImm3;
extern bool linternaActiva;

// ── Notificaciones y Tiempos ──────────────────────────
extern String msgNotificacion;
extern unsigned long timerNotificacion;
extern unsigned long lastInteraction;
extern unsigned long lastMqttRetry;
extern unsigned long lastDisplay;
extern unsigned long lastCheck;
// Handle de la tarea de clima para notificaciones
extern TaskHandle_t weatherTaskHandle;
// Prototipo para solicitar fetch inmediato de clima (desde web/MQTT)
void triggerWeatherFetch();
// Prototipo para reiniciar estado de redibujo del reloj (declarado en Relojes.h)
extern void resetClockDraw();
// ── Wizard y Edición ──────────────────────────────────
extern int wizardTone, wizardRepeat, wizardHour, wizardMin;
extern bool wizardDays[7];
extern int wizardYear, wizardMonth, wizardDay, wizardDateStage;
extern bool wizardFromDate, previewActivo;
extern bool isCreatingAlarm;
extern int alarmStep, menuOpcion;
extern unsigned long lastNeoUpdate;

// NeoPixel para juegos
extern unsigned long gameNeoTimer;
extern uint32_t gameNeoColor;
extern int gameIndex;

#endif
