'use strict';
import { h, html, render, useEffect, useState } from '../dist/preact.min.js';

const Ticket = function (props) {
  const [ticket, setTicket] = useState(props.ticket || {});

  useEffect(() => {
    setTicket(props.ticket);
  }, [props.ticket]);
  return html`
    <div class="container-sm mt-4">
        <h3><i class="fa-solid fa-pen"></i> Edit ticket</h3>
        <form action="/tickets/${ticket.id}" method="post">
            <input type="hidden" class="form-control" name="id" id="id" value=${ticket.id} />
            <div class="row mb-3">
                <label for="title" class="col-sm-2 col-form-label">Title</label>
                <div class="col-sm-10">
                  <input type="text" class="form-control" name="title" id="title" value=${ticket.title} />
                </div>
            </div>
            <div class="row mb-3">
                <label for="score" class="col-sm-2 col-form-label">Score</label>
                <div class="col-sm-10">
                  <input type="range" class="form-range" name="score" id="score" value=${ticket.score} min="0" max="1" step="0.1" />
                </div>
            </div>
            <div class="row mb-3">
                <label for="description" class="col col-form-label">Description</label>
                <div class="col-sm-10">
                  <textarea class="form-control" name="description" id="description" rows="5">${ticket.description}</textarea>
                </div>
            </div>
            <button type="submit" name="submit" class="btn btn-sm btn-primary"><i class="fa-solid fa-cloud-arrow-up"></i> Submit</button>
        </form>
    </div>
    `;
};

const App = function (props) {
  const [tickets, setTickets] = useState([]);

  const getTickets = () =>
    fetch('/tickets/' + props.id, { method: "GET", headers: { "Accept": "application/json" }})
      .then(r => r.json())
      .then(r => setTickets(r))
      .catch(err => console.log(err));

  useEffect(() => {
    getTickets();
  }, []);

  return html`
  <div>
    <nav class="bg-dark-subtle mb-3" aria-label="breadcrumb">
      <div class="container-xxl">
        <ol class="breadcrumb">
            <li class="breadcrumb-item"><a href="/" class="icon-link icon-link-hover" style="--bs-icon-link-transform: translate3d(0, -.125rem, 0);text-decoration:none;"><i class="bi fa-solid fa-house"></i> Home</a></li>
            <li class="breadcrumb-item"><a href="/tickets" class="icon-link icon-link-hover" style="--bs-icon-link-transform: translate3d(0, -.125rem, 0);text-decoration:none;"><i class="bi fa-solid fa-ticket"></i> Tickets</a></li>
            <li class="breadcrumb-item active" aria-current="page">${tickets[0]? tickets[0].id : 'View'}</li>
        </ol>
      </div>
    </nav>
    <div class="container mt-4">
    ${h(Ticket, { ticket: tickets[0] || {} })}
    </div>
  </div>
`;
};

window.onload = () => render(h(App, { id: window.selectedId }), document.getElementById('main'));
