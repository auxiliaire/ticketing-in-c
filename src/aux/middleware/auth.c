#include "auth.h"
#include "../auth.h"

gboolean guard_auth_html(struct mg_connection* c, struct mg_http_message *hm, application_context *ctx) {
    User *user = get_user(hm, ctx);
    if (user == NULL) {
        MG_INFO(("%s", "Unauthenticated user"));
        mg_http_reply(c, 302, "Location: /login\r\n", "");
        return TRUE;
    } else {
        MG_INFO(("User found: %s", ((User*)user)->username->str));
        user_delete(user);
        return FALSE;
    }
}

gboolean guard_auth_json(struct mg_connection* c, struct mg_http_message *hm, application_context *ctx) {
    void *user = get_user(hm, ctx);
    if (user == NULL) {
        MG_INFO(("%s", "Unauthenticated user"));
        mg_http_reply(c, 403, "", "Denied\n");
        return TRUE;
    } else {
        MG_INFO(("User found: %s", ((User*)user)->username->str));
        user_delete(user);
        return FALSE;
    }
}

