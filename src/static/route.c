#include "route.h"

#include <limits.h>
#include <stdlib.h>
#include <glib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int is_file(const char *path);

bool static_route_try(struct mg_connection *c, void *ev_data, application_context *ctx) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    char path[120];
    GString *filename = g_string_new(ctx->root_dir);
    filename = g_string_append_len(filename, hm->uri.ptr, hm->uri.len);
    MG_INFO(("Serving file: '%s'", filename->str));
    if (access(filename->str, R_OK) == 0 && is_file(filename->str)) {
        struct mg_str unknown = mg_str_n("?", 1), *cl;
        struct mg_http_serve_opts opts = {
            .mime_types = "htm=text/html,html=text/html",
            .page404 = "./view/404.html"
        };
        mg_http_serve_file(c, hm, filename->str, &opts);
        return true;
    } else {
        MG_INFO(("...not found"));
        return false;
    }
}

int is_file(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode) | S_ISLNK(path_stat.st_mode);
}
