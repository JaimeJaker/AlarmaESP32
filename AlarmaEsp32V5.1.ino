#include "Config.h"
#include "Melodies.h"
#include "Alarms.h"
#include "Clima.h"
#include "Relojes.h"
#include "Interfaz.h"
#include "Red.h"
#include "Asteroids.h"
#include "Runner.h"
#include "Logica.h"

// ── Definición de Objetos Globales ───────────────────
Adafruit_NeoPixel strip(NEO_NUM, NEO_PIN, NEO_GRB + NEO_KHZ800);
Preferences preferences;
Adafruit_GC9A01A display = Adafruit_GC9A01A(TFT_CS, TFT_DC, TFT_RST);
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
WebServer server(80);
TaskHandle_t weatherTaskHandle = NULL;

// ── Variables Globales ───────────────────────────────
const char* ssid       = "Mesa8";
const char* password   = "123456789";
const char* mqttServer = "broker.emqx.io";
const int   mqttPort   = 1883;

const char* topicSet      = "alarm/set";
const char* topicStop     = "alarm/stop";
const char* topicStatus   = "alarm/status";
const char* topicTimer    = "alarm/timer";
const char* topicList     = "alarm/list";
const char* topicDelete   = "alarm/delete";
const char* topicModify   = "alarm/modify";
const char* topicFace     = "alarm/face";
const char* topicCrono    = "alarm/crono";
const char* topicMessage  = "alarm/message";
const char* topicInfo     = "alarm/info";
const char* topicWeather  = "alarm/weather";
const char* topicWeatherOut = "alarm/weather_data";

String globalTemp       = "--";
String globalWeather    = "Sincronizando...";
int globalWeatherCode   = -1;
bool newWeatherData     = false;
int globalHumidity      = -1; // porcentaje

Alarma alarmas[MAX_ALARMAS];
int    numAlarmas   = 0;
int    alarmaActiva = -1;
bool   linternaActiva = false;
unsigned long lastNeoUpdate = 0;
unsigned long gameNeoTimer = 0;
uint32_t gameNeoColor = 0;
uint32_t alarmNeoColor = 0xFF0000;
int gameIndex = 0;

MenuEstado menuEstado = MENU_NORMAL;
int        menuIndice = 0;

bool forceRedraw = true;
bool isAnalog = false;
int analogFace = 1;
int digitalFace = 1;
int mainMenuIndex = 0;
int disenoTipoIndex = 0;
int faceMenuIndex = 0;

WiFiManager wifiManager;
bool shouldSaveConfig = false;

void saveConfigCallback() {
  shouldSaveConfig = true;
  notify("Guardando WiFi...");
}

void configModeCallback(WiFiManager *myWiFiManager) {
  notify("AP WiFi listo: ESP32_Alarma_V5");
}

unsigned long cronoStart = 0;
unsigned long cronoElapsed = 0;
bool cronoRunning = false;
bool timerActivo = false;
bool timerSonando = false;
unsigned long timerFin = 0;
int tempoSetHour = 0, tempoSetMin = 5, tempoSetSec = 0, tempoStage = 0;

// Sistema de botones: disparo al SOLTAR para menús, INMEDIATO para juegos
volatile bool buttonPressed = false, btn2Pressed = false, btn3Pressed = false;
volatile bool btnImm1 = false, btnImm2 = false, btnImm3 = false;
volatile unsigned long buttonPressStart = 0, btn2PressStart = 0, btn3PressStart = 0;
// Timestamps del último disparo inmediato (anti-rebote en FALLING)
volatile unsigned long btnImm1Last = 0, btnImm2Last = 0, btnImm3Last = 0;
bool vBtn1 = false, vBtn2 = false, vBtn3 = false; // Botones virtuales para Web Panel

#define BTN_DEBOUNCE_MIN 40
#define BTN_DEBOUNCE_MAX 3000
#define BTN_GAME_DEBOUNCE 150   // ms entre disparos inmediatos consecutivos

volatile unsigned long btn1HighTime = 0, btn2HighTime = 0, btn3HighTime = 0;

String msgNotificacion = "";
unsigned long timerNotificacion = 0;
unsigned long lastInteraction = 0;
unsigned long lastMqttRetry = 0;
unsigned long lastCheck = 0;
unsigned long lastDisplay = 0;

int wizardTone = 0, wizardRepeat = 0, wizardHour = 7, wizardMin = 0;
bool wizardDays[7] = {true, true, true, true, true, true, true};
int wizardYear = 2026, wizardMonth = 4, wizardDay = 30, wizardDateStage = 0;
bool wizardFromDate = false, previewActivo = false;
bool isCreatingAlarm = false;
int alarmStep = 5, menuOpcion = 0;

// ISR B1: inmediato (juegos) + al soltar (menús)
void IRAM_ATTR onButton() {
  unsigned long m = millis();
  if (digitalRead(BUTTON_PIN) == LOW) {           // Flanco de bajada → presionar
    // Ignorar si el rebote mecánico ocurre al soltar (debe estar HIGH > 50ms)
    if (m - buttonPressStart > BTN_GAME_DEBOUNCE && m - btn1HighTime > 50) btnImm1 = true;
    buttonPressStart = m;
  } else {                                         // Flanco de subida → soltar
    btn1HighTime = m;
    unsigned long dur = m - buttonPressStart;
    if (dur >= BTN_DEBOUNCE_MIN && dur <= BTN_DEBOUNCE_MAX)
      buttonPressed = true;  // para menús (opcional)
  }
}
// ISR B2
void IRAM_ATTR onButton2() {
  unsigned long m = millis();
  if (digitalRead(BUTTON2_PIN) == LOW) {
    if (m - btn2PressStart > BTN_GAME_DEBOUNCE && m - btn2HighTime > 50) btnImm2 = true;
    btn2PressStart = m;
  } else {
    btn2HighTime = m;
    unsigned long dur = m - btn2PressStart;
    if (dur >= BTN_DEBOUNCE_MIN && dur <= BTN_DEBOUNCE_MAX)
      btn2Pressed = true;
  }
}
// ISR B3
void IRAM_ATTR onButton3() {
  unsigned long m = millis();
  if (digitalRead(BUTTON3_PIN) == LOW) {
    if (m - btn3PressStart > BTN_GAME_DEBOUNCE && m - btn3HighTime > 50) btnImm3 = true;
    btn3PressStart = m;
  } else {
    btn3HighTime = m;
    unsigned long dur = m - btn3PressStart;
    if (dur >= BTN_DEBOUNCE_MIN && dur <= BTN_DEBOUNCE_MAX)
      btn3Pressed = true;
  }
}

// ── Setup ──────────────────────────────────────────────
void setup() {
  // Inicializar puerto serial para debug
  Serial.begin(115200);
  delay(500); // Esperar a que se estabilice
  Serial.println("\n\n╔════════════════════════════════════════╗");
  Serial.println("║   ALARMA ESP32 v5.1 - INICIANDO       ║");
  Serial.println("╚════════════════════════════════════════╝\n");
  
  randomSeed(micros());
  pinMode(BUZZER_PIN, OUTPUT); digitalWrite(BUZZER_PIN, LOW);
  pinMode(BUTTON_PIN, INPUT_PULLUP); pinMode(BUTTON2_PIN, INPUT_PULLUP); pinMode(BUTTON3_PIN, INPUT_PULLUP);
  // Botones: CHANGE para detectar presión Y liberación
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN),  onButton,  CHANGE);
  attachInterrupt(digitalPinToInterrupt(BUTTON2_PIN), onButton2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BUTTON3_PIN), onButton3, CHANGE);
  // Nota: "// ── Interrupciones" queda como comentario de sección

  strip.begin(); strip.show(); strip.setBrightness(255);
  display.begin();

  WiFi.disconnect(true, true);
  WiFi.persistent(false);
  delay(100);

  wifiManager.resetSettings();

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP("ESP32_Alarma_V5.1");
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.setConfigPortalTimeout(0);
  wifiManager.setDebugOutput(false);

  notify("Intentando WiFi guardado...");
  bool connected = wifiManager.autoConnect("ESP32_Alarma_V5.1");
  if (connected) {
    String ssid = WiFi.SSID();
    notify("WiFi conectada:\n" + ssid);
    Serial.printf("[WIFI] Conectado a %s\n", ssid.c_str());
  } else {
    notify("WiFi no conectada.\nUsa AP para configurar.");
    Serial.println("[WIFI] No se pudo conectar con WiFi guardado");
  }
  WiFi.setSleep(false);

  loadAlarmas();
  configTime(TIMEZONE_OFFSET_HOURS * 3600, 0, "pool.ntp.org", "time.google.com");
  // Mostrar splash hasta 10s o hasta que el reloj esté sincronizado/visible
  drawSplashScreen();
  setupWebServer();
  mqttClient.setServer(mqttServer, mqttPort); mqttClient.setCallback(mqttCallback);
  connectMQTT();
  // Crear la tarea de clima y guardar su handle para notificaciones
  xTaskCreate(weatherTask, "ClimaTask", 8192, NULL, 1, &weatherTaskHandle);
  lastInteraction = millis();
  
  Serial.println("[SISTEMA] ✅ Inicialización completada correctamente");
  Serial.println("[SISTEMA] 🎯 Tarea de clima creada - Esperando sincronización...\n");
}

// ── Loop Principal ─────────────────────────────────────
void loop() {
  procesarBotones();
  server.handleClient();
  if (!mqttClient.connected()) connectMQTT();
  mqttClient.loop();

  // Timeout Menú: 60s para QR, 30s para el resto
  unsigned long menuTimeout = (menuEstado == MENU_QR) ? 60000 : 30000;
  if (menuEstado != MENU_NORMAL && (millis() - lastInteraction > menuTimeout)) {
    if (menuEstado == MENU_CRONOMETRO && cronoRunning) {
      lastInteraction = millis();
    } else if ((menuEstado == MENU_MINIJUEGO && !gameOver) || (menuEstado == MENU_ASTEROIDS && !astGameOver) || (menuEstado == MENU_RUNNER && !runGameOver)) {
      lastInteraction = millis();
    } else {
      menuEstado = MENU_NORMAL;
      forceRedraw = true;
      resetClockDraw();
    }
  }

  // Lógica de Sonido / Alarma Activa
  if (alarmaActiva >= 0 || previewActivo) {
    static unsigned long lastNote = 0;
    static int noteIdx = 0;
    static int lastTIdx = -1;
    int tIdx = (alarmaActiva >= 0) ? alarmas[alarmaActiva].tono : wizardTone;
    
    if (tIdx != lastTIdx || (previewActivo && noteIdx == 0 && lastNote == 0)) { 
      noteIdx = 0; lastTIdx = tIdx; lastNote = 0; 
    }

    const int* melody;
    const int* beats;
    int n;

    switch(tIdx) {
      case 1: melody = m1; beats = b1; n = n1; break;
      case 2: melody = m2; beats = b2; n = n2; break;
      case 3: melody = m3; beats = b3; n = n3; break;
      case 4: melody = m4; beats = b4; n = n4; break;
      default: melody = m0; beats = b0; n = n0; break;
    }

    if (millis() - lastNote > (unsigned long)(pgm_read_word(&beats[noteIdx]) + 40)) {
      int note = pgm_read_word(&melody[noteIdx]);
      if (note > 0) tone(BUZZER_PIN, note, pgm_read_word(&beats[noteIdx]));
      else noTone(BUZZER_PIN);
      lastNote = millis();
      noteIdx++;
      if (noteIdx >= n) {
        noteIdx = 0;
        if (previewActivo) {
          previewActivo = false;
          noTone(BUZZER_PIN);
        }
      }
    }
    
    if (alarmaActiva >= 0) {
      drawAlarmAnimation(alarmas[alarmaActiva].nombre, alarmas[alarmaActiva].hora);
    }
  } else if (timerSonando) {
    static unsigned long lastTimerNote = 0;
    static bool timerToggle = false;
    if (millis() - lastTimerNote > 400) {
      lastTimerNote = millis();
      timerToggle = !timerToggle;
      if (timerToggle) tone(BUZZER_PIN, 2500, 200);
      else noTone(BUZZER_PIN);
    }
    drawTimerAnimation();
  } else {
    if (!previewActivo) noTone(BUZZER_PIN);

    // Prioridad: Si hay notificación activa, mostrar ventana de aviso (igual que V4)
    if (millis() < timerNotificacion && timerNotificacion != 0 && msgNotificacion != "") {
      if (forceRedraw) {
        forceRedraw = false;
        drawGlobalNotification(true);
      }
    } else {
      // DIBUJADO DINÁMICO (Reloj, Crono)
      unsigned long cronoInterval = cronoRunning ? 80 : 500;
      if (millis() - lastDisplay > (menuEstado == MENU_CRONOMETRO ? cronoInterval : 250)) {
        lastDisplay = millis();
        if (menuEstado == MENU_NORMAL) drawNormal();
        else if (menuEstado == MENU_CRONOMETRO) {
          unsigned long t = cronoElapsed + (cronoRunning ? (millis() - cronoStart) : 0);
          int ms = (t % 1000) / 10, s = (t / 1000) % 60, m = (t / 60000) % 60, h = (t / 3600000);
          char buf[16];
          if (h > 0) sprintf(buf, "%02d:%02d:%02d", h, m, s); else sprintf(buf, "%02d:%02d.%02d", m, s, ms);
          // Borrar área de tiempo
          display.fillRect(25, 90, 190, 32, COLOR_BLACK);
          display.setTextSize(3); display.setTextColor(COLOR_CYAN);
          display.setCursor(120 - (8*18)/2, 95); display.print(buf);
          // Borrar y redibujar estado CORRIENDO/PAUSADO
          display.fillRect(30, 133, 150, 16, COLOR_BLACK);
          display.setTextSize(1);
          display.setTextColor(cronoRunning ? COLOR_GREEN : COLOR_YELLOW);
          display.setCursor(120 - (9*6)/2, 140);
          display.print(cronoRunning ? "CORRIENDO" : "PAUSADO  ");
        }
      }

      // DIBUJADO ESTÁTICO (Solo cuando cambia algo)
      static MenuEstado lastState = MENU_NORMAL;
      if (forceRedraw) {
        if (menuEstado != lastState) {
          animateTransition();
          lastState = menuEstado;
        }
        forceRedraw = false;
        // MENU_NORMAL: limpiar pantalla y resetear estado del reloj
        if (menuEstado == MENU_NORMAL) {
          display.fillScreen(COLOR_BLACK);
          resetClockDraw(); // Fuerza redibujo completo del fondo del reloj
        }
        else if (menuEstado == MENU_MAIN) drawMainMenu();
        else if (menuEstado == MENU_CRONOMETRO) drawCronometro();
        else if (menuEstado == MENU_DISENO_TIPO) drawDisenoTipo();
        else if (menuEstado == MENU_DISENO_ANALOGO) drawFaceMenu("Fondo de Pantalla Analogo");
        else if (menuEstado == MENU_DISENO_DIGITAL) drawFaceMenu("Fondo de Pantalla Digital");
        else if (menuEstado == MENU_ALARM_SUBMENU) drawAlarmSubmenu();
        else if (menuEstado == MENU_LISTA) drawListaAlarmas(menuIndice);
        else if (menuEstado == MENU_TEMPO_SET) drawTempoSet();
        else if (menuEstado == MENU_MODIFICAR) drawModificar(menuIndice, isCreatingAlarm);
        else if (menuEstado == MENU_OPCIONES) drawOpciones(menuIndice);
        else if (menuEstado == MENU_ALARM_ADD_TONE) drawAlarmAddTone(wizardTone, previewActivo);
        else if (menuEstado == MENU_ALARM_ADD_REPEAT) drawAlarmAddRepeat(wizardRepeat);
        else if (menuEstado == MENU_ALARM_ADD_CUSTOM) drawAlarmAddCustom(wizardDays);
        else if (menuEstado == MENU_ALARM_ADD_DATE) drawAlarmAddDate(wizardYear, wizardMonth, wizardDay, wizardHour, wizardMin, wizardDateStage);
        else if (menuEstado == MENU_ALARM_DELETE_FIRE) drawAlarmDeleteOnFire(menuOpcion);
        else if (menuEstado == MENU_SELECCION_JUEGO) drawSeleccionJuego();
        else if (menuEstado == MENU_QR) drawQR();
      }
      
      if (menuEstado == MENU_MINIJUEGO) {
        updateMinijuego();
        if (forceRedraw) { drawMinijuego(); forceRedraw = false; }
      } else if (menuEstado == MENU_ASTEROIDS) {
        updateAsteroids(digitalRead(BUTTON_PIN) == LOW || vBtn1, digitalRead(BUTTON2_PIN) == LOW || vBtn2);
        if (forceRedraw) { drawAsteroids(); forceRedraw = false; }
      } else if (menuEstado == MENU_RUNNER) {
        updateRunner();
        if (forceRedraw) { drawRunner(); forceRedraw = false; }
      }
    }
  }

  // Lógica de Tiempos y Alarmas
  String now = getCurrentTime();
  if (millis() - lastCheck > 1000) {
    lastCheck = millis();
    // Resetear lastFired si el minuto ya pasó (igual que V4)
    for (int i = 0; i < numAlarmas; i++) {
      if (alarmas[i].lastFired != "" && now != alarmas[i].lastFired)
        alarmas[i].lastFired = "";
    }
    for (int i = 0; i < numAlarmas && alarmaActiva < 0; i++) {
      if (alarmas[i].hora == now && alarmas[i].lastFired != now && alarmaHoyValida(alarmas[i])) {
        alarmaActiva = i; alarmas[i].lastFired = now; menuEstado = MENU_NORMAL; forceRedraw = true; wakeUp();
      }
    }
    if (timerActivo && millis() >= timerFin) { timerActivo = false; timerSonando = true; menuEstado = MENU_NORMAL; forceRedraw = true; wakeUp(); }
  }

  // Limpiar notificación expirada globalmente
  if (millis() >= timerNotificacion && timerNotificacion != 0 && msgNotificacion != "") {
    msgNotificacion = "";
    timerNotificacion = 0;
    forceRedraw = true;
  }

  updateNeoPixels();
}

void updateNeoPixels() {
  static unsigned long lastTick = 0;
  if (millis() - lastNeoUpdate < 30) return; // 33 FPS aprox
  lastNeoUpdate = millis();

  // 1. PRIORIDAD: Alarma o Timer Sonando
  if (alarmaActiva >= 0 || timerSonando) {
    if (timerSonando) {
      // Arcoiris para Timer
      static uint16_t j = 0;
      for(int i=0; i<strip.numPixels(); i++)
        strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
      j += 5;
    } else {
      // Efecto COMETA dinámico para alarma
      static int cometPos = 0;
      static unsigned long lastStep = 0;
      static bool headFlash = false;
      if (millis() - lastStep > 55) {
        lastStep = millis();
        cometPos = (cometPos + 1) % strip.numPixels();
        headFlash = !headFlash;
      }
      strip.clear();
      // Cola del cometa con degradado
      for (int i = 0; i < 8; i++) {
        int pixIdx = (cometPos - i + strip.numPixels()) % strip.numPixels();
        uint8_t bright = (uint8_t)(255 - i * 30);
        uint8_t r = (uint8_t)(((alarmNeoColor >> 16) & 0xFF) * bright / 255);
        uint8_t g = (uint8_t)(((alarmNeoColor >>  8) & 0xFF) * bright / 255);
        uint8_t b = (uint8_t)(( alarmNeoColor        & 0xFF) * bright / 255);
        strip.setPixelColor(pixIdx, strip.Color(r, g, b));
      }
      // Cabeza: destello blanco intermitente para mayor urgencia
      if (headFlash) strip.setPixelColor(cometPos, strip.Color(255, 255, 255));
    }
    strip.show();
    return;
  }

  // 2. Linterna (Modo Manual)
  if (linternaActiva) {
    strip.fill(strip.Color(255, 255, 255));
    strip.show();
    return;
  }

  // 2.5 Eventos Minijuegos
  if (millis() < gameNeoTimer) {
    strip.fill(gameNeoColor);
    strip.show();
    return;
  }

  // 3. Error de WiFi (Parpadeo Rojo Lento)
  if (WiFi.status() != WL_CONNECTED) {
    static bool blink = false;
    static unsigned long lastBlink = 0;
    if (millis() - lastBlink > 500) {
      lastBlink = millis();
      blink = !blink;
    }
    if (blink) strip.fill(strip.Color(150, 0, 0));
    else strip.clear();
    strip.show();
    return;
  }

  // 4. Sincronización de Clima (Respiración Azul)
  if (globalWeather == "Sincronizando...") {
    static int val = 0;
    static int dir = 1;
    strip.fill(strip.Color(0, 0, val));
    strip.show();
    val += dir * 5;
    if (val >= 150 || val <= 0) dir *= -1;
    return;
  }

  // 5. Estado Normal (Apagado o muy tenue)
  strip.clear();
  strip.show();
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
