# AlarmaESP32 V5.1

Alarma inteligente para ESP32 con pantalla circular GC9A01A, reloj analógico/digital, alarmas avanzadas, cronómetro, temporizador, juegos arcade, control web y MQTT, y clima en tiempo real.

> Este proyecto es un prototipo universitario. Usa con fines educativos y no como solución IoT de producción.

## Índice

- [Descripción](#descripción)
- [Resumen en inglés](#english-summary)
- [Funcionalidades](#funcionalidades)
- [Requisitos](#requisitos)
- [Instalación](#instalación)
- [Configuración](#configuración)
- [Uso](#uso)
- [MQTT remoto](#mqtt-remoto)
- [Panel web](#panel-web)
- [Documentación adicional](#documentación-adicional)
- [Estructura de archivos](#estructura-de-archivos)
- [Notas y limitaciones](#notas-y-limitaciones)
- [Solución de problemas](#solución-de-problemas)
- [Estado del proyecto](#estado-del-proyecto)
- [Seguridad](#seguridad)
- [Licencia](#licencia)
- [Contribuciones](#contribuciones)

## Descripción

AlarmaESP32 V5.1 es un prototipo para ESP32 que combina:

- pantalla circular GC9A01A
- reloj analógico y digital
- alarmas recurrentes y alarmas por fecha
- cronómetro y temporizador
- tres juegos arcade: Flappy Bird, Asteroids y Runner
- control web integrado
- comandos MQTT para control remoto
- estado de clima en pantalla
- efectos NeoPixel

El diseño está orientado a prueba de concepto académico y al aprendizaje de IoT, UI embedded y comunicación MQTT.

## English summary

AlarmaESP32 V5.1 is an ESP32 university prototype that provides a circular TFT alarm clock with remote control through MQTT and a web dashboard. It includes analog and digital clock faces, advanced alarms, timer/stopwatch, three arcade games, and weather display.

This repository includes additional documentation for contributors:

- `CONTRIBUTING.md`
- `CODE_OF_CONDUCT.md`
- `SECURITY.md`
- `STATUS.md`
- `CHANGELOG.md`
- `docs/README.md`

## Funcionalidades

### Reloj

- reloj analógico y digital
- 3 carátulas analógicas y 3 digitales
- cambio de diseño por botones o web
- sincronización de hora con NTP

### Alarmas

- alarmas recurrentes por días de la semana
- alarmas puntuales por fecha
- hasta 10 alarmas almacenadas
- 5 tonos de alarma
- crear, eliminar y listar alarmas
- opción de borrar alarma al dispararse
- vista previa de tono
- detener / snooze

### Cronómetro y temporizador

- cronómetro con inicio, pausa y reset
- temporizador con alarma sonora
- visualización en pantalla

### Juegos arcade

- Flappy Bird
- Asteroids
- Runner
- puntajes Top 5 persistentes por juego
- control físico y por web

### Clima

- actualización de clima por API
- votación entre tres fuentes (Open-Meteo, OpenWeatherMap y Met.no) en cada sincronización
- estado meteorológico en pantalla con icono y temperatura
- refresco manual desde web o MQTT
- actualización automática cada 10 minutos y cada solicitud manual

### Conectividad

- WiFi con WiFiManager
- punto de acceso de configuración siempre disponible en el arranque (`ESP32_Alarma_V5`)
- intenta conectar a la red guardada al iniciar
- servidor web integrado en ESP32 (puerto 80)
- MQTT con broker público por defecto
- API web interna y páginas HTML embebidas

### Persistencia

- alarmas y puntajes guardados en Preferences
- configuración básica almacenada en la memoria interna

## Requisitos

### Hardware

- ESP32 (DevKit o similar)
- pantalla TFT GC9A01A circular 240x240
- tira NeoPixel WS2812 (16 LEDs recomendado)
- buzzer pasivo compatible con 3.3V
- 3 botones con pull-up interno
- fuente USB o batería 3.7–5V

### Software

- VS Code + PlatformIO o Arduino IDE
- PlatformIO con plataforma espressif32

### Librerías utilizadas

- PubSubClient
- Adafruit GFX Library
- Adafruit GC9A01A
- Adafruit NeoPixel
- ArduinoJson
- WiFiManager

## Instalación

### 1. Clona el repositorio

```bash
git clone https://github.com/<usuario>/AlarmaEsp32V5.1.git
cd AlarmaEsp32V5.1
```

### 2. Abre el proyecto

- en PlatformIO: abre la carpeta en VS Code
- en Arduino IDE: abre `AlarmaEsp32V5.1.ino`

### 3. Instala dependencias

En PlatformIO, las dependencias se descargan automáticamente con `pio run`.

```bash
pio run
```

### 4. Ajusta la configuración

Revisa `Config.h` si tu hardware difiere de los pines definidos.

### 5. Compila y sube

```bash
pio run -e esp32dev
pio run -e esp32dev --target upload
```

### 6. Monitor serie

```bash
pio device monitor -b 115200
```

## Configuración

- `Config.h` contiene pines, MQTT, WiFi y constantes del sistema.
- `platformio.ini` define la placa ESP32, framework y librerías.
- Si usas otra placa ESP32, ajusta el entorno PlatformIO.

## Uso

### Configuración WiFi inicial

Al arrancar, el ESP32 inicia siempre con el portal de configuración abierto en el AP `ESP32_Alarma_V5`.
- Si hay una red WiFi guardada, el dispositivo intenta conectarse automáticamente.
- Si no puede conectarse, el portal de configuración permanece activo para ingresar nueva red.
- Conéctate a `ESP32_Alarma_V5` y abre `http://192.168.4.1`.

### Accede al panel web

Abre `http://<IP-del-ESP32>` en tu navegador, o `http://192.168.4.1` cuando estés conectado al AP de configuración.

### Control físico

- B1: atrás / cancelar
- B2: siguiente / bajar
- B3: aceptar / acción

## MQTT remoto

El dispositivo publica respuestas en `alarm/status` y acepta los siguientes comandos.

### Comandos MQTT

- `alarm/set`
  - Payload: `NOMBRE|DIAS|HH:MM|TONO`
  - Ejemplo: `Trabajo|LUN,MAR,MIE|07:30|2`
  - También admite `NOMBRE|DIAS|HH:MM|TONO|1` para borrar al dispararse.

- `alarm/delete`
  - Payload: `NombreDeAlarma`

- `alarm/modify`
  - Payload: `NOMBRE|NUEVOS_DIAS|NUEVA_HORA`
  - Usa `*` para no cambiar un campo
  - Ejemplo: `Trabajo|VIE|08:00`

- `alarm/timer`
  - Payload: minutos (ej: `15`)

- `alarm/crono`
  - Payload: `START`, `PAUSE`, `RESET`, `GET`

- `alarm/face`
  - Payload: `ANALOG1`, `ANALOG2`, `ANALOG3`, `DIGITAL1`, `DIGITAL2`, `DIGITAL3`

- `alarm/message`
  - Payload: texto (hasta 120 caracteres)

- `alarm/weather`
  - solicita actualización inmediata del clima

- `alarm/info`
  - solicita IP, RSSI y uptime

- `alarm/list`
  - solicita listado de alarmas

- `alarm/stop`
  - Payload: `SNOOZE` para posponer, otro valor para detener

### Ejemplo

```bash
mosquitto_pub -h broker.emqx.io -t alarm/set -m "Trabajo|LUN,MAR,MIE|07:30|2"
mosquitto_pub -h broker.emqx.io -t alarm/crono -m "START"
mosquitto_pub -h broker.emqx.io -t alarm/face -m "DIGITAL2"
```

## Panel web

La interfaz web ofrece:

- control de alarmas
- cronómetro y temporizador
- selección de carátulas
- control NeoPixel
- información de dispositivo
- juegos con D-Pad virtual
- scores Top 5

También hay una pantalla QR integrada para acceder al panel sin escribir la IP.

## Documentación adicional

Este repositorio incluye documentación de soporte para contribuir y revisar el proyecto.

- `CONTRIBUTING.md`
- `CODE_OF_CONDUCT.md`
- `SECURITY.md`
- `STATUS.md`
- `CHANGELOG.md`
- `docs/README.md`
- `.github/ISSUE_TEMPLATE/`
- `.github/pull_request_template.md`

## Estructura de archivos

```
AlarmaEsp32V5.1/
├── AlarmaEsp32V5.1.ino
├── Config.h
├── Alarms.h
├── Logica.h
├── Interfaz.h
├── Red.h
├── Clima.h
├── Relojes.h
├── Melodies.h
├── Asteroids.h
├── Runner.h
├── Flapply_Bird.h
├── Scores.h
├── partitions.csv
├── platformio.ini
├── qrcode.c
├── qrcode.h
├── README.md
├── CONTRIBUTING.md
├── CODE_OF_CONDUCT.md
├── SECURITY.md
├── STATUS.md
├── CHANGELOG.md
├── LICENSE
├── .gitignore
└── docs/README.md
```

## Notas y limitaciones

- Los juegos implementados son Flappy Bird, Asteroids y Runner.
- No existe soporte Breakout en el código.
- Límite de 10 alarmas.
- Las puntuaciones Top 5 se guardan por juego.
- El panel web no está autenticado.
- MQTT no usa TLS/autenticación por defecto.
- El portal WiFi de configuración está disponible en cada arranque mediante el AP `ESP32_Alarma_V5`.
- Las credenciales WiFi se muestran como ejemplo en el código.

## Solución de problemas

| Problema | Solución |
|----------|----------|
| WiFi no conecta | Verifica las credenciales en el portal del ESP32 o conéctate a `ESP32_Alarma_V5` en `192.168.4.1` |
| Pantalla en blanco | Revisa las conexiones SPI y la alimentación 3.3V |
| Buzzer no suena | Comprueba el pin 25 y la polaridad del buzzer |
| MQTT no conecta | Verifica broker, puerto y firewall |
| Botones no responden | Revisa el cableado y las interrupciones |

## Estado del proyecto

- Prototipo funcional
- Material de soporte para OSS agregado
- Seguridad básica documentada
- No está listo para red pública sin refactorización

## Seguridad

- El panel web no tiene autenticación.
- MQTT usa broker sin TLS por defecto.
- Cambia `ssid`, `password` y `mqttServer` antes de compilar.
- Revisa `SECURITY.md` para más detalles.

## Licencia

Este repositorio incluye un archivo `LICENSE` con licencia MIT.

## Contribuciones

1. Haz fork del repositorio.
2. Abre un issue o usa una plantilla de solicitud de función.
3. Crea una rama descriptiva.
4. Verifica la compilación con `pio run -e esp32dev`.
5. Envía un pull request con descripción y pruebas.
