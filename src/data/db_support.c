#include "db_support.h"

void *_fetch_as_object(sqlite3_stmt *ppStmt, int ncols, allocator_fn allocator, setter_fn setter);

void *fetch_as_object(sqlite3_stmt *ppStmt, allocator_fn allocator, setter_fn setter) {
    int rc = sqlite3_step(ppStmt);
    int ncols = sqlite3_column_count(ppStmt);
    void *object = NULL;
    if (rc == SQLITE_ROW) {
        object = _fetch_as_object(ppStmt, ncols, allocator, setter);
    }
    sqlite3_finalize(ppStmt);
    return object;
}

GList *fetch_as_list(sqlite3_stmt *ppStmt, allocator_fn allocator, setter_fn setter) {
    int rc = sqlite3_step(ppStmt);
    int ncols = sqlite3_column_count(ppStmt);
    int current = 0;
    GList *list = NULL;
    while (rc == SQLITE_ROW) {
        list = g_list_append(list, _fetch_as_object(ppStmt, ncols, allocator, setter));
        rc = sqlite3_step(ppStmt);
        current++;
    }
    sqlite3_finalize(ppStmt);
    return list;
}

int count_array(const void **array) {
    int count = 0;
    while (array[count] != NULL) {
        count++;
    }
    return count;
}

GString *fetch_as_json(sqlite3_stmt *ppStmt) {
    // Open bracket:
    GString *json = g_string_new("[");
    GString *text = g_string_new("");

    int rc = sqlite3_step(ppStmt);
    int ncols = sqlite3_column_count(ppStmt);
    int current = 0;
    int level = 0;
    char *colAlias;
    while (rc == SQLITE_ROW) {
        if (current > 0) g_string_append_c(json, ',');
        g_string_append_c(json, '{');
        for (int i = 0; i < ncols; i++) {
            const char *colName = sqlite3_column_name(ppStmt, i);
            // Check embedded objects:
            gchar **tokens = g_strsplit(colName, ":", -1);
            int tokenNum = count_array((const void **)tokens);
            if (tokenNum < level + 1) {
                // Close embedded object
                g_string_append(json, "},");
                level--;
            } else if (i > 0) {
                g_string_append_c(json, ',');
            }
            if (tokenNum > level + 1) {
                // Open embedded object
                g_string_append_printf(json, "\"%s\":{", tokens[level]);
                level++;
            }
            if (tokenNum >= level + 1) {
                colAlias = tokens[tokenNum - 1];
            } else {
                colAlias = (char*)colName;
            }
            int colType = sqlite3_column_type(ppStmt, i);
            switch (colType) {
            case SQLITE_INTEGER:
                g_string_append_printf(json, "\"%s\":%d", colAlias, sqlite3_column_int(ppStmt, i));
                break;
            case SQLITE_FLOAT:
                g_string_append_printf(json, "\"%s\":%lf", colAlias, sqlite3_column_double(ppStmt, i));
                break;
            case SQLITE_NULL:
                g_string_append_printf(json, "\"%s\":null", colAlias);
                break;
            default: /* TEXT | BLOB */
                g_string_printf(text, "%s", sqlite3_column_text(ppStmt, i));
                g_string_replace(text, "\r\n", "\\n", 0);
                g_string_replace(text, "\n", "\\n", 0);
                g_string_append_printf(json, "\"%s\":\"%s\"", colAlias, text->str);
            }
        }
        g_string_append_c(json, '}');
        rc = sqlite3_step(ppStmt);
        current++;
    }
    // Close bracket:
    g_string_append_c(json, ']');

    sqlite3_finalize(ppStmt);

    g_string_free(text, TRUE);
    return json;
}

void *_fetch_as_object(sqlite3_stmt *ppStmt, int ncols, allocator_fn allocator, setter_fn setter) {
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
    return object;
}
