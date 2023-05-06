#ifndef TICKETING_AUX_LOADER_H
#define TICKETING_AUX_LOADER_H

#include <glib.h>
#include "../context.h"

typedef void (*ActionDelegate)(struct mg_connection *c, void *ev_data, application_context *ctx);

ActionDelegate get_action_delegate(GString *method, GString *controller, GString *action);
void loader_release();

#endif
