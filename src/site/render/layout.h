#ifndef TICKETING_RENDER_LAYOUT_H
#define TICKETING_RENDER_LAYOUT_H

#include <glib.h>

typedef struct LayoutModel {
    char *layout_filename;
    char *title;
    char *content;
    char *script;
} LayoutModel;

GString *render_layout(const LayoutModel *layout_model);

#endif
