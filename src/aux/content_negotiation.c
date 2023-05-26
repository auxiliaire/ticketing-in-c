#include "content_negotiation.h"

void cn_switch_json_html(struct mg_connection* c, void* ev_data, application_context* ctx, ActionDelegate json_method, ActionDelegate html_method) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    struct mg_str *s = mg_http_get_header(hm, "Accept");
    GRegex *regex = g_regex_new("\\*/\\*", 0, 0, NULL);

    if (s == NULL) {
        // No Accept header
        html_method(c, ev_data, ctx);

    } else if (mg_match(*s, mg_str("#application/json#"), NULL)) {
        // application/json
        json_method(c, ev_data, ctx);

    } else if (g_regex_match(regex, s->ptr, 0, NULL) || mg_match(*s, mg_str("#text/html#"), NULL)) {
        // text/html, */*
        html_method(c, ev_data, ctx);

    } else {
        // Reject
        mg_http_reply(c, 406, "", "%s", "Not Acceptable\n");
    }
    g_regex_unref(regex);
}
