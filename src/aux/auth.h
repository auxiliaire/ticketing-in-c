#ifndef TICKETING_AUX_AUTH_H
#define TICKETING_AUX_AUTH_H

#include "../context.h"

void *get_user(struct mg_http_message *hm, application_context *ctx);

#endif
