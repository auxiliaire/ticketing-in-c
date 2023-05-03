#ifndef TICKETING_CONTEXT_H
#define TICKETING_CONTEXT_H

#include "../lib/mongoose.h"
#include <glib.h>

typedef struct context {
    void *db;
    GString *layout;
    GList *url_matches;
} application_context;

#endif
