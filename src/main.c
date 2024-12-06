// Copyright (c) 2020 Cesanta Software Limited
// All rights reserved

#include <signal.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <glib.h>

#include "../lib/mongoose.h"

#include "static/route.h"
#include "site/default/route.h"

#define TICKETS_ROUTE "/tickets/*"

#define LAYOUT_VIEW "view/layout.html"

/* SERVER */
static int s_debug_level = MG_LL_INFO;
static const char *s_root_dir = ".";
static const char *s_listening_address = "http://0.0.0.0:8000";
static const char *s_enable_hexdump = "no";
static const char *s_ssi_pattern = "#.shtml";

/* DATABASE*/
static const char *s_database = "./dev.sqlite";
static sqlite3 *_db;
static application_context _ctx = {};

// Handle interrupts, like Ctrl-C
static int s_signo;
static void signal_handler(int signo) {
    s_signo = signo;
}

static void http_dispatcher(struct mg_connection *c, int ev, void *ev_data) {
#ifdef ENABLE_TLS
    if (ev == MG_EV_ACCEPT) {
        struct mg_tls_opts opts = {
            .cert = "cert.pem",
            .certkey = "key.pem",
        };
        mg_tls_init(c, &opts);
    }
#endif
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = ev_data;

        // TODO: refactor this if to a loop
        if (static_route_try(c, ev_data, c->fn_data)) {
            /* STATIC */
            return;

        } else if (default_route_try(c, ev_data, c->fn_data)) {
            /* DEFAULT ROUTE */
            return;

        } else {
            /* 404 */
            struct mg_http_serve_opts opts = {0};
            opts.root_dir = s_root_dir;
            mg_http_serve_file(c, hm, "view/404.html", &opts);
        }
    }
}

static void usage(const char *prog) {
    fprintf(stderr,
            "Mongoose v.%s\n"
            "Usage: %s OPTIONS\n"
            "  -H yes|no - enable traffic hexdump, default: '%s'\n"
            "  -S PAT    - SSI filename pattern, default: '%s'\n"
            "  -d DIR    - directory to serve, default: '%s'\n"
            "  -l ADDR   - listening address, default: '%s'\n"
            "  -v LEVEL  - debug level, from 0 to 4, default: %d\n",
            MG_VERSION, prog, s_enable_hexdump, s_ssi_pattern, s_root_dir,
            s_listening_address, s_debug_level);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    char path[MG_PATH_MAX] = ".";
    struct mg_mgr mgr;
    struct mg_connection *c;
    int i;

    // Parse command-line flags
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            s_root_dir = argv[++i];
        } else if (strcmp(argv[i], "-H") == 0) {
            s_enable_hexdump = argv[++i];
        } else if (strcmp(argv[i], "-S") == 0) {
            s_ssi_pattern = argv[++i];
        } else if (strcmp(argv[i], "-l") == 0) {
            s_listening_address = argv[++i];
        } else if (strcmp(argv[i], "-v") == 0) {
            s_debug_level = atoi(argv[++i]);
        } else {
            usage(argv[0]);
        }
    }

    // Root directory must not contain double dots. Make it absolute
    // Do the conversion only if the root dir spec does not contain overrides
    if (strchr(s_root_dir, ',') == NULL) {
        realpath(s_root_dir, path);
        s_root_dir = path;
    }

    // Initialize stuff
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    // Initialize database
    if (sqlite3_open(s_database, &_db)) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(_db));
        sqlite3_close(_db);
        exit(EXIT_FAILURE);
    }

    // Initialize application context
    _ctx.db = _db;
    _ctx.layout = g_string_new(LAYOUT_VIEW);
    _ctx.url_matches = NULL;
    _ctx.root_dir = s_root_dir;

    // Initialize webserver
    mg_log_set(s_debug_level);
    mg_mgr_init(&mgr);
    if ((c = mg_http_listen(&mgr, s_listening_address, http_dispatcher, &_ctx)) == NULL) {
        MG_ERROR(("Cannot listen on %s. Use http://ADDR:PORT or :PORT",
                  s_listening_address));
        exit(EXIT_FAILURE);
    }
    if (mg_casecmp(s_enable_hexdump, "yes") == 0) c->is_hexdumping = 1;

    // Start infinite event loop
    MG_INFO(("Mongoose version : v%s", MG_VERSION));
    MG_INFO(("Listening on     : %s", s_listening_address));
    MG_INFO(("Web root         : [%s]", s_root_dir));
    MG_INFO(("Database         : %s", s_database));

    while (s_signo == 0) mg_mgr_poll(&mgr, 1000);

    // Cleanup
    mg_mgr_free(&mgr);
    g_string_free(_ctx.layout, TRUE);
    default_route_cleanup();
    MG_INFO(("Exiting on signal %d", s_signo));
    MG_INFO(("Closing database connection to '%s'", s_database));
    sqlite3_close(_db);
    return 0;
}
