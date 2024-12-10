#include "auth.h"

#include "../data/user/user.h"

User *get_user(struct mg_http_message *hm, application_context *ctx) {
    char user[256], pass[256];
    User *u = NULL;
    mg_http_creds(hm, user, sizeof(user), pass, sizeof(pass));
    if (user[0] != '\0' && pass[0] != '\0') {
        // Both user and password are set, search by user/password
        GString *username = g_string_new(user);
        MG_INFO(("user / pass: '%s' / ***", user));
        u = users_fetch_one(ctx->db, username);
        if (u == NULL) {
            MG_INFO((" \\ user could not be retrieved"));
        } else if (strcmp(pass, u->password->str) != 0) {
            user_delete(u);
            u = NULL;
        }
        g_string_free(username, TRUE);
    } else if (user[0] == '\0') {
        // Only password is set, search by token
        GString *token = g_string_new(pass);
        MG_INFO(("token: '%s'", pass));
        // TODO: implement user_fetch_one_by_token and replace this one:
        if (token->len > 0) {
            u = (User*)users_fetch_one(ctx->db, token);
            if (u == NULL || u->token == NULL || strcmp(pass, u->token->str) != 0) {
                user_delete(u);
                u = NULL;
            }
        } else {
            MG_INFO(("Token not usable."));
        }
        g_string_free(token, TRUE);
    }
    return (void*)u;
}
