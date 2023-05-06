#ifndef TICKETING_STATIC_ROUTE_H
#define TICKETING_STATIC_ROUTE_H

#include <stdbool.h>
#include "../../lib/mongoose.h"
#include "../context.h"

bool static_route_try(struct mg_connection *c, void *ev_data, application_context *ctx);

#endif
