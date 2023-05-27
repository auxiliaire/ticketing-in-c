#include <stdio.h>

#include "users.h"

#define USERS_FETCH_ONE "SELECT * FROM users WHERE username = ?;"

User *users_fetch_one(sqlite3 *db, GString *username) {
    sqlite3_stmt *ppStmt;
    int rc = sqlite3_prepare_v2(db, USERS_FETCH_ONE, -1, &ppStmt, NULL);

    if (rc != SQLITE_OK) {
        char *zErrMsg = 0;
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return NULL;
    }

    rc = sqlite3_bind_text(ppStmt, 1, username->str, username->len, NULL);

    if (rc != SQLITE_OK) {
        char *zErrMsg = 0;
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return NULL;
    }

    return fetch_as_object(ppStmt, user_new, user_set_value);
}

