#ifndef TICKETING_AUX_AUTH_H
#define TICKETING_AUX_AUTH_H

#include "../context.h"
#include "../data/users.h"

User *get_user(struct mg_http_message *hm, application_context *ctx);

#endif
