#include <stdio.h>

#include "tickets.h"

GString *tickets_fetch_all(sqlite3 *db) {
    sqlite3_stmt *ppStmt;
    int rc = sqlite3_prepare_v2(db, TICKETS_FETCH_ALL, -1, &ppStmt, NULL);

    if (rc != SQLITE_OK) {
        char *zErrMsg = 0;
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return g_string_new("[]");
    }
    return fetch_as_json(ppStmt);
}

GString *tickets_fetch_one(sqlite3 *db, sqlite3_int64 id) {
    sqlite3_stmt *ppStmt;
    int rc = sqlite3_prepare_v2(db, TICKETS_FETCH_ONE, -1, &ppStmt, NULL);

    if (rc != SQLITE_OK) {
        char *zErrMsg = 0;
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return g_string_new("[]");
    }

    rc = sqlite3_bind_int64(ppStmt, 1, id);

    if (rc != SQLITE_OK) {
        char *zErrMsg = 0;
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return g_string_new("[]");
    }

    return fetch_as_json(ppStmt);
}
