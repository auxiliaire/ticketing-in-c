#ifndef TICKETING_TICKETS_H
#define TICKETING_TICKETS_H

#include "db_support.h"

#define TICKETS_FETCH_ALL "SELECT * FROM tickets WHERE 1;"
#define TICKETS_FETCH_ONE "SELECT * FROM tickets WHERE id = ?;"
#define TICKETS_INSERT_ONE "INSERT INTO tickets (id, title, description, created_at, created_by, score) VALUES (:i,':t',':d',':a',:b,:s) ON CONFLICT(id) DO UPDATE SET title=':t',description=':d',score=:s WHERE id=:i;"

GString *tickets_fetch_all(sqlite3 *db);
GString *tickets_fetch_one(sqlite3 *db, sqlite3_int64 id);
int tickets_upsert_one(sqlite3 *db, char* sql, int len);

#endif
