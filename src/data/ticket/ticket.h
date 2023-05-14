#ifndef TICKETING_TICKET_H
#define TICKETING_TICKET_H

#include <glib.h>
#include "data_access.h"

typedef enum ticket_field {
  ID,
  TITLE,
  DESCRIPTION,
  CREATED_AT,
  CREATED_BY,
  SCORE
} TicketField;

typedef struct ticket {
  int64_t id;
  GString *title;
  GString *description;
  GString *created_at;
  int64_t created_by;
  double  score;
} Ticket;

Ticket *ticket_new();
void ticket_delete(Ticket *ticket);

int64_t ticket_get_id(Ticket *ticket);
GString *ticket_get_title(Ticket *ticket);
GString *ticket_get_desctiption(Ticket *ticket);
GString *ticket_get_created_at(Ticket *ticket);
int64_t ticket_get_created_by(Ticket *ticket);
double  ticket_get_score(Ticket *ticket);

void ticket_set_id(Ticket *ticket, int64_t id);
void ticket_set_title(Ticket *ticket, GString *title);
void ticket_set_desctiption(Ticket *ticket, GString *description);
void ticket_set_created_at(Ticket *ticket, GString *created_at);
void ticket_set_created_by(Ticket *ticket, int64_t created_by);
void ticket_set_score(Ticket *ticket, double score);

#endif
