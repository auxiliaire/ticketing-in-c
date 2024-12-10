#include <stdio.h>

#include "user.h"

void *user_new() {
    User *user = g_malloc(sizeof(User));
    user->id = 0;
    user->public_id = NULL;
    user->username = NULL;
    user->password = NULL;
    user->token = NULL;
    user->token_expiry = NULL;
    return user;
}

void user_delete(User* user) {
    if (user == NULL) {
        fprintf(stderr, "Possible double free attempt @user_delete\n");
        return;
    }
    if (user->public_id != NULL) {
        g_string_free(user->public_id, TRUE);
        user->public_id = NULL;
    }
    if (user->username != NULL) {
        g_string_free(user->username, TRUE);
        user->username = NULL;
    }
    if (user->password != NULL) {
        g_string_free(user->password, TRUE);
        user->password = NULL;
    }
    if (user->token != NULL) {
        g_string_free(user->token, TRUE);
        user->token = NULL;
    }
    if (user->token_expiry != NULL) {
        g_string_free(user->token_expiry, TRUE);
        user->token_expiry = NULL;
    }
    g_free(user);
}

void user_set_value(void *object, size_t index, void *value) {
    User *user = (User*) object;
    int64_t *i;
    double *d;
    switch (index) {
        case USER_ID:
            i = value;
            user->id = *i;
            break;
        case PUBLIC_ID:
            user->public_id = (GString*) value;
            break;
        case USERNAME:
            user->username = (GString*) value;
            break;
        case PASSWORD:
            user->password = (GString*) value;
            break;
        case TOKEN:
            user->token = (GString*) value;
            break;
        case TOKEN_EXPIRY:
            user->token_expiry = (GString*) value;
            break;
    }
}

int64_t user_get_id(User *user) {
    return user->id;
}

GString *user_get_public_id(User *user) {
    return user->public_id;
}

GString *user_get_username(User *user) {
    return user->username;
}

GString *user_get_password(User *user) {
    return user->password;
}

GString *user_get_token(User *user) {
    return user->token;
}

GString *user_get_token_expiry(User *user) {
    return user->token_expiry;
}

void user_set_id(User *user, int64_t id) {
    user_set_value(user, USER_ID, (void*)&id);
}

void user_set_title(User *user, GString *public_id) {
    user_set_value(user, PUBLIC_ID, (void*)public_id);
}

void user_set_username(User *user, GString *username) {
    user_set_value(user, USERNAME, (void*)username);
}

void user_set_password(User *user, GString *password) {
    user_set_value(user, PASSWORD, (void*)password);
}

void user_set_token(User *user, GString *token) {
    user_set_value(user, TOKEN, (void*)token);
}

void user_set_token_expiry(User *user, GString *token_expiry) {
    user_set_value(user, TOKEN_EXPIRY, (void*)token_expiry);
}
