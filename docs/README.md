# Documentación del proyecto

Este documento describe la arquitectura y la organización principal del proyecto `AlarmaESP32`.

## Visión general

El proyecto es un firmware para ESP32 que combina:

- reloj analógico/digital
- alarmas con repetición semanal y por fecha
- cronómetro y temporizador
- tres juegos arcade
- control web y MQTT
- visualización de clima
- efectos NeoPixel

## Módulos principales

- `AlarmaEsp32V5.1.ino`
  - Inicializa el hardware, WiFi, MQTT, servidor web y las tareas del sistema.
  - Define los objetos globales compartidos entre módulos.

- `Config.h`
  - Contiene constantes de pines, tópicos MQTT y variables externas compartidas.
  - Actúa como punto de referencia para la configuración general.

- `Red.h`
  - Implementa el servidor web y las rutas HTTP.
  - Contiene el HTML embebido para el dashboard y control arcade.
  - Registra suscripciones MQTT.

- `Logica.h`
  - Procesa mensajes MQTT.
  - Maneja comandos como `alarm/set`, `alarm/delete`, `alarm/face`, `alarm/crono`, `alarm/weather`.
  - Gestiona la lógica de parada, snooze y respuestas en `alarm/status`.

- `Alarms.h`
  - Parseo y almacenamiento de alarmas.
  - Soporta alarmas recurrentes y alarmas por fecha.

- `Interfaz.h`
  - Controla la navegación del menú en pantalla con los botones físicos.
  - Dibuja relojes, alarmas, mensajes y estado en la pantalla TFT.

- `Clima.h`
  - Obtiene datos de clima desde APIs externas.
  - Actualiza variables globales de temperatura, humedad y código meteorológico.

- `Relojes.h`
  - Lógica de dibujo de carátulas de reloj analógico y digital.

- `Melodies.h`
  - Define tonos y secuencias del buzzer para alarmas y notificaciones.

- `Scores.h`
  - Maneja el almacenamiento de puntajes top 5 para los juegos.

- `Asteroids.h`, `Runner.h`, `Flapply_Bird.h`
  - Contienen la lógica de los juegos arcade implementados.

## Flujo de datos

1. El ESP32 arranca y se conecta a WiFi mediante `WiFiManager`.
2. Se inicia el servidor web y se suscribe a tópicos MQTT.
3. Los botones físicos actualizan el estado de `menuEstado` y la pantalla.
4. Los comandos MQTT se procesan en `Logica.h` y publican respuestas en `alarm/status`.
5. El panel web ejecuta acciones mediante rutas `/api/...` que actualizan el firmware.
6. Las alarmas, puntajes y configuraciones se almacenan en `Preferences`.

## Observaciones técnicas

- El HTML del panel web está embebido en `Red.h` como `PROGMEM`.
- El proyecto no tiene pruebas automatizadas; solo se valida con compilación.
- La configuración de MQTT y WiFi está hardcodeada en `AlarmaEsp32V5.1.ino`.
- No se usa un esquema modular de hardware/software aislado; hay muchas variables globales.

## Recomendaciones para evolución

- Extraer la configuración de red a un archivo separado o a un gestor de parámetros.
- Mover el HTML a archivos externos si el firmware crece.
- Añadir pruebas de compilación y scripts de verificación.
- Normalizar el idioma del código y los nombres de archivos.
