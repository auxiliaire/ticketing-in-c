#ifndef TICKETING_AUX_MIDDLEWARE_AUTH_H
#define TICKETING_AUX_MIDDLEWARE_AUTH_H

#include "../../context.h"

gboolean guard_auth_html(struct mg_connection* c, struct mg_http_message *hm, application_context *ctx);
gboolean guard_auth_json(struct mg_connection* c, struct mg_http_message *hm, application_context *ctx);

#endif
