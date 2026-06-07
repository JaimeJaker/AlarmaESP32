# Estado del proyecto

Este documento describe el estado actual del repositorio y el nivel de madurez del proyecto.

## Estado general

- Prototipo funcional: ✅
- Documentación básica: ✅
- Seguridad adecuada para producción: ❌
- Integración continua: ✅ (build automático en GitHub Actions)
- Pruebas automatizadas: ❌
- Preparado para contribuciones externas: Parcialmente

## Funcionalidades implementadas

- Reloj analógico y digital
- Alarmas recurrentes y alarmas por fecha
- Cronómetro y temporizador
- Juegos arcade: Flappy Bird, Asteroids, Runner
- Panel web con control remoto y comandos MQTT
- Visualizador de clima y datos de dispositivo
- Almacenamiento de alarmas y puntajes en `Preferences`

## Limitaciones conocidas

- No hay autenticación web ni MQTT.
- No se debe usar en redes públicas sin revisión previa.
- No existen pruebas unitarias o de integración.
- El nombre del proyecto incluye la versión; podría normalizarse.

## Próximos pasos sugeridos

- Añadir autenticación y seguridad en la interfaz web.
- Implementar tests incrementales y CI más completo.
- Reorganizar código en módulos más independientes.
- Incluir `CONTRIBUTING.md`, plantillas e instrucciones de contribución.
