#include "ticket.h"

Ticket *ticket_new() {
    Ticket *ticket = g_malloc(sizeof(Ticket));
    ticket->id = 0;
    ticket->title = NULL;
    ticket->description = NULL;
    ticket->created_at = NULL;
    ticket->created_by = 0;
    ticket->score = 0.0;
    return ticket;
}

void ticket_delete(Ticket* ticket) {
    g_string_free(ticket->title, TRUE);
    ticket->title = NULL;
    g_string_free(ticket->description, TRUE);
    ticket->description = NULL;
    g_string_free(ticket->created_at, TRUE);
    ticket->created_at = NULL;
    g_free(ticket);
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

int64_t ticket_get_id(Ticket *ticket) {
    return ticket->id;
}

GString *ticket_get_title(Ticket *ticket) {
    return ticket->title;
}

GString *ticket_get_desctiption(Ticket *ticket) {
    return ticket->description;
}

GString *ticket_get_created_at(Ticket *ticket) {
    return ticket->created_at;
}

int64_t ticket_get_created_by(Ticket *ticket) {
    return ticket->created_by;
}

double ticket_get_score(Ticket *ticket) {
    return ticket->score;
}

void ticket_set_id(Ticket *ticket, int64_t id) {
    set_value(ticket, ID, (void*)&id);
}

void ticket_set_title(Ticket *ticket, GString *title) {
    set_value(ticket, TITLE, (void*)title);
}

void ticket_set_desctiption(Ticket *ticket, GString *description) {
    set_value(ticket, DESCRIPTION, (void*)description);
}

void ticket_set_created_at(Ticket *ticket, GString *created_at) {
    set_value(ticket, CREATED_AT, (void*)created_at);
}

void ticket_set_created_by(Ticket *ticket, int64_t created_by) {
    set_value(ticket, CREATED_BY, (void*)&created_by);
}

void ticket_set_score(Ticket *ticket, double score) {
    set_value(ticket, SCORE, (void*)&score);
}
