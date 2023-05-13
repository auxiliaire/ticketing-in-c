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

int tickets_upsert_one(sqlite3* db, char* sql, int len) {
    sqlite3_stmt *ppStmt;
    int rc = sqlite3_prepare_v2(db, sql, len, &ppStmt, NULL);

    if (rc != SQLITE_OK) {
        char *zErrMsg = 0;
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return 0;
    }

    rc = sqlite3_step(ppStmt);

    if (rc != SQLITE_DONE) {
        char *zErrMsg = 0;
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return 0;
    }

    sqlite3_finalize(ppStmt);

    return 1;
}

