#ifndef TICKETING_SITE_DEFAULT_ROUTE_H
#define TICKETING_SITE_DEFAULT_ROUTE_H

#include "../../../lib/mongoose.h"
#include "../../context.h"

#define DEFAULT_ROUTE "/"
#define CONTROLLER_ACTION_ROUTE "/*/#"
#define ACTION_ID_ROUTE "*/*"

bool default_route_try(struct mg_connection *c, void *ev_data, application_context *ctx);
void default_route_handle(struct mg_connection *c, void *ev_data, application_context *ctx);
void default_route_cleanup();

#endif
