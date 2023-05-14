#ifndef TICKETING_TICKETS_H
#define TICKETING_TICKETS_H

#include "db_support.h"

#include "ticket/ticket.h"

GString *tickets_fetch_all(sqlite3 *db);
GString *tickets_fetch_one(sqlite3 *db, sqlite3_int64 id);
int tickets_delete_one(sqlite3 *db, sqlite3_int64 id);
int tickets_upsert_one(sqlite3 *db, Ticket *ticket);

#endif
