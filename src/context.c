#include "context.h"

void reset_url_matches(application_context *ctx) {
    if (ctx->url_matches != NULL) {
        g_list_free_full(ctx->url_matches, g_free);
        ctx->url_matches = NULL;
    }
}
