#ifndef TICKETING_USERS_H
#define TICKETING_USERS_H

#include "db_support.h"

#include "user/user.h"

// GList *users_fetch_all(sqlite3 *db);
User *users_fetch_one(sqlite3 *db, GString *username);
// int users_delete_one(sqlite3 *db, sqlite3_int64 id);
// int users_upsert_one(sqlite3 *db, User *user);

#endif
