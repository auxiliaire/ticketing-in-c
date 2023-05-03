#include "route.h"
#include <glib.h>
#include "../../data/tickets.h"
#include "json-c/json_object.h"
#include "mustach/mustach.h"
#include "mustach/mustach-json-c.h"
#include <dlfcn.h>
#include <ctype.h>


void log_key(gpointer key, gpointer value, gpointer user_data);
void *get_handle(GString *library_key);
GHashTable *get_action_table();
GHashTable *get_handle_table();
ActionDelegate get_action_delegate(GString *method, GString *controller, GString *action);


bool default_route_try(struct mg_connection* c, void* ev_data, application_context* ctx) {
    struct mg_http_message *hm = ev_data;
    struct mg_str parts[3];
    bool exitcode = false;
    GString *method = g_string_new_len(hm->method.ptr, hm->method.len);
    method = g_string_ascii_down(method);
    GString *controller;
    GString *action = NULL;
    GList *matches = NULL;
    if (mg_match(hm->uri, mg_str(CONTROLLER_ACTION_ROUTE), parts)) {

        controller = parts[0].len > 0
                              ? g_string_new_len(parts[0].ptr, parts[0].len)
                              : g_string_new("default");

        if (parts[1].len > 0) {
            GString *part1 = g_string_new_len(parts[1].ptr, parts[1].len);
            GMatchInfo *match_info;
            GError *error = NULL;
            GRegex *regex = g_regex_new("\\d+", 0, 0, &error);
            if (error != NULL) {
                MG_ERROR(("Regex error: '%s'", error->message));
                g_error_free(error);
            } else {
                MG_DEBUG(("Checking part1 '%s'", part1->str));
                gboolean m = g_regex_match_full(regex, part1->str, part1->len, 0, 0, &match_info, &error);
                MG_DEBUG((m ? "matching" : "not matching"));
                while (g_match_info_matches(match_info)) {
                    gchar *word = g_match_info_fetch(match_info, 0);
                    MG_INFO(("\t\tmatch: %s", word));
                    matches = g_list_append(matches, word);
                    // g_free(word);
                    action = g_string_new("index");
                    g_match_info_next(match_info, &error);
                }
                ctx->url_matches = matches;
                g_match_info_free(match_info);
                g_regex_unref(regex);
                if (error != NULL) {
                    MG_ERROR(("Regex error: '%s'", error->message));
                    g_error_free(error);
                }
            }
            if (!action) {
                MG_DEBUG(("Action falling back to '%s'", part1->str));
                action = g_string_new_len(part1->str, part1->len);
            }
            g_string_free(part1, FALSE);
        } else {
            action = g_string_new("index");
        }
        /*
                    action = parts[1].len > 0
                          ? g_string_new_len(parts[1].ptr, parts[1].len)
                          : g_string_new("index");
        */
        MG_INFO(("\t\t(%s) / (%s)", controller->str, action->str));

    } else if (mg_match(hm->uri, mg_str(DEFAULT_ROUTE), NULL)) {

        MG_INFO(("Default match: %s", DEFAULT_ROUTE));
        controller = g_string_new("default");
        action = g_string_new("index");

    } else {

        exitcode = false;
        goto exit;

    }

    ActionDelegate action_delegate = get_action_delegate(method, controller, action);
    if (action_delegate == NULL) {
        exitcode = false;
        goto exitwithcleanup;
    }

    action_delegate(c, ev_data, ctx);

    exitcode = true;

exitwithcleanup:

    g_string_free(controller, TRUE);
    g_string_free(action, TRUE);

exit:

    return exitcode;
}


ActionDelegate get_action_delegate(GString *method, GString *controller, GString *action) {

    GString *library_key = g_string_new("");
    g_string_append_printf(library_key, "./build/lib%s-controller.so", controller->str);

    GString *action_key = g_string_new("");
    g_string_append_printf(action_key, "%s:%s@%s", method->str, action->str, controller->str);

    GString *action_name = g_string_new("");
    g_string_append_printf(action_name, "%s_%s_action", method->str, action->str);

    ActionDelegate action_delegate = NULL;

    action_delegate = g_hash_table_lookup(get_action_table(), action_key->str);
    if (action_delegate) {
        MG_DEBUG(("\t\t - action '%s' found in lookup table", action_key->str));
        g_string_free(action_key, FALSE);
        goto exitwithcleanup;
    }

    void *handle = get_handle(library_key);
    if (!handle) {
        MG_DEBUG(("\t\t - library '%s' not found", library_key->str));
        g_string_free(library_key, TRUE);
        g_string_free(action_key, TRUE);
        goto exitwithcleanup;
    }

    action_delegate = dlsym(handle, action_name->str);
    if (!action_delegate) {
        MG_DEBUG(("\t\t - action '%s' not found", action_name->str));
        g_string_free(library_key, FALSE);
        g_string_free(action_key, TRUE);
        goto exitwithcleanup;
    } else {
        MG_DEBUG(("\t\t - action '%s' loaded successfully", action_key->str));
    }

    g_hash_table_insert(get_action_table(), action_key->str, action_delegate);

    g_string_free(library_key, FALSE);
    g_string_free(action_key, FALSE);

    goto exitwithcleanup;

exitwithcleanup:

    g_string_free(action_name, TRUE);
    return action_delegate;
}

void *get_handle(GString* library_key) {
    void *handle = g_hash_table_lookup(get_handle_table(), library_key);
    if (!handle) {
        handle = dlopen(library_key->str, RTLD_LAZY);
        if (handle) {
            g_hash_table_insert(get_handle_table(), library_key, handle);
        }
    }
    return handle;
}

GHashTable *get_handle_table() {
    static GHashTable *handle_table = NULL;
    if (!handle_table) {
        handle_table = g_hash_table_new(g_str_hash, g_str_equal);
    }
    MG_DEBUG(("handle_table: %p", handle_table));
    return handle_table;
}

GHashTable *get_action_table() {
    static GHashTable *action_table = NULL;
    if (!action_table) {
        action_table = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    }
    MG_DEBUG(("action_table: %p", action_table));
    return action_table;
}

void log_key(gpointer key, gpointer value, gpointer user_data) {
    MG_DEBUG(("\t\t\t - '%s'", (gchar *)key));
}
