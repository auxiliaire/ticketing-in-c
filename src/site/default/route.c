#include "route.h"
#include <glib.h>
#include "../../data/tickets.h"
#include "json-c/json_object.h"
#include "mustach/mustach.h"
#include "mustach/mustach-json-c.h"

void default_route(struct mg_connection *c, void *ev_data) {
    MG_INFO(("Default match: %s", DEFAULT_ROUTE));
    struct mg_http_message *hm = ev_data;

    size_t template_size = 0;
    char *template = mg_file_read(&mg_fs_posix, LAYOUT_VIEW, &template_size);
    if (template == NULL) {
        MG_ERROR(("Template read error: Error reading '%s'.", LAYOUT_VIEW));
    }

    struct json_object *o = json_object_new_object();
    struct json_object *title_val = json_object_new_string("Welcome");
    struct json_object *content_val = json_object_new_string("<div><h1>Welcome to our Ticketing platform</h1><span class=\"badge text-bg-primary\">Written in C</span></div><b>Current routes:</b><ul><li><a href=\"/tickets\">Tickets</a></li></ul>");
    struct json_object *script_val = json_object_new_string("<!-- No scripts this time -->");
    json_object_object_add(o, "title", title_val);
    json_object_object_add(o, "content", content_val);
    json_object_object_add(o, "script", script_val);

    char *result = NULL;
    size_t *rendered_size = 0;

    int mr = mustach_json_c_mem(template, template_size, o, Mustach_With_AllExtensions, &result, rendered_size);
    if (mr == MUSTACH_OK) {
        mg_http_reply(c, 200, "Content-Type: text/html\r\n", "%s", result, rendered_size);
    } else {
        MG_ERROR(("Template error: %d", mr));

        struct mg_http_serve_opts opts = {0};
        opts.root_dir = ".";
        opts.ssi_pattern = "*.html";
        mg_http_serve_file(c, hm, "view/500.html", &opts);
    }

    json_object_put(o);

}

