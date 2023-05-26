#ifndef TICKETING_AUX_CONTENT_NEGOTIATION_H
#define TICKETING_AUX_CONTENT_NEGOTIATION_H

#include "loader.h"

void cn_switch_json_html(struct mg_connection* c, void* ev_data, application_context* ctx, ActionDelegate json_method, ActionDelegate html_method);

#endif
