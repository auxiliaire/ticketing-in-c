#ifndef TICKETING_CONTEXT_H
#define TICKETING_CONTEXT_H

#include "../lib/mongoose.h"
#include <glib.h>

typedef struct context {
    void *db;
    GString *layout;
    GList *url_matches;
    const char *root_dir;
} application_context;

void reset_url_matches(application_context *ctx);

#endif
