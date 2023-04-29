#include "ticket.h"

Ticket *ticket_new() {
    return (Ticket*) g_malloc(sizeof(Ticket));
}

void set_value(void *object, size_t index, void *value) {
    Ticket *ticket = (Ticket*) object;
    int64_t *i;
    double *d;
    switch (index) {
        case ID:
            i = value;
            ticket->id = *i;
            break;
        case TITLE:
            ticket->title = (GString*) value;
            break;
        case DESCRIPTION:
            ticket->description = (GString*) value;
            break;
        case CREATED_AT:
            ticket->created_at = (GString*) value;
            break;
        case CREATED_BY:
            i = value;
            ticket->created_by = *i;
            break;
        case SCORE:
            d = value;
            ticket->score = *d;
            break;
    }
}
