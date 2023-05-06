#ifndef TIICKETING_TICKETS_CONTROLLER_H
#define TIICKETING_TICKETS_CONTROLLER_H

#include "../../../lib/mongoose.h"
#include "../../context.h"

void post_index_action(struct mg_connection* c, void* ev_data, application_context* ctx);
void get_index_action(struct mg_connection* c, void* ev_data, application_context* ctx);
void put_index_action(struct mg_connection* c, void* ev_data, application_context* ctx);
void delete_index_action(struct mg_connection* c, void* ev_data, application_context* ctx);

void get_create_action(struct mg_connection* c, void* ev_data, application_context* ctx);
void get_show_action(struct mg_connection* c, void* ev_data, application_context* ctx);
void get_edit_action(struct mg_connection* c, void* ev_data, application_context* ctx);
void put_update_action(struct mg_connection* c, void* ev_data, application_context* ctx);
void post_update_action(struct mg_connection* c, void* ev_data, application_context* ctx);
void delete_one_action(struct mg_connection* c, void* ev_data, application_context* ctx);

#endif
