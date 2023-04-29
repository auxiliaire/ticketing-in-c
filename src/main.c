// Copyright (c) 2020 Cesanta Software Limited
// All rights reserved

#include <signal.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <glib.h>

#include "../lib/mongoose.h"

#include "api/route.h"
#include "site/default/route.h"

#define TICKETS_ROUTE "/tickets/*"

/* SERVER */
static int s_debug_level = MG_LL_INFO;
static const char *s_root_dir = ".";
static const char *s_listening_address = "http://0.0.0.0:8000";
static const char *s_enable_hexdump = "no";
static const char *s_ssi_pattern = "#.html";

/* DATABASE*/
static const char *s_database = "./dev.sqlite";
static sqlite3 *_db;
static application_context _ctx = {};

// Handle interrupts, like Ctrl-C
static int s_signo;
static void signal_handler(int signo) {
    s_signo = signo;
}

static void http_dispatcher(struct mg_connection *c, int ev, void *ev_data, void *ctx) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = ev_data, tmp = {0};
        struct mg_str parts[3];

        if (api_route_try(c, ev_data, ctx)) {
            /* API */
            return;

        } else if (mg_match(hm->uri, mg_str("/*/*"), parts)) {
            /* CONTROLLER/ACTION ROUTE */
            GString *controller = g_string_new_len(parts[0].ptr, parts[0].len);
            GString *action = g_string_new_len(parts[1].ptr, parts[1].len);
            MG_INFO(("\tController: %s, Action: %s", controller->len > 0 ? controller->str : "default", action->len > 0 ? action->str : "index"));
            mg_http_reply(c, 200, "Content-Type: text/plain\r\n", "OK");

        } else if (mg_match(hm->uri, mg_str(DEFAULT_ROUTE), NULL)) {
            /* DEFAULT ROUTE */
            default_route(c, ev_data);

        } else {

            /* SERVE FILES */
            struct mg_str unknown = mg_str_n("?", 1), *cl;
            struct mg_http_serve_opts opts = {0};
            opts.root_dir = s_root_dir;
            opts.ssi_pattern = s_ssi_pattern;
            mg_http_serve_dir(c, hm, &opts);
            mg_http_parse((char *) c->send.buf, c->send.len, &tmp);
            cl = mg_http_get_header(&tmp, "Content-Length");
            if (cl == NULL) cl = &unknown;
            MG_INFO(("%.*s %.*s %.*s %.*s", (int) hm->method.len, hm->method.ptr,
                     (int) hm->uri.len, hm->uri.ptr, (int) tmp.uri.len, tmp.uri.ptr,
                     (int) cl->len, cl->ptr));

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
    _ctx.db = _db;
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
    MG_INFO(("Exiting on signal %d", s_signo));
    MG_INFO(("Closing database connection to '%s'", s_database));
    sqlite3_close(_db);
    return 0;
}