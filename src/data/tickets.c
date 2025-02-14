#include <stdio.h>

#include "tickets.h"

#define TICKETS_FETCH_ALL "SELECT t.id, t.title, t.description, t.created_at, u.public_id AS 'created_by:public_id', u.username AS 'created_by:username', t.score FROM tickets t LEFT JOIN users u ON (t.created_by = u.id) WHERE 1;"
#define TICKETS_FETCH_ONE "SELECT * FROM tickets WHERE id = ?;"
#define TICKETS_INSERT_ONE "INSERT INTO tickets (id, title, description, created_at, created_by, score) VALUES (:i,:t,:d,:a,:b,:s) ON CONFLICT(id) DO UPDATE SET title=:t,description=:d,score=:s WHERE id=:i;"
#define TICKETS_DELETE_ONE "DELETE FROM tickets WHERE id=:i LIMIT 1;"

GString *tickets_fetch_all(sqlite3 *db) {
    sqlite3_stmt *ppStmt;
    int rc = sqlite3_prepare_v2(db, TICKETS_FETCH_ALL, -1, &ppStmt, NULL);

    if (rc != SQLITE_OK) {
        const char *zErrMsg = sqlite3_errstr(rc);
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        return g_string_new("[]");
    }
    return fetch_as_json(ppStmt);
}

GString *tickets_fetch_one(sqlite3 *db, sqlite3_int64 id) {
    sqlite3_stmt *ppStmt;
    int rc = sqlite3_prepare_v2(db, TICKETS_FETCH_ONE, -1, &ppStmt, NULL);

    if (rc != SQLITE_OK) {
        const char *zErrMsg = sqlite3_errstr(rc);
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        return g_string_new("[]");
    }

    rc = sqlite3_bind_int64(ppStmt, 1, id);

    if (rc != SQLITE_OK) {
        const char *zErrMsg = sqlite3_errstr(rc);
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        return g_string_new("[]");
    }

    return fetch_as_json(ppStmt);
}

int tickets_delete_one(sqlite3 *db, sqlite3_int64 id) {
    sqlite3_stmt *ppStmt;
    int rc = sqlite3_prepare_v2(db, TICKETS_DELETE_ONE, -1, &ppStmt, NULL);

    if (rc != SQLITE_OK) {
        const char *zErrMsg = sqlite3_errstr(rc);
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        return 0;
    }

    rc = sqlite3_bind_int64(ppStmt, 1, id);
    rc = sqlite3_step(ppStmt);

    if (rc != SQLITE_DONE) {
        const char *zErrMsg = sqlite3_errstr(rc);
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        return 0;
    }

    sqlite3_finalize(ppStmt);

    return 1;
}

int tickets_upsert_one(sqlite3* db, Ticket *ticket) {
    sqlite3_stmt *ppStmt;
    int rc = sqlite3_prepare_v2(db, TICKETS_INSERT_ONE, -1, &ppStmt, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error at prepare\n");
        goto error;
    }

    if (ticket->id != 0) {
        rc = sqlite3_bind_int64(ppStmt, sqlite3_bind_parameter_index(ppStmt, ":i"), ticket->id);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error at id\n");
            goto error;
        }
    }
    rc = sqlite3_bind_text(ppStmt, sqlite3_bind_parameter_index(ppStmt, ":t"), ticket->title->str, ticket->title->len, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error at title\n");
        goto error;
    }
    rc = sqlite3_bind_text(ppStmt, sqlite3_bind_parameter_index(ppStmt, ":d"), ticket->description->str, ticket->description->len, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error at description\n");
        goto error;
    }
    if (ticket->created_at != NULL) {
        rc = sqlite3_bind_text(ppStmt, sqlite3_bind_parameter_index(ppStmt, ":a"), ticket->created_at->str, ticket->created_at->len, SQLITE_STATIC);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error at created_at\n");
            goto error;
        }
    }
    if (ticket->created_by != 0) {
        rc = sqlite3_bind_int64(ppStmt, sqlite3_bind_parameter_index(ppStmt, ":b"), ticket->created_by);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error at created_by\n");
            goto error;
        }
    }
    rc = sqlite3_bind_double(ppStmt, sqlite3_bind_parameter_index(ppStmt, ":s"), ticket->score);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error at score\n");
        goto error;
    }

    rc = sqlite3_step(ppStmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error at step\n");
        goto error;
    }

    sqlite3_finalize(ppStmt);

    return 1;

error:
    fprintf(stderr, "SQL error: %d - '%s'\n", rc, sqlite3_errmsg(db));

    return 0;
}

