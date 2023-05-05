#include "layout.h"

#include <stdio.h>
#include "json-c/json_object.h"
#include <mustach/mustach.h>
#include <mustach/mustach-json-c.h>

GString *render_layout(const LayoutModel* layout_model) {
    size_t template_size = 0;
    gchar *template = NULL;
    char *result = NULL;
    size_t rendered_size = 0;
    GError *error = NULL;
    GString *render = g_string_new("");

    gboolean success = g_file_get_contents(layout_model->layout_filename, &template, &template_size, &error);
    if (!success) {
        g_warning("Template read error: Error reading '%s'.", layout_model->layout_filename);
        return g_string_new("");
    }

    struct json_object *o = json_object_new_object();
    struct json_object *title_val = json_object_new_string(layout_model->title);
    struct json_object *content_val = json_object_new_string(layout_model->content);
    struct json_object *script_val = json_object_new_string(layout_model->script);
    json_object_object_add(o, "title", title_val);
    json_object_object_add(o, "content", content_val);
    json_object_object_add(o, "script", script_val);

    int mr = mustach_json_c_mem(template, template_size, o, Mustach_With_AllExtensions, &result, &rendered_size);
    if (mr == MUSTACH_OK) {
        render = g_string_append_len(render, result, rendered_size);
        free(result);
        g_free(template);
    } else {
        g_warning("Template error: %d", mr);
    }
    json_object_put(o);
    return render;
}

