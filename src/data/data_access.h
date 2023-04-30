#ifndef TICKETING_DATA_ACCESS_H
#define TICKETING_DATA_ACCESS_H

/**
 * Defines an interface only.
 * Model classes are expected to implement it.
 */

#include <stdlib.h>

void set_value(void *object, size_t index, void *value);
// TODO: define as_json.

#endif
