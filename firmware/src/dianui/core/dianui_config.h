#ifndef DIANUI_CONFIG_H
#define DIANUI_CONFIG_H

/* Maximum number of elements in the registry */
#ifndef DIANUI_MAX_ELEMENTS
#define DIANUI_MAX_ELEMENTS 20
#endif

/* Pool sizes per element type */
#ifndef DIANUI_MAX_TEXT
#define DIANUI_MAX_TEXT 10
#endif

#ifndef DIANUI_MAX_ICONS
#define DIANUI_MAX_ICONS 5
#endif

#ifndef DIANUI_MAX_RECTANGLES
#define DIANUI_MAX_RECTANGLES 5
#endif

#ifndef DIANUI_MAX_CIRCLES
#define DIANUI_MAX_CIRCLES 5
#endif

#ifndef DIANUI_MAX_LINES
#define DIANUI_MAX_LINES 5
#endif

#ifndef DIANUI_MAX_TRIANGLES
#define DIANUI_MAX_TRIANGLES 5
#endif

#ifndef DIANUI_MAX_QR
#define DIANUI_MAX_QR 1
#endif

#ifndef DIANUI_MAX_ANIMATIONS
#define DIANUI_MAX_ANIMATIONS 5
#endif

#ifndef DIANUI_MAX_SCENES
#define DIANUI_MAX_SCENES 2
#endif

#ifndef DIANUI_MAX_POLYGONS
#define DIANUI_MAX_POLYGONS 5
#endif

#ifndef DIANUI_MAX_MENUS
#define DIANUI_MAX_MENUS 3
#endif

/* Enable or disable logging (set to 0 to disable) */
#ifndef DIANUI_LOG_ENABLED
#define DIANUI_LOG_ENABLED 1
#endif

#endif // DIANUI_CONFIG_H