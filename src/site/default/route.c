#include "route.h"
#include <glib.h>
#include "../../data/tickets.h"
#include "json-c/json_object.h"
#include "mustach/mustach.h"
#include "mustach/mustach-json-c.h"
#include "../../aux/loader.h"
#include <ctype.h>


gchar *get_parameterized_action(const GString *method);
void set_param(GString *value, application_context *ctx);
void add_param(GString *value, application_context *ctx);

bool default_route_try(struct mg_connection* c, void* ev_data, application_context* ctx) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    struct mg_str parts4[4];
    struct mg_str parts3[3];
    struct mg_str parts2[2];
    GString *guri = g_string_new_len(hm->uri.buf, hm->uri.len);
    struct mg_str uri = mg_str_n(hm->uri.buf, hm->uri.len);
    GString *method = g_string_new_len(hm->method.buf, hm->method.len);
    GString *controller = g_string_new("");
    GString *action = g_string_new("");
    GString *id = g_string_new("");
    method = g_string_ascii_down(method);
    ActionDelegate action_delegate = NULL;
    bool exitcode = FALSE;

    MG_INFO(("Trying URI: '%s'", guri->str));
    g_string_free(guri, TRUE);

    if (mg_match(uri, mg_str("/*/*/*"), parts4)) {

        // Controller/Action/Id
        controller = parts4[0].len > 0
                        ? g_string_append_len(controller, parts4[0].buf, parts4[0].len)
                        : g_string_append(controller, "default");
        action = parts4[1].len > 0
                        ? g_string_append_len(action, parts4[1].buf, parts4[1].len)
                        : g_string_append(action, "index");
        id = parts4[2].len > 0
                        ? g_string_append_len(id, parts4[2].buf, parts4[2].len)
                        : id;
        set_param(id, ctx);
    } else if (mg_match(uri, mg_str("/*/*"), parts3)) {

        // Controller/Action|Id
        controller = parts3[0].len > 0
                        ? g_string_append_len(controller, parts3[0].buf, parts3[0].len)
                        : g_string_append(controller, "default");
        action = parts3[1].len > 0
                        ? g_string_append_len(action, parts3[1].buf, parts3[1].len)
                        : g_string_append(action, "index");
        action_delegate = get_action_delegate(method, controller, action);
        // Either action or id:
        if (action_delegate == NULL) {
            id = g_string_append(id, action->str);
            g_string_printf(action, "%s", "index");
            set_param(id, ctx);
        }
    } else if (mg_match(uri, mg_str("/*"), parts2)) {

        // Controller/index
        controller = parts2[0].len > 0
                        ? g_string_append_len(controller, parts2[0].buf, parts2[0].len)
                        : g_string_append(controller, "default");
        action = g_string_append(action, "index");
        action_delegate = get_action_delegate(method, controller, action);
        // Either controller/index or default/action:
        if (action_delegate == NULL) {
            g_string_printf(action, "%s", controller->str);
            g_string_printf(controller, "%s", "default");
        }
    } else {

        // unknown/unknown
        controller = g_string_append(controller, "_unknown");
        action = g_string_append(action, "_unknown");
    }

    MG_INFO(("\t-> %s: %s('%s') @ %s", method->str, action->str, id->str, controller->str));
    if (action_delegate == NULL) {
        action_delegate = get_action_delegate(method, controller, action);
    }
    if (action_delegate != NULL) {
        exitcode = TRUE;
        action_delegate(c, ev_data, ctx);
    } else {
        MG_INFO(("...not found"));
    }

    g_string_free(method, TRUE);
    g_string_free(controller, TRUE);
    g_string_free(action, TRUE);
    g_string_free(id, TRUE);

    return exitcode;
}

void set_param(GString* value, application_context* ctx) {
    reset_url_matches(ctx);
    add_param(value, ctx);
}

void add_param(GString* value, application_context* ctx) {
    GString *item = g_string_new("");
    item = g_string_append(item, value->str);
    ctx->url_matches = g_list_append(ctx->url_matches, g_string_free_and_steal(item));
}

gchar *get_parameterized_action(const GString* method) {
    gchar *action = NULL;
    GString *mtest = g_string_new("get");
    if (g_string_equal(method, mtest)) {
        action = (gchar*)"show";
    } else {
        /* POST */
        action = (gchar*)"update";
    }
    g_string_free(mtest, TRUE);
    return action;
}

void default_route_cleanup() {
    loader_release();
}
