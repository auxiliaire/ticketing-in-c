#ifndef TIICKETING_DEFAULT_CONTROLLER_H
#define TIICKETING_DEFAULT_CONTROLLER_H

#include "../../../lib/mongoose.h"
#include "../../context.h"

void get_index_action(struct mg_connection* c, void* ev_data, application_context* ctx);
void get_login_action(struct mg_connection* c, void* ev_data, application_context* ctx);

#endif
