#ifndef TICKETING_DB_SUPPORT_H
#define TICKETING_DB_SUPPORT_H

#include <sqlite3.h>
#include <glib.h>

#include "data_access.h"

typedef void *(*allocator_fn)();
typedef void (*setter_fn)(void *object, size_t index, void *value);

GList *fetch_as_list(sqlite3_stmt *ppStmt, allocator_fn allocator, setter_fn setter);
GString *fetch_as_json(sqlite3_stmt *ppStmt);

#endif
