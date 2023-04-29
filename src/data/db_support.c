#include "db_support.h"

GList *fetch_as_list(sqlite3_stmt *ppStmt, allocator_fn allocator, setter_fn setter) {
    int rc = sqlite3_step(ppStmt);
    int ncols = sqlite3_column_count(ppStmt);
    int current = 0;
    GList *list = NULL;
    while (rc == SQLITE_ROW) {
        void *object = allocator();
        for (int i = 0; i < ncols; i++) {
            int colType = sqlite3_column_type(ppStmt, i);
            int *ival;
            double *dval;
            switch (colType) {
            case SQLITE_INTEGER:
                ival = g_malloc(sizeof(int));
                *ival = sqlite3_column_int(ppStmt, i);
                setter(object, i, ival);
                break;
            case SQLITE_FLOAT:
                dval = g_malloc(sizeof(double));
                *dval = sqlite3_column_double(ppStmt, i);
                setter(object, i, dval);
                break;
            case SQLITE_NULL:
                setter(object, i, NULL);
                break;
            default: /* TEXT | BLOB */
                setter(object, i, g_string_new((gchar*) sqlite3_column_text(ppStmt, i)));
            }
        }
        list = g_list_append(list, object);
        rc = sqlite3_step(ppStmt);
        current++;
    }
    sqlite3_finalize(ppStmt);
    return list;
}

GString *fetch_as_json(sqlite3_stmt *ppStmt) {
    // Open bracket:
    GString *json = g_string_new("[");

    int rc = sqlite3_step(ppStmt);
    int ncols = sqlite3_column_count(ppStmt);
    int current = 0;
    while (rc == SQLITE_ROW) {
        if (current > 0) g_string_append_c(json, ',');
        g_string_append_c(json, '{');
        for (int i = 0; i < ncols; i++) {
            if (i > 0) g_string_append_c(json, ',');
            const char *colName = sqlite3_column_name(ppStmt, i);
            int colType = sqlite3_column_type(ppStmt, i);
            switch (colType) {
            case SQLITE_INTEGER:
                g_string_append_printf(json, "\"%s\":%d", colName, sqlite3_column_int(ppStmt, i));
                break;
            case SQLITE_FLOAT:
                g_string_append_printf(json, "\"%s\":%lf", colName, sqlite3_column_double(ppStmt, i));
                break;
            case SQLITE_NULL:
                g_string_append_printf(json, "\"%s\":null", colName);
                break;
            default: /* TEXT | BLOB */
                g_string_append_printf(json, "\"%s\":\"%s\"", colName, sqlite3_column_text(ppStmt, i));
            }
        }
        g_string_append_c(json, '}');
        rc = sqlite3_step(ppStmt);
        current++;
    }
    // Close bracket:
    g_string_append_c(json, ']');

    sqlite3_finalize(ppStmt);

    return json;
}