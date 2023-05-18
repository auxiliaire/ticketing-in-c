#include "default_controller.h"

#include <glib.h>
#include "../render/layout.h"

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
        content = s->str;
        g_string_free(s, FALSE);
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

static void handle_error(GError *error) {
    if (error != NULL) {
        g_warning("File read error: %s", error->message);
        g_clear_error(&error);
    }
}
