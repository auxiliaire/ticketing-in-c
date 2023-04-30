#ifndef TICKETING_SITE_DEFAULT_ROUTE_H
#define TICKETING_SITE_DEFAULT_ROUTE_H

#include "../../../lib/mongoose.h"

#define DEFAULT_ROUTE "/"

#define LAYOUT_VIEW "view/layout.html"

// TODO: define default_route_try.
void default_route(struct mg_connection *c, void *ev_data);

#endif
