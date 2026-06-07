# Seguridad

Este proyecto contiene componentes de red y controles remotos. Ten en cuenta lo siguiente antes de usarlo en un entorno real.

## Riesgos conocidos

- El panel web no está autenticado.
- MQTT usa broker sin TLS por defecto (`broker.emqx.io` y puerto 1883).
- Las credenciales WiFi están visibles en el código como ejemplo.
- No hay protección contra ataques de fuerza bruta o acceso no autorizado en la interfaz web.

## Recomendaciones

- Usa este proyecto solo en redes privadas de confianza.
- Cambia `ssid`, `password` y `mqttServer` antes de compilar.
- Implementa autenticación web y MQTT si planeas exponer el dispositivo a Internet.
- Evalúa agregar TLS y autenticación MQTT.

## Reporte de vulnerabilidades

Si descubres un problema de seguridad, abre un issue con el prefijo `SECURITY:` y describe el riesgo, la versión del proyecto y los pasos para reproducirlo.
