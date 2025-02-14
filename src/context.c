#include "context.h"

void string_free(void *p) {
    free(p);
}

void reset_url_matches(application_context *ctx) {
    if (ctx->url_matches != NULL) {
        g_list_free_full(ctx->url_matches, string_free);
        ctx->url_matches = NULL;
    }
}
