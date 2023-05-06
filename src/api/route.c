#include "route.h"
#include <glib.h>
#include "../data/tickets.h"

bool api_route_try(struct mg_connection* c, void* ev_data, application_context* ctx) {
    struct mg_http_message *hm = ev_data;
    struct mg_str parts[3];
    bool match = mg_match(hm->uri, mg_str(API_ROUTE), parts);
    if (match) {
        api_route_handle(c, ev_data, parts, ctx);
    }
    return match;
}

void api_route_handle(struct mg_connection *c, void *ev_data, struct mg_str parts[3], application_context *ctx) {
    MG_INFO(("API match: %s", API_ROUTE));
    GString *controller = parts[0].len > 0
        ? g_string_new_len(parts[0].ptr, parts[0].len)
        : g_string_new("default");
    GString *action = parts[1].len > 0
        ? g_string_new_len(parts[1].ptr, parts[1].len)
        : g_string_new("world");
    MG_INFO(("\t\t(%s) / (%s)", controller->str, action->str));
    struct mg_http_message *hm = ev_data;

    if (mg_http_match_uri(hm, "/api/tickets/*")) {

        if (mg_match(mg_str(action->str), mg_str("world"), NULL)) {
            GString *json = tickets_fetch_all(ctx->db);
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s\n", json->str);
            g_string_free(json, TRUE);
        } else {
            GString *json = tickets_fetch_one(ctx->db, mg_to64(mg_str(action->str)));
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s\n", json->str);
            g_string_free(json, TRUE);
        }

    } else {
        mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{%m:%m,%m:%m}",
                      mg_print_esc, 0, "greeting", mg_print_esc, controller->len, controller->str,
                      mg_print_esc, 0, "name",     mg_print_esc, action->len,     action->str);
    }
    g_string_free(controller, TRUE);
    g_string_free(action, TRUE);
}
