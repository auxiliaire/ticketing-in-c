#include "route.h"
#include <glib.h>
#include "../../data/tickets.h"
#include "json-c/json_object.h"
#include "mustach/mustach.h"
#include "mustach/mustach-json-c.h"
#include <dlfcn.h>
#include <ctype.h>


void log_key(gpointer key, gpointer value, gpointer user_data);
void *get_handle(gchar *library_key);
GHashTable *get_action_table();
GHashTable *get_handle_table();
ActionDelegate get_action_delegate(GString *method, GString *controller, GString *action);
gchar *get_parameterized_action(const GString *method);


// TODO: factor dynamic loading out
bool default_route_try(struct mg_connection* c, void* ev_data, application_context* ctx) {
    struct mg_http_message *hm = ev_data;
    struct mg_str parts[3];
    bool exitcode = false;
    GString *method = g_string_new_len(hm->method.ptr, hm->method.len);
    method = g_string_ascii_down(method);
    GString *controller = g_string_new("");
    GString *action = g_string_new("");

    if (mg_match(hm->uri, mg_str(CONTROLLER_ACTION_ROUTE), parts)) {

        controller = parts[0].len > 0
                              ? g_string_append_len(controller, parts[0].ptr, parts[0].len)
                              : g_string_append(controller, "default");

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
                reset_url_matches(ctx);
                while (g_match_info_matches(match_info)) {
                    gchar *word = g_match_info_fetch(match_info, 0);
                    MG_INFO(("\t\tmatch: %s", word));
                    ctx->url_matches = g_list_append(ctx->url_matches, word);
                    action = g_string_append(action, get_parameterized_action(method));
                    g_match_info_next(match_info, &error);
                }
                g_match_info_free(match_info);
                g_regex_unref(regex);
                if (error != NULL) {
                    MG_ERROR(("Regex error: '%s'", error->message));
                    g_error_free(error);
                }
            }
            if (!action) {
                MG_DEBUG(("Action falling back to '%s'", part1->str));
                action = g_string_append_len(action, part1->str, part1->len);
            }
            g_string_free(part1, TRUE);
        } else {
            action = g_string_append(action, "index");
        }
        /*
                    action = parts[1].len > 0
                          ? g_string_new_len(parts[1].ptr, parts[1].len)
                          : g_string_new("index");
        */
        MG_INFO(("\t\t(%s) / (%s)", controller->str, action->str));

    } else if (mg_match(hm->uri, mg_str(DEFAULT_ROUTE), NULL)) {

        MG_INFO(("Default match: %s", DEFAULT_ROUTE));
        controller = g_string_append(controller, "default");
        action = g_string_append(action, "index");

    } else {

        exitcode = false;
        goto exit;

    }

    ActionDelegate action_delegate = get_action_delegate(method, controller, action);
    if (action_delegate == NULL) {
        exitcode = false;
        goto exit;
    }

    action_delegate(c, ev_data, ctx);

    exitcode = true;

exit:

    g_string_free(method, TRUE);
    g_string_free(controller, TRUE);
    g_string_free(action, TRUE);

    return exitcode;
}


gchar *get_parameterized_action(const GString* method) {
    gchar *action = NULL;
    GString *mtest = g_string_new("get");
    if (g_string_equal(method, mtest)) {
        action = (gchar*)"show";
    } else {
        g_string_assign(mtest, "put");
        if (g_string_equal(method, mtest)) {
            action = (gchar*)"update";
        } else {
            g_string_assign(mtest, "delete");
            if (g_string_equal(method, mtest)) {
                action = (gchar*)"one";
            }
        }
    }
    g_string_free(mtest, TRUE);
    return action;
}


ActionDelegate get_action_delegate(GString *method, GString *controller, GString *action) {

    GString *action_key = g_string_new("");
    g_string_append_printf(action_key, "%s:%s@%s", method->str, action->str, controller->str);

    GString *action_name = g_string_new("");
    g_string_append_printf(action_name, "%s_%s_action", method->str, action->str);

    ActionDelegate action_delegate = NULL;

    action_delegate = g_hash_table_lookup(get_action_table(), action_key->str);
    if (action_delegate) {
        MG_DEBUG(("\t\t - action '%s' found in lookup table", action_key->str));
        g_string_free(action_key, TRUE);
        goto exitwithcleanup;
    }

    GString *library_key = g_string_new("");
    g_string_append_printf(library_key, "./build/lib%s-controller.so", controller->str);

    void *handle = get_handle(library_key->str);
    if (!handle) {
        MG_DEBUG(("\t\t - library '%s' not found", library_key->str));
        g_string_free(library_key, FALSE);
        g_string_free(action_key, TRUE);
        goto exitwithcleanup;
    } else {
        g_string_free(library_key, FALSE);
    }

    action_delegate = dlsym(handle, action_name->str);
    if (!action_delegate) {
        MG_DEBUG(("\t\t - action '%s' not found", action_name->str));
        g_string_free(action_key, TRUE);
        goto exitwithcleanup;
    } else {
        MG_DEBUG(("\t\t - action '%s' loaded successfully", action_key->str));
    }

    g_hash_table_insert(get_action_table(), action_key->str, action_delegate);

    g_string_free(action_key, FALSE);

    goto exitwithcleanup;

exitwithcleanup:

    g_string_free(action_name, TRUE);
    return action_delegate;
}

void *get_handle(gchar *library_key) {
    void *handle = g_hash_table_lookup(get_handle_table(), library_key);
    if (!handle) {
        handle = dlopen(library_key, RTLD_LAZY);
        if (handle) {
            g_hash_table_insert(get_handle_table(), library_key, handle);
        } else {
            g_free(library_key);
        }
    } else {
        g_free(library_key);
    }
    return handle;
}

void handle_close(void *handle) {
    int r = dlclose(handle);
    if (r != 0) {
        g_warning("Unable to close handle (code: %d).", r);
    }
}

GHashTable *get_handle_table() {
    static GHashTable *handle_table = NULL;
    if (!handle_table) {
        handle_table = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, handle_close);
    }
    MG_DEBUG(("handle_table: %p", handle_table));
    return handle_table;
}

GHashTable *get_action_table() {
    static GHashTable *action_table = NULL;
    if (!action_table) {
        action_table = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
    }
    MG_DEBUG(("action_table: %p", action_table));
    return action_table;
}

void log_key(gpointer key, gpointer value, gpointer user_data) {
    MG_DEBUG(("\t\t\t - '%s'", (gchar *)key));
}

void default_route_cleanup() {
    GHashTable *handle_table = get_handle_table();
    GHashTable *action_table = get_action_table();
    g_hash_table_remove_all(handle_table);
    g_hash_table_destroy(handle_table);
    g_hash_table_remove_all(action_table);
    g_hash_table_destroy(action_table);
}

