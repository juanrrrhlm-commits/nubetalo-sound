# Seguridad de Nubetalo Sound

## Cómo reportar una vulnerabilidad
Escribe a **juanrrrhlm@gmail.com** con el asunto `[Nubetalo Sound Soporte] Seguridad`. Incluye la versión, los pasos para reproducirlo y su impacto. Responderé lo antes posible; por favor no publiques el detalle hasta que haya una corrección.

## Versiones con soporte
Solo la versión más reciente publicada.

## Qué se ha revisado (versión 1.0.0)
Revisión propia del código y pruebas locales sobre la app:

| Área | Resultado |
|---|---|
| Secretos, claves o credenciales en el código | Ninguno. Solo el correo de soporte, que es público a propósito. |
| Conexiones de red | Ninguna iniciada por la app. Se eliminaron la consulta de anuncios, el actualizador y la encuesta heredados. |
| Procesos externos | Se eliminó el lanzamiento de `updater.exe` desde la carpeta de trabajo. |
| Archivos de preajuste malformados (vacíos, binarios, 3 MB en una línea, valores extremos) | Se corrigió un bloqueo de la app con una línea gigante; ahora se descartan los archivos de más de 64 KB. |
| Nombres de preajuste con rutas (`..\..\x`) | Se corrigió: una importación podía escribir un archivo fuera de la carpeta de presets. Ahora se rechazan nombres con rutas, caracteres reservados o nombres de dispositivo de Windows. |
| Carga de archivos desde la carpeta desde donde se lanza el programa | Los presets de fábrica se leen solo junto al ejecutable. |
| Borrado de claves de registro de otros programas | Se eliminó el borrado de `HKCU\Software\DFX` que hacía el original. |
| Ganchos de desarrollo (variables de entorno) | Se compilan fuera de la versión de distribución. |
| Protecciones del ejecutable | ASLR de alta entropía, DEP, Control Flow Guard; sin ruta del equipo de compilación en el binario. |

## Límites conocidos
- Cualquier proceso del mismo usuario de Windows puede controlar la app con su línea de comandos (`--power`, `--preset`...), igual que podría cerrarla. No cruza límites de privilegio.
- El dispositivo virtual de audio usa el controlador FXVAD de FxSound (firmado por su autor). Un fallo en ese controlador queda fuera del alcance de este proyecto.
- El ejecutable no está firmado digitalmente con un certificado de editor: Windows SmartScreen puede avisar al instalarlo.

## Código abierto
La licencia AGPL v3 exige ofrecer el código fuente a quien reciba el programa, por lo que no puede mantenerse oculto. La seguridad no depende de ocultarlo.
