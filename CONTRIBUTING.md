# Contribuciones

Gracias por tu interés en contribuir a este proyecto de universidad. Este repositorio está diseñado para ser un prototipo educativo, pero queremos que cualquier contribución sea clara, segura y respetuosa.

## Cómo contribuir

1. Abre un issue antes de comenzar un cambio importante.
2. Crea una rama con un nombre descriptivo, por ejemplo: `feature/alarm-mqtt` o `fix/web-panel`.
3. Realiza los cambios en tu rama.
4. Verifica que el proyecto compila con `pio run -e esp32dev`.
5. Envía un pull request detallando el propósito y los pasos para probarlo.

## Requisitos mínimos

- Mantén la documentación actualizada en `README.md` o `docs/README.md`.
- No incluyas credenciales, claves, contraseñas ni datos personales.
- Reduce cambios globales de estilo a lo estrictamente necesario.
- Prefiere comentarios en español para que el repositorio sea consistente con el código actual.

## Proceso de revisión

- Los cambios serán revisados mediante pull request.
- Agrega una descripción clara del problema que resuelve el PR.
- Identifica si tu cambio es un arreglo, una mejora o una nueva funcionalidad.
- Incluye capturas de pantalla o ejemplos cuando el cambio afecte la interfaz web.

## Pruebas

Antes de enviar un PR, ejecuta:

```bash
pio run -e esp32dev
```

Si desarrollas nuevas funciones importantes, agrega documentación en `docs/README.md` y describe cómo probarlas.
