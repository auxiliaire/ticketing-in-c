#ifndef TICKETING_API_ROUTE_H
#define TICKETING_API_ROUTE_H

#include <stdbool.h>
#include "../../lib/mongoose.h"
#include "../context.h"

#define API_ROUTE "/api/*/*"

bool api_route_try(struct mg_connection *c, void *ev_data, application_context *ctx);
void api_route_handle(struct mg_connection *c, void *ev_data, struct mg_str parts[3], application_context *ctx);

#endif
