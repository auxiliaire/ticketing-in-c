#include "tickets_controller.h"

#include "../render/layout.h"
#include "../../data/tickets.h"

#define SCRIPT_TEMPALTE "<script type=\"module\" src=\"/tickets/%s\"></script>"
#define ID_SCRIPT_TEMPLATE "<script type=\"application/javascript\">window.ticketId = '%s';</script><script type=\"module\" src=\"/tickets/%s\"></script>"

void render_script_template(GString* render, const char* src, const char *id);

void post_index_action(struct mg_connection* c, void* ev_data, application_context* ctx) {

    struct mg_http_message *hm = ev_data;

    MG_INFO(("Body: '%.*s'", (int) hm->body.len, hm->body.ptr));

    char _id[16] = "";
    char *id;
    bool new = false;
    int id_len = mg_http_get_var(&hm->body, "id", _id, sizeof(_id));
    if (id_len < 0) goto error;
    if (id_len == 0) {
        id_len = 4;
        id = "null";
        new = true;
    } else {
        id = _id;
    }

    char title[255] = "";
    int title_len = mg_http_get_var(&hm->body, "title", title, sizeof(title));
    if (title_len <= 0) goto error;

    char score[16] = "";
    int score_len = mg_http_get_var(&hm->body, "score", score, sizeof(score));
    if (score_len <= 0) goto error;

    char desc[500] = "";
    int desc_len = mg_http_get_var(&hm->body, "description", desc, sizeof(desc));
    if (desc_len <= 0) goto error;

    MG_INFO(("\tId: \t\t'%.*s'", id_len, id));
    MG_INFO(("\tTitle: \t\t'%.*s'", title_len, title));
    MG_INFO(("\tScore: \t\t'%.*s'", score_len, score));
    MG_INFO(("\tDescription: \t'%.*s'", desc_len, desc));

    GString *sql = g_string_new("");
    g_string_printf(sql, "INSERT INTO tickets (id, title, description, created_at, created_by, score) VALUES (%.*s,'%.*s','%.*s','2023-05-13 18:23:00',1,%.*s) ON CONFLICT(id) DO UPDATE SET title='%.*s',description='%.*s',score=%.*s WHERE id=%.*s;",
                    id_len,     id,
                    title_len,  title,
                    desc_len,   desc,
                    score_len,  score,
                    title_len,  title,
                    desc_len,   desc,
                    score_len,  score,
                    id_len,     id
                );
    MG_INFO(("SQL: '%.*s'", sql->len, sql->str));

    int res = tickets_upsert_one(ctx->db, sql->str, sql->len);

    MG_INFO(("\t%d inserted", res));

    GString *location = g_string_new("Location: /tickets/");
    if (!new)
        g_string_append_printf(location, "%.*s", id_len, id);
    g_string_append(location, "\r\n");

    reset_url_matches(ctx);

    mg_http_reply(c, 302, location->str, "");

    g_string_free(sql, TRUE);
    g_string_free(location, TRUE);
    return;
error:
    mg_http_reply(c, 200, "Content-Type: text/html\r\n", "%s", "post index, malformed data");
}

void get_index_action(struct mg_connection* c, void* ev_data, application_context* ctx) {

    struct mg_http_message *hm = ev_data;

    GList *param;
    for (param = ctx->url_matches; param != NULL; param = param->next) {
        MG_INFO(("\t\tparam: %s", param->data));
        return get_show_action(c, ev_data, ctx);
    }
    g_list_free(param);
    reset_url_matches(ctx);

    GString *script = g_string_new("");
    render_script_template(script, "index.js", NULL);
    LayoutModel layout_model = {
        .layout_filename = ctx->layout->str,
        .title = "Tickets",
        .content = "",
        .script = script->str
    };

    GString *render = render_layout(&layout_model);
    if (render->len > 0) {
        mg_http_reply(c, 200, "Content-Type: text/html\r\n", "%s", render->str, render->len);
    } else {
        struct mg_http_serve_opts opts = {0};
        opts.root_dir = ".";
        opts.ssi_pattern = "#.shtml";
        mg_http_serve_file(c, hm, "view/500.html", &opts);
    }
    g_string_free(script, TRUE);
    g_string_free(render, TRUE);
}

void put_index_action(struct mg_connection* c, void* ev_data, application_context* ctx) {
    mg_http_reply(c, 200, "Content-Type: text/html\r\n", "%s", "put index");
}

void delete_index_action(struct mg_connection* c, void* ev_data, application_context* ctx) {
    mg_http_reply(c, 200, "Content-Type: text/html\r\n", "%s", "delete index");
}


void get_new_action(struct mg_connection* c, void* ev_data, application_context* ctx) {
    struct mg_http_message *hm = ev_data;
    GString *id = g_string_new("");
    GString *script = g_string_new("");
    render_script_template(script, "create.js", NULL);
    LayoutModel layout_model = {
        .layout_filename = ctx->layout->str,
        .title = "Create New Ticket",
        .content = "",
        .script = script->str
    };

    GString *render = render_layout(&layout_model);
    mg_http_reply(c, 200, "Content-Type: text/html\r\n", "%s", render->str, render->len);
    g_string_free(render, TRUE);
    g_string_free(script, TRUE);
    g_string_free(id, TRUE);
}

void get_show_action(struct mg_connection* c, void* ev_data, application_context* ctx) {

    struct mg_http_message *hm = ev_data;

    GString *script = g_string_new("");
    GList *param;
    for (param = ctx->url_matches; param != NULL; param = param->next) {
        MG_INFO(("\t\tparam: %s", param->data));
        render_script_template(script, "show.js", (char*)param->data);
    }
    g_list_free(param);
    reset_url_matches(ctx);

    LayoutModel layout_model = {
        .layout_filename = ctx->layout->str,
        .title = "Tickets Show",
        .content = "",
        .script = script->str
    };

    GString *render = render_layout(&layout_model);
    if (render->len > 0) {
        mg_http_reply(c, 200, "Content-Type: text/html\r\n", "%s", render->str, render->len);
    } else {
        struct mg_http_serve_opts opts = {0};
        opts.root_dir = ".";
        opts.ssi_pattern = "#.shtml";
        mg_http_serve_file(c, hm, "view/500.html", &opts);
    }
    g_string_free(script, TRUE);
    g_string_free(render, TRUE);
}

void get_edit_action(struct mg_connection* c, void* ev_data, application_context* ctx) {
    struct mg_http_message *hm = ev_data;
    int res;
    GString *id = g_string_new("");
    GString *script = g_string_new("");
    if (ctx->url_matches != NULL) {
        /* Path Id */
        GList *param;
        for (param = ctx->url_matches; param != NULL; param = param->next) {
            MG_INFO(("\t\tparam: %s", param->data));
            g_string_printf(id, "%s", (gchar*)param->data);
            render_script_template(script, "edit.js", (char*)param->data);
        }
        g_list_free(param);
        reset_url_matches(ctx);
    } else {
        /* Query Id */
        char buf[80] = "";
        struct mg_str idval = mg_http_var(hm->query, mg_str("id"));
        MG_INFO(("\t\tfound Id of length %d", idval.len));
        res = mg_http_get_var(&hm->query, "id", buf, sizeof(buf));
        if (res > 0) {
            id = g_string_append_len(id, buf, res);
            render_script_template(script, "edit.js", id->str);
        }
    }
    if (res <= 0) {
        MG_ERROR(("Error decoding var 'id': %d", res));
        struct mg_http_serve_opts opts = {0};
        opts.root_dir = ".";
        opts.ssi_pattern = "#.shtml";
        mg_http_serve_file(c, hm, "view/404.html", &opts);
    } else {
        LayoutModel layout_model = {
            .layout_filename = ctx->layout->str,
            .title = "Tickets Show",
            .content = "",
            .script = script->str
        };

        GString *render = render_layout(&layout_model);
        mg_http_reply(c, 200, "Content-Type: text/html\r\n", "%s", render->str, render->len);
        g_string_free(render, TRUE);
    }
    g_string_free(script, TRUE);
    g_string_free(id, TRUE);
}

void put_update_action(struct mg_connection* c, void* ev_data, application_context* ctx) {
    mg_http_reply(c, 200, "Content-Type: text/html\r\n", "%s", "put update");
}

void post_update_action(struct mg_connection* c, void* ev_data, application_context* ctx) {
    mg_http_reply(c, 200, "Content-Type: text/html\r\n", "%s", "post update");
}

void post_delete_action(struct mg_connection* c, void* ev_data, application_context* ctx) {

    struct mg_http_message *hm = ev_data;

    MG_INFO(("Body: '%.*s'", (int) hm->body.len, hm->body.ptr));

    char id[16] = "";
    int id_len = mg_http_get_var(&hm->body, "id", id, sizeof(id));
    if (id_len <= 0) goto error;

    MG_INFO(("\tId: \t\t'%.*s'", id_len, id));

    GString *sql = g_string_new("");
    g_string_printf(sql, "DELETE FROM tickets WHERE id=%.*s LIMIT 1;", id_len, id);
    MG_INFO(("SQL: '%.*s'", sql->len, sql->str));

    int res = tickets_upsert_one(ctx->db, sql->str, sql->len);

    MG_INFO(("\t%d deleted", res));

    reset_url_matches(ctx);

    mg_http_reply(c, 302, "Location: /tickets\r\n", "");

    g_string_free(sql, TRUE);
    return;
error:
    mg_http_reply(c, 200, "Content-Type: text/html\r\n", "%s", "post delete, malformed data");
}

void delete_one_action(struct mg_connection* c, void* ev_data, application_context* ctx) {
    mg_http_reply(c, 200, "Content-Type: text/html\r\n", "%s", "delete one");
}

void render_script_template(GString* render, const char* src, const char* id) {
    if (id == NULL) {
        g_string_printf(
            render,
            SCRIPT_TEMPALTE,
            src
        );
    } else {
        g_string_printf(
            render,
            ID_SCRIPT_TEMPLATE,
            id,
            src
        );

    }
}
