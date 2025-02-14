#include "default_controller.h"

#include <glib.h>
#include "../render/layout.h"
#include "../render/script.h"
#include "../../data/user/user.h"
#include "../../aux/auth.h"
#include "../../aux/content_negotiation.h"

static void handle_error(GError*);

void get_index_action(struct mg_connection *c, void *ev_data, application_context *ctx) {
    struct mg_http_message *hm = ev_data;

    gchar *filename, *content;
    unsigned long length;
    GError *error = NULL;

    g_info("Home dir: %s", g_get_home_dir());
    filename = g_build_filename(g_get_current_dir(), "/view/main.html", NULL);

    gboolean loaded = g_file_get_contents(filename, &content, &length, &error);
    if (!loaded) {
        handle_error(error);
        GString *s = g_string_new("Could not load content");
        content = g_string_free_and_steal(s);
    }

    LayoutModel layout_model = {
        .layout_filename = ctx->layout->str,
        .title = "Welcome",
        .content = content,
        .script = "<!-- No scripts this time -->"
    };

    GString *render = render_layout(&layout_model);
    if (render->len > 0) {
        mg_http_reply(c, 200, "Content-Type: text/html\r\n", "%s", render->str, render->len);
    } else {
        struct mg_http_serve_opts opts = {0};
        opts.root_dir = ".";
        opts.ssi_pattern = "*.html";
        mg_http_serve_file(c, hm, "view/500.html", &opts);
    }
    g_string_free(render, TRUE);
    g_free(content);
}

void get_login_json_action(struct mg_connection* c, void* ev_data, application_context* ctx) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    void *user = get_user(hm, ctx);
    if (user == NULL) {
        MG_INFO(("%s", "Unauthenticated user"));
        return mg_http_reply(c, 403, "", "Denied\n");
    } else {
        MG_INFO(("User found: %s", ((User*)user)->username->str));
        mg_http_reply(c, 200, "Content-Type: application/json\r\n",
                    "{%m:%m,%m:%m}\n", MG_ESC("user"), MG_ESC(((User*)user)->username->str),
                    MG_ESC("token"), MG_ESC(((User*)user)->username->str));
        user_delete(user);
    }
}

void get_login_html_action(struct mg_connection* c, void* ev_data, application_context* ctx) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;

    GString *script = g_string_new("");
    render_script_template(script, "/default/login.js", NULL);
    LayoutModel layout_model = {
        .layout_filename = ctx->layout->str,
        .title = "Login",
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

void get_login_action(struct mg_connection* c, void* ev_data, application_context* ctx) {

    cn_switch_json_html(c, ev_data, ctx,

        get_login_json_action,

        get_login_html_action
    );
}


static void handle_error(GError *error) {
    if (error != NULL) {
        g_warning("File read error: %s", error->message);
        g_clear_error(&error);
    }
}
