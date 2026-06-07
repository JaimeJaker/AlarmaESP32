#ifndef CLIMA_H
#define CLIMA_H

#include "Config.h"

// ─────────────────────────────────────────────
//  API KEYS
// ─────────────────────────────────────────────
#define OWM_API_KEY  "4e0a88d3c98722ac4cedd2e7bd868a0b"
//  Fuente 2 (wttr.in) no requiere key

// ─────────────────────────────────────────────
//  COORDENADAS (Bogotá)
// ─────────────────────────────────────────────
#define LAT  "4.638"
#define LON  "-74.084"

// ─────────────────────────────────────────────
//  ALMACENAMIENTO DE FUENTES
//  [0] = Open-Meteo  [1] = OpenWeatherMap  [2] = Met.no
// ─────────────────────────────────────────────
static float  tempSources[3]    = {0, 0, 0};
static int    weatherCodes[3]   = {-1, -1, -1};  // código WMO normalizado
static int    humiditySources[3] = {-1, -1, -1}; // humedad % por fuente (-1 = no disponible)
static bool   sourceValid[3]    = {false, false, false};

volatile bool weatherFetchRequested = false;


// ═════════════════════════════════════════════
//  MAPEOS DE CONDICIÓN CLIMÁTICA
//  Todos los códigos se normalizan a 14 categorías internas:
//  0=DESPEJADO  1=POCO NUBOSO  2=PARCIALMENTE NUBLADO  3=CUBIERTO
//  4=NIEBLA / NEBLINA  5=LLOVIZNA  6=LLUVIA  7=CHUBASCO
//  8=TORMENTA ELÉCTRICA  9=NIEVE  10=GRANIZO  11=VIENTO FUERTE
//  12=LLUVIA CON NIEBLA  13=TORMENTA CON GRANIZO
// ═════════════════════════════════════════════

// Open-Meteo WMO → categoría interna
int owmWMOtoCategory(int code) {
  if (code == 0)                          return 0; // despejado
  if (code == 1)                          return 1; // poco nuboso
  if (code == 2)                          return 2; // parcialmente nublado
  if (code == 3)                          return 3; // cubierto
  if (code == 45 || code == 48)           return 4; // niebla / neblina
  if (code >= 51 && code <= 57)           return 5; // llovizna
  if ((code >= 61 && code <= 65) || (code >= 66 && code <= 67)) return 6; // lluvia
  if (code >= 71 && code <= 77)           return 9; // nieve
  if (code >= 80 && code <= 82)           return 7; // chubasco
  if (code == 95)                         return 8; // tormenta electrica
  if (code == 96 || code == 99)           return 13; // tormenta con granizo
  return 2;
}

// OpenWeatherMap ID → categoría interna
int owmIDtoCategory(int id) {
  if (id >= 200 && id < 300) {
    if (id == 906 || id == 962) return 13; // tormenta con granizo
    return 8; // tormenta electrica
  }
  if (id >= 300 && id < 400)              return 5; // llovizna
  if (id >= 500 && id < 600) {
    if (id >= 520 && id < 600)            return 7; // chubasco / lluvia fuerte
    return 6; // lluvia
  }
  if (id >= 600 && id < 700)              return 9; // nieve
  if (id >= 700 && id < 800) {
    if (id == 771 || id == 781)           return 11; // viento fuerte / tornado
    return 4; // niebla / neblina
  }
  if (id == 800)                          return 0; // despejado
  if (id == 801)                          return 1; // poco nuboso
  if (id == 802)                          return 2; // parcialmente nublado
  if (id == 803 || id == 804)             return 3; // cubierto
  if (id == 906)                          return 10; // granizo
  if (id >= 951 && id <= 957)             return 11; // viento fuerte
  if (id == 960 || id == 961)             return 8; // tormenta electrica fuerte
  return 2;
}

// Met.no utiliza códigos WMO, reutilizamos la función de Open-Meteo
// (Los códigos WMO de Met.no son idénticos a los de Open-Meteo)

// Categoría interna → texto display
String categoryToString(int cat) {
  switch (cat) {
    case 0: return "DESPEJADO";
    case 1: return "POCO NUBOSO";
      case 2: return "NUBLADO";
      case 3: return "CUBIERTO";
      case 4: return "NEBLINA";
    case 5: return "LLOVIZNA";
    case 6: return "LLUVIA";
    case 7: return "CHUBASCO";
    case 8: return "TORMENTA ELÉCTRICA";
    case 9: return "NIEVE";
    case 10: return "GRANIZO";
    case 11: return "VIENTO FUERTE";
    case 12: return "LLUVIA CON NIEBLA";
    case 13: return "TORMENTA CON GRANIZO";
    default: return "DESCONOCIDO";
  }
}


// ═════════════════════════════════════════════
//  FUSIÓN DE DATOS: promedio temp + votación clima
// ═════════════════════════════════════════════
void fuseWeatherData() {
  Serial.println("\n" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "");
  Serial.println("[CLIMA] ✦ SINCRONIZACIÓN COMPLETADA");
  Serial.println("=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "");
  
  // --- Temperatura: promedio de fuentes válidas ---
  float sum = 0;
  int   count = 0;
  for (int i = 0; i < 3; i++) {
    if (sourceValid[i]) {
      sum += tempSources[i];
      count++;
    }
  }
  if (count == 0) {
    Serial.println("[CLIMA] ⚠ ADVERTENCIA: Ninguna fuente respondió");
    return;  // ninguna fuente respondió, no actualizar
  }

  float avgTemp = sum / count;

  // --- Clima: votación por mayoría (14 categorías posibles) ---
  int votes[14] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int validCodes = 0;
  
  Serial.println("[CLIMA] 🗳️  DESGLOSE DE VOTOS:");
  const char* sourceNames[3] = {"Open-Meteo", "OpenWeatherMap", "Met.no"};
  
  for (int i = 0; i < 3; i++) {
    if (sourceValid[i] && weatherCodes[i] >= 0) {
      votes[weatherCodes[i]]++;
      validCodes++;
      if (humiditySources[i] >= 0) {
        Serial.printf("  • %s -> %s (codigo %d) | Humedad: %d%%\n", sourceNames[i], categoryToString(weatherCodes[i]).c_str(), weatherCodes[i], humiditySources[i]);
      } else {
        Serial.printf("  • %s -> %s (codigo %d)\n", sourceNames[i], categoryToString(weatherCodes[i]).c_str(), weatherCodes[i]);
      }
    }
  }

  int winnerCat = 2; // fallback: nublado
  int maxVotes  = 0;
  for (int cat = 0; cat < 14; cat++) {
    if (votes[cat] > maxVotes) {
      maxVotes  = votes[cat];
      winnerCat = cat;
    }
  }

  int chosenSource = -1;
  if (maxVotes >= 2) {
    // Al menos dos fuentes coinciden en la misma categoría
    Serial.printf("[CLIMA] ✅ Consenso alcanzado: %s con %d votos\n", categoryToString(winnerCat).c_str(), maxVotes);
  } else {
    // Todas las fuentes difieren: preferir OpenWeatherMap si está disponible,
    // si no, usar la primera fuente válida disponible
    if (sourceValid[1] && weatherCodes[1] >= 0) {
      chosenSource = 1; // OpenWeatherMap
    } else {
      for (int i = 0; i < 3; i++) {
        if (sourceValid[i] && weatherCodes[i] >= 0) {
          chosenSource = i;
          break;
        }
      }
    }
    if (chosenSource >= 0) {
      winnerCat = weatherCodes[chosenSource];
      Serial.printf("[CLIMA] ⚠ Sin consenso: usando %s\n", sourceNames[chosenSource]);
    }
  }

  // Solo actualizar globals si al menos 1 fuente es válida
  globalTemp    = String((int)round(avgTemp)) + "C";
  globalWeather = categoryToString(winnerCat);
  // Actualizar código de clima global para sincronizar iconos y fondo
  globalWeatherCode = winnerCat;
  newWeatherData = true;
  // Forzar redibujado de pantalla para que fondo, icono y texto se actualicen
  forceRedraw = true;
  // Reiniciar variables del reloj para forzar re-inicialización del fondo meteorológico
  resetClockDraw();

  // --- Humedad: promedio de fuentes que reportaron valor válido ---
  int humSum = 0; int humCount = 0;
  for (int i = 0; i < 3; i++) {
    if (sourceValid[i] && humiditySources[i] >= 0) {
      humSum += humiditySources[i];
      humCount++;
    }
  }
  if (humCount > 0) {
    int avgHum = (humSum + humCount/2) / humCount; // redondeo
    globalHumidity = avgHum;
    Serial.printf("[CLIMA] 💧 Humedad promedio: %d%% (basado en %d fuentes)\n", avgHum, humCount);
  } else {
    globalHumidity = -1;
    Serial.println("[CLIMA] 💧 Humedad no disponible");
  }

  Serial.printf("[CLIMA] 📡 Fuentes válidas: %d/3\n", count);
  Serial.printf("[CLIMA] 🌡️  Temperatura promedio: %.1f°C\n", avgTemp);
  Serial.printf("[CLIMA] 🎨 Resultado aplicado: %s (código: %d)\n", globalWeather.c_str(), winnerCat);
  Serial.println("[CLIMA] ✅ DATOS ACTUALIZADOS Y SINCRONIZADOS");
  Serial.println("");
}


// ═════════════════════════════════════════════
//  FUENTE 0 — OPEN-METEO (sin key)
// ═════════════════════════════════════════════
void fetchOpenMeteo() {
  HTTPClient http;
  String url = "http://api.open-meteo.com/v1/forecast"
               "?latitude=" LAT "&longitude=" LON "&current_weather=true&hourly=relativehumidity_2m";
  http.setTimeout(20000);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  http.begin(url);
  int code = http.GET();
  if (code > 0) {
    DynamicJsonDocument doc(1500);
    if (deserializeJson(doc, http.getString()) == DeserializationError::Ok) {
      if (doc.containsKey("current_weather")) {
        float t = (float)doc["current_weather"]["temperature"] + tempOffset;
        int   wc = doc["current_weather"]["weathercode"];
        tempSources[0]  = t;
        weatherCodes[0] = owmWMOtoCategory(wc);
        // Intentar leer humedad desde array hourly.relativehumidity_2m si está disponible
        int hum0 = -1;
        if (doc.containsKey("hourly") && doc["hourly"].containsKey("relativehumidity_2m")) {
          JsonArray rh = doc["hourly"]["relativehumidity_2m"].as<JsonArray>();
          if (rh.size() > 0) hum0 = (int)rh[rh.size()-1];
        }
        humiditySources[0] = hum0;
        sourceValid[0]  = true;
        if (hum0 >= 0) {
          Serial.printf("  ✅ Open-Meteo: %.1f°C | Código WMO: %d | Categoría: %d | Humedad: %d%%\n", t, wc, weatherCodes[0], hum0);
        } else {
          Serial.printf("  ✅ Open-Meteo: %.1f°C | Código WMO: %d | Categoría: %d | Humedad: N/D\n", t, wc, weatherCodes[0]);
        }
      }
    } else {
      sourceValid[0] = false;
      Serial.println("  ❌ Open-Meteo: Error al parsear JSON");
    }
  } else {
    sourceValid[0] = false;
    Serial.printf("  ❌ Open-Meteo: Error HTTP %d\n", code);
  }
  http.end();
}


// ═════════════════════════════════════════════
//  FUENTE 1 — OPENWEATHERMAP (requiere key)
// ═════════════════════════════════════════════
void fetchOpenWeatherMap() {
  if (String(OWM_API_KEY) == "TU_KEY_OPENWEATHERMAP") {
    Serial.println("  ⏭️  OpenWeatherMap: API Key no configurada, omitido");
    sourceValid[1] = false;
    return;
  }
  HTTPClient http;
  String url = "http://api.openweathermap.org/data/2.5/weather"
               "?lat=" LAT "&lon=" LON "&units=metric&appid=" OWM_API_KEY;
  http.setTimeout(20000);
  http.begin(url);
  int code = http.GET();
  if (code > 0) {
    DynamicJsonDocument doc(2048);
    if (deserializeJson(doc, http.getString()) == DeserializationError::Ok) {
      if (doc.containsKey("main") && doc.containsKey("weather")) {
        float t  = (float)doc["main"]["temp"] + tempOffset;
        int   id = doc["weather"][0]["id"];
        tempSources[1]  = t;
        weatherCodes[1] = owmIDtoCategory(id);
        // Humedad (OpenWeatherMap)
        int hum1 = -1;
        if (doc.containsKey("main") && doc["main"].containsKey("humidity")) {
          hum1 = (int)doc["main"]["humidity"];
        }
        humiditySources[1] = hum1;
        sourceValid[1]  = true;
        if (hum1 >= 0) {
          Serial.printf("  ✅ OpenWeatherMap: %.1f°C | ID: %d | Categoría: %d | Humedad: %d%%\n", t, id, weatherCodes[1], hum1);
        } else {
          Serial.printf("  ✅ OpenWeatherMap: %.1f°C | ID: %d | Categoría: %d | Humedad: N/D\n", t, id, weatherCodes[1]);
        }
      }
    } else {
      sourceValid[1] = false;
      Serial.println("  ❌ OpenWeatherMap: Error al parsear JSON");
    }
  } else {
    sourceValid[1] = false;
    Serial.printf("  ❌ OpenWeatherMap: Error HTTP %d\n", code);
  }
  http.end();
}


// ═════════════════════════════════════════════
//  FUENTE 2 — MET.NO (sin key, sin registro)
//  Responde JSON con temperatura y símbolo de condición
// ═════════════════════════════════════════════
void fetchMetno() {
  HTTPClient http;
  // Met.no API utiliza https y requiere User-Agent obligatoriamente
  String url = "https://api.met.no/weatherapi/locationforecast/2.0/compact"
               "?lat=" LAT "&lon=" LON;
  http.setTimeout(20000);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  http.begin(url);
  http.addHeader("User-Agent", "ESP32WeatherStation/1.0");
  int code = http.GET();
  if (code > 0) {
    String jsonResponse = http.getString();
    DynamicJsonDocument doc(4096);  // Aumentar buffer para Met.no
    
    DeserializationError error = deserializeJson(doc, jsonResponse);
    if (error == DeserializationError::Ok) {
      if (doc.containsKey("properties")) {
        JsonObject properties = doc["properties"];
        if (properties.containsKey("timeseries") && properties["timeseries"].size() > 0) {
          JsonObject timeseries0 = properties["timeseries"][0];
          if (timeseries0.containsKey("data")) {
            JsonObject data = timeseries0["data"];
            
            // Leer temperatura desde instant.details.air_temperature
            float t = 0;
            if (data["instant"]["details"].containsKey("air_temperature")) {
              t = (float)data["instant"]["details"]["air_temperature"] + tempOffset;
            }
            
            // Leer símbolo de condición desde next_1_hours.summary.symbol_code
            String symbolCode = "unknown";
            if (data.containsKey("next_1_hours") && data["next_1_hours"].containsKey("summary")) {
              if (data["next_1_hours"]["summary"].containsKey("symbol_code")) {
                symbolCode = (const char*)data["next_1_hours"]["summary"]["symbol_code"];
              }
            }
            
            // Mapeo de symbol_code de Met.no a categoría interna
            int category = 2; // fallback: nublado
            if (symbolCode.indexOf("clearsky") >= 0) category = 0;      // despejado
            else if (symbolCode.indexOf("fair") >= 0) category = 1;     // poco nuboso
            else if (symbolCode.indexOf("partlycloudy") >= 0) category = 1; // poco nuboso
            else if (symbolCode.indexOf("cloudy") >= 0) category = 2;   // nublado
            else if (symbolCode.indexOf("fog") >= 0) category = 3;      // niebla
            else if (symbolCode.indexOf("rain") >= 0) category = 4;     // lluvia
            else if (symbolCode.indexOf("sleet") >= 0) category = 4;    // lluvia/nieve → lluvia
            else if (symbolCode.indexOf("snow") >= 0) category = 6;     // nieve
            else if (symbolCode.indexOf("thunder") >= 0) category = 5;  // tormenta
            
            tempSources[2]  = t;
            weatherCodes[2] = category;
            // Intentar leer humedad desde instant.details (si existe)
            int hum2 = -1;
            if (data["instant"]["details"].containsKey("relative_humidity")) {
              hum2 = (int)data["instant"]["details"]["relative_humidity"];
            } else if (data["instant"]["details"].containsKey("relative_humidity_percent")) {
              hum2 = (int)data["instant"]["details"]["relative_humidity_percent"];
            } else if (data["instant"]["details"].containsKey("humidity")) {
              hum2 = (int)data["instant"]["details"]["humidity"];
            }
            humiditySources[2] = hum2;
            sourceValid[2]  = true;
            Serial.printf("  ✅ Met.no: %.1f°C | Símbolo: %s | Categoría: %d\n", t, symbolCode.c_str(), category);
          } else {
            sourceValid[2] = false;
            Serial.println("  ❌ Met.no: No hay datos en timeseries[0]");
          }
        } else {
          sourceValid[2] = false;
          Serial.println("  ❌ Met.no: No hay timeseries o está vacío");
        }
      } else {
        sourceValid[2] = false;
        Serial.println("  ❌ Met.no: No hay 'properties' en respuesta");
      }
    } else {
      sourceValid[2] = false;
      Serial.printf("  ❌ Met.no: Error al parsear JSON - %s\n", error.c_str());
    }
  } else {
    sourceValid[2] = false;
    Serial.printf("  ❌ Met.no: Error HTTP %d\n", code);
  }
  http.end();
}


// ═════════════════════════════════════════════
//  ACTUALIZACIÓN PRINCIPAL
// ═════════════════════════════════════════════
void updateWeather() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[CLIMA] ❌ WiFi no conectado - Sincronización pausada");
    return;
  }

  unsigned long startTime = millis();
  Serial.println("\n" "—" "—" "—" "—" "—" "—" "—" "—" "—" "—" "—" "—" "—" "—" "—" "—" "—" "—" "—" "—" "—" "");
  Serial.println("[CLIMA] 🔄 INICIANDO SINCRONIZACIÓN CON API DE CLIMA");
  Serial.println("—" "—" "—" "—" "—" "—" "—" "—" "—" "—" "—" "—" "—" "—" "—" "—" "—" "—" "—" "—" "—" "");

  // Resetear validez antes de cada ciclo completo
  for (int i = 0; i < 3; i++) sourceValid[i] = false;

  Serial.println("[CLIMA] 📡 Consultando Open-Meteo...");
  fetchOpenMeteo();
  
  Serial.println("[CLIMA] 📡 Consultando OpenWeatherMap...");
  fetchOpenWeatherMap();
  
  Serial.println("[CLIMA] 📡 Consultando Met.no...");
  fetchMetno();
  
  fuseWeatherData();
  
  unsigned long elapsed = millis() - startTime;
  Serial.printf("[CLIMA] ⏱️  Sincronización completada en %lu ms\n", elapsed);
}


// ═════════════════════════════════════════════
//  TRIGGER EXTERNO (desde otro task/ISR)
// ═════════════════════════════════════════════
void triggerWeatherFetch() {
  weatherFetchRequested = true;
  if (weatherTaskHandle != NULL) xTaskNotifyGive(weatherTaskHandle);
}


// ═════════════════════════════════════════════
//  TASK DE FREERTOS
// ═════════════════════════════════════════════
void weatherTask(void *pvParameters) {
  const TickType_t shortDelay = pdMS_TO_TICKS(600000); // 10 minutos
  const TickType_t longDelay  = pdMS_TO_TICKS(300000); // 5 minutos (sin WiFi)
  
  Serial.println("\n[CLIMA] ✦ TAREA DE SINCRONIZACIÓN INICIADA");
  Serial.println("[CLIMA] ⏰ Intervalo de actualización: 10 minutos (WiFi conectado)");
  Serial.println("[CLIMA] ⏰ Intervalo de reintento: 5 minutos (WiFi desconectado)\n");

  while (true) {
    // Actualización periódica
    if (WiFi.status() == WL_CONNECTED) {
      updateWeather();
    } else {
      Serial.println("[CLIMA] ⏳ Esperando reconexión WiFi para sincronizar...");
    }

    // Esperar notificación o timeout (10 min cuando WiFi conectado)
    ulTaskNotifyTake(pdTRUE, shortDelay);

    // Petición externa (botón, evento, etc.)
    if (weatherFetchRequested) {
      weatherFetchRequested = false;
      Serial.println("[CLIMA] 🔔 Sincronización solicitada por evento externo");
      if (WiFi.status() == WL_CONNECTED) {
        updateWeather();
      } else {
        Serial.println("[CLIMA] ❌ No se puede sincronizar: WiFi desconectado");
      }
      continue;
    }

    // Sin WiFi: esperar más antes del próximo intento
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("[CLIMA] 📶 Reconexión en 5 minutos...");
      vTaskDelay(longDelay);
    }
  }
}

#endif // CLIMA_H