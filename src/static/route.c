#include "route.h"

#include <limits.h>
#include <stdlib.h>
#include <glib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int is_file(const char *path);

bool static_route_try(struct mg_connection *c, void *ev_data, const application_context *ctx) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    GString *filename = g_string_new(ctx->root_dir);
    filename = g_string_append_len(filename, hm->uri.buf, hm->uri.len);
    MG_INFO(("Serving file: '%s'", filename->str));
    if (access(filename->str, R_OK) == 0 && is_file(filename->str)) {
        struct mg_http_serve_opts opts = {
            .mime_types = "htm=text/html,html=text/html",
            .page404 = "./view/404.html"
        };
        char *path = g_string_free_and_steal(filename);
        mg_http_serve_file(c, hm, path, &opts);
        free(path);
        return TRUE;
    } else {
        g_string_free(filename, TRUE);
        MG_INFO(("...not found"));
        return FALSE;
    }
}

int is_file(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode) | S_ISLNK(path_stat.st_mode);
}
