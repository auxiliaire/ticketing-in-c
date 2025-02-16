#include "loader.h"

#include <dlfcn.h>

void log_key(gpointer key);
void* get_handle(gchar* library_key);
GHashTable *get_action_table();
GHashTable *get_handle_table();
ActionDelegate get_action_delegate(GString *method, GString *controller, GString *action);

ActionDelegate get_action_delegate(GString *method, GString *controller, GString *action) {

    GString *action_key = g_string_new(NULL);
    g_string_append_printf(action_key, "%s:%s@%s", method->str, action->str, controller->str);

    GString *action_name = g_string_new(NULL);
    g_string_append_printf(action_name, "%s_%s_action", method->str, action->str);

    ActionDelegate action_delegate = NULL;

    action_delegate = g_hash_table_lookup(get_action_table(), action_key->str);
    if (action_delegate) {
        g_debug("\t\t - action '%s' found in lookup table", action_key->str);
        g_string_free(action_key, TRUE);
        goto exitwithcleanup;
    }

    GString *library_key = g_string_new(NULL);
    g_string_printf(library_key, "./build/lib%s-controller.so", controller->str);
    g_debug("\t\t - library '%s'", library_key->str);

    void *handle = get_handle(g_string_free_and_steal(library_key));
    if (!handle) {
        g_debug("\t\t ...not found");
        g_string_free(action_key, TRUE);
        goto exitwithcleanup;
    } else {
        g_debug("\t\t ...found");
    }

    action_delegate = dlsym(handle, action_name->str);
    if (!action_delegate) {
        g_debug("\t\t - action '%s' not found", action_name->str);
        g_string_free(action_key, TRUE);
        goto exitwithcleanup;
    } else {
        g_debug("\t\t - action '%s' loaded successfully", action_key->str);
    }

    g_hash_table_insert(get_action_table(), g_string_free_and_steal(action_key), action_delegate);

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
    // Closing the module prevents ASan from symbolizing the call:
    int r = 0; // dlclose(handle);
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

void log_key(gpointer key) {
    MG_DEBUG(("\t\t\t - '%s'", (gchar *)key));
}

void loader_release() {
    GHashTable *handle_table = get_handle_table();
    GHashTable *action_table = get_action_table();
    g_hash_table_destroy(handle_table);
    g_hash_table_destroy(action_table);
}
