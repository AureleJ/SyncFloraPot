#ifndef QR_CODE_H
#define QR_CODE_H

#include "../core/ui_types.h"

QRCodeElement *ui_create_qr_code(int x, int y, int w, int h, Anchor xAnchor, Anchor yAnchor, const char *data);

#endif // QR_CODE_H