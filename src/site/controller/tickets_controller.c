#include "tickets_controller.h"

#include "../render/layout.h"

#define SCRIPT_TEMPALTE "<script type=\"module\" src=\"/tickets/%s\"></script>"
#define ID_SCRIPT_TEMPLATE "<script type=\"application/javascript\">window.ticketId = '%s';</script><script type=\"module\" src=\"/tickets/%s\"></script>"

void render_script_template(GString* render, const char* src, const char *id);

void post_index_action(struct mg_connection* c, void* ev_data, application_context* ctx) {
    mg_http_reply(c, 200, "Content-Type: text/html\r\n", "%s", "post index");
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


void get_create_action(struct mg_connection* c, void* ev_data, application_context* ctx) {
    mg_http_reply(c, 200, "Content-Type: text/html\r\n", "%s", "get create");
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
