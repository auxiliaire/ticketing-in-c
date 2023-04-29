#ifndef TICKETING_CONTEXT_H
#define TICKETING_CONTEXT_H

#include "../lib/mongoose.h"

typedef struct context {
    void *db;
} application_context;

#endif
