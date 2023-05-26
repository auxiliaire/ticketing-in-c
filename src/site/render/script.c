#include "script.h"

#define SCRIPT_TEMPALTE "<script type=\"module\" src=\"%s\"></script>"
#define ID_SCRIPT_TEMPLATE "<script type=\"application/javascript\">window.selectedId = '%s';</script><script type=\"module\" src=\"%s\"></script>"

void render_script_template(GString* render, const char* src, const char* id) {
    if (id == NULL) {
        g_string_printf(
            render,
            SCRIPT_TEMPALTE,
            src
        );
    } else {
        g_string_printf(
            render,
            ID_SCRIPT_TEMPLATE,
            id,
            src
        );
    }
}
