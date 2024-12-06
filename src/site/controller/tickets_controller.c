#include "tickets_controller.h"

#include "../render/layout.h"
#include "../render/script.h"
#include "../../data/tickets.h"
#include "../../data/user/user.h"
#include "../../aux/content_negotiation.h"
#include "../../aux/middleware/auth.h"

void post_index_action(struct mg_connection* c, void* ev_data, application_context* ctx) {
    struct mg_http_message *hm = ev_data;

    if (guard_auth_html(c, hm, ctx)) return;

    MG_INFO(("Body: '%.*s'", (int) hm->body.len, hm->body.buf));

    char _id[16] = "";
    char *s_id;
    int64_t id;
    bool new = false;
    int id_len = mg_http_get_var(&hm->body, "id", _id, sizeof(_id));
    if (id_len < 0) goto error;
    if (id_len == 0) {
        id_len = 1;
        s_id = "0";
        new = true;
    } else {
        s_id = _id;
    }
    if (!g_ascii_string_to_signed(s_id, 10, 0, INT64_MAX, &id, NULL)) goto error;

    char title[255] = "";
    int title_len = mg_http_get_var(&hm->body, "title", title, sizeof(title));
    if (title_len <= 0) goto error;

    char score[16] = "";
    int score_len = mg_http_get_var(&hm->body, "score", score, sizeof(score));
    if (score_len <= 0) goto error;

    char desc[500] = "";
    int desc_len = mg_http_get_var(&hm->body, "description", desc, sizeof(desc));
    if (desc_len <= 0) goto error;

    MG_INFO(("\tId: \t\t'%.*s' -> %d", id_len, s_id, id));
    MG_INFO(("\tTitle: \t\t'%.*s'", title_len, title));
    MG_INFO(("\tScore: \t\t'%.*s'", score_len, score));
    MG_INFO(("\tDescription: \t'%.*s'", desc_len, desc));

    Ticket *ticket = ticket_new();
    ticket_set_id(ticket, id);
    ticket_set_title(ticket, g_string_new_len(title, title_len));
    ticket_set_desctiption(ticket, g_string_new_len(desc, desc_len));
    ticket_set_score(ticket, g_strtod(score, NULL));
    // Will be ignored on update:
    GString *datetime = g_string_new(g_date_time_format(g_date_time_new_now_utc(), "%F %T"));
    ticket_set_created_at(ticket, datetime);
    ticket_set_created_by(ticket, 1);

    int res = tickets_upsert_one(ctx->db, ticket);

    MG_INFO(("\t%d inserted", res));

    GString *location = g_string_new("Location: /tickets/");
    if (!new)
        g_string_append_printf(location, "%.*s", id_len, s_id);
    g_string_append(location, "\r\n");

    reset_url_matches(ctx);

    mg_http_reply(c, 302, location->str, "");

    g_string_free(location, TRUE);
    ticket_delete(ticket);
    return;
error:
    mg_http_reply(c, 200, "Content-Type: text/html\r\n", "%s", "post index, malformed data");
}

void get_index_html_action(struct mg_connection* c, void* ev_data, application_context* ctx) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;

    if (guard_auth_html(c, hm, ctx)) return;

    GString *script = g_string_new("");
    render_script_template(script, "/tickets/index.js", NULL);
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

void get_index_json_action(struct mg_connection* c, void* ev_data, application_context* ctx) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;

    if (guard_auth_json(c, hm, ctx)) return;

    GString *json = tickets_fetch_all(ctx->db);
    mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s\n", json->str);
    g_string_free(json, TRUE);
}

void get_index_action(struct mg_connection* c, void* ev_data, application_context* ctx) {
    GList *param;
    for (param = ctx->url_matches; param != NULL; param = param->next) {
        MG_INFO(("\t\tparam: %s", param->data));
        return get_show_action(c, ev_data, ctx);
    }
    g_list_free(param);
    reset_url_matches(ctx);

    cn_switch_json_html(c, ev_data, ctx,

        get_index_json_action,

        get_index_html_action
    );
}

void put_index_action(struct mg_connection* c, void* ev_data, application_context* ctx) {
    mg_http_reply(c, 200, "Content-Type: text/html\r\n", "%s", "put index");
}

void delete_index_action(struct mg_connection* c, void* ev_data, application_context* ctx) {
    mg_http_reply(c, 200, "Content-Type: text/html\r\n", "%s", "delete index");
}


void get_new_action(struct mg_connection* c, void* ev_data, application_context* ctx) {
    struct mg_http_message *hm = ev_data;

    if (guard_auth_html(c, hm, ctx)) return;

    GString *id = g_string_new("");
    GString *script = g_string_new("");
    render_script_template(script, "/tickets/create.js", NULL);
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

void get_show_html_action(struct mg_connection* c, void* ev_data, application_context* ctx) {
    struct mg_http_message *hm = ev_data;

    if (guard_auth_html(c, hm, ctx)) return;

    GString *script = g_string_new("");
    GList *param;
    for (param = ctx->url_matches; param != NULL; param = param->next) {
        MG_INFO(("\t\tparam: %s", param->data));
        render_script_template(script, "/tickets/show.js", (char*)param->data);
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

void get_show_json_action(struct mg_connection* c, void* ev_data, application_context* ctx) {
    struct mg_http_message *hm = ev_data;

    if (guard_auth_json(c, hm, ctx)) return;

    GList *param;
    int64_t id;
    gboolean result;
    GError *error = NULL;
    for (param = ctx->url_matches; param != NULL; param = param->next) {
        result = g_ascii_string_to_signed((char*)(param->data), 10, 1, INT64_MAX, &id, &error);
    }
    g_list_free(param);
    reset_url_matches(ctx);

    if (result) {
        GString *json = tickets_fetch_one(ctx->db, id);
        mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s\n", json->str);
        g_string_free(json, TRUE);
    } else {
        mg_http_reply(c, 400, "Content-Type: application/json\r\n", "{\"code\":%d,\"message\":\"%s\"}\n", error->code, error->message);
        g_error_free(error);
    }
}

void get_show_action(struct mg_connection* c, void* ev_data, application_context* ctx) {

    cn_switch_json_html(c, ev_data, ctx,

        get_show_json_action,

        get_show_html_action
    );
}

void get_edit_action(struct mg_connection* c, void* ev_data, application_context* ctx) {
    struct mg_http_message *hm = ev_data;

    if (guard_auth_html(c, hm, ctx)) return;

    int res;
    GString *id = g_string_new("");
    GString *script = g_string_new("");
    if (ctx->url_matches != NULL) {
        /* Path Id */
        GList *param;
        for (param = ctx->url_matches; param != NULL; param = param->next) {
            MG_INFO(("\t\tparam: %s", param->data));
            g_string_printf(id, "%s", (gchar*)param->data);
            render_script_template(script, "/tickets/edit.js", (char*)param->data);
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
            render_script_template(script, "/tickets/edit.js", id->str);
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

    if (guard_auth_html(c, hm, ctx)) return;

    MG_INFO(("Body: '%.*s'", (int) hm->body.len, hm->body.buf));

    char s_id[16] = "";
    int id_len = mg_http_get_var(&hm->body, "id", s_id, sizeof(s_id));
    if (id_len <= 0) goto error;
    int64_t id;
    if (!g_ascii_string_to_signed(s_id, 10, 1, INT64_MAX, &id, NULL)) goto error;

    MG_INFO(("\tId: \t\t'%.*s' -> %d", id_len, s_id, id));

    int res = tickets_delete_one(ctx->db, id);

    MG_INFO(("\t%d deleted", res));

    reset_url_matches(ctx);

    mg_http_reply(c, 302, "Location: /tickets\r\n", "");

    return;
error:
    mg_http_reply(c, 200, "Content-Type: text/html\r\n", "%s", "post delete, malformed data");
}

void delete_one_action(struct mg_connection* c, void* ev_data, application_context* ctx) {
    mg_http_reply(c, 200, "Content-Type: text/html\r\n", "%s", "delete one");
}
