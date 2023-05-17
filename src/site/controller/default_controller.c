#include "default_controller.h"

#include "../render/layout.h"

void get_index_action(struct mg_connection *c, void *ev_data, application_context *ctx) {
    struct mg_http_message *hm = ev_data;

    LayoutModel layout_model = {
        .layout_filename = ctx->layout->str,
        .title = "Welcome",
        .content = "<nav class=\"bg-dark-subtle mb-3\" aria-label=\"breadcrumb\"><div class=\"container-xxl\"><ol class=\"breadcrumb\"><li class=\"breadcrumb-item active\" aria-current=\"page\"><i class=\"bi fa-solid fa-house\"></i> Home</li><li class=\"breadcrumb-item\"><a href=\"/tickets\" class=\"icon-link icon-link-hover\" style=\"--bs-icon-link-transform: translate3d(0, -.125rem, 0);text-decoration:none;\"><i class=\"bi fa-solid fa-ticket\"></i> Tickets</a></li></ol></div></nav><div class=\"container-xxl\"><h3 class=\"mt-4\">Welcome to our Ticketing platform</h3><span class=\"badge text-bg-primary\">Written in C</span><div class=\"row mt-4\"><b>Current routes:</b><ul><li><a href=\"/tickets\">Tickets</a></li></ul></div></div>",
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
}
