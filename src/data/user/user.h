#ifndef TICKETING_USER_H
#define TICKETING_USER_H

#include <glib.h>
#include "data_access.h"

typedef enum user_field {
  USER_ID,
  PUBLIC_ID,
  USERNAME,
  PASSWORD,
  TOKEN,
  TOKEN_EXPIRY
} UserField;

typedef struct user {
  int id;
  GString *public_id;
  GString *username;
  GString *password;
  GString *token;
  GString *token_expiry;
} User;

void *user_new();
void user_delete(User *user);

int user_get_id(User *user);
GString *user_get_public_id(User *user);
GString *user_get_username(User *user);
GString *user_get_password(User *user);
GString *user_get_token(User *user);
GString *user_get_token_expiry(User *user);

void user_set_value(void *object, size_t index, void *value);
void user_set_id(User *user, int id);
void user_set_public_id(User *user, GString *public_id);
void user_set_username(User *user, GString *username);
void user_set_password(User *user, GString *password);
void user_set_token(User *user, GString *token);
void user_set_token_expiry(User *user, GString *token_expiry);

#endif
