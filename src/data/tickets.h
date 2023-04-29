#ifndef TICKETING_TICKETS_H
#define TICKETING_TICKETS_H

#include "db_support.h"

#define TICKETS_FETCH_ALL "SELECT * FROM tickets WHERE 1;"
#define TICKETS_FETCH_ONE "SELECT * FROM tickets WHERE id = ?;"

GString *tickets_fetch_all(sqlite3 *db);
GString *tickets_fetch_one(sqlite3 *db, sqlite3_int64 id);

#endif
