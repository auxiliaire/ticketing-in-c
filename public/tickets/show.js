'use strict';
import { h, html, render, useEffect, useState } from '../dist/preact.min.js';

const Ticket = function (props) {
  const [ticket, setTicket] = useState(props.ticket || {});

  useEffect(() => {
    setTicket(props.ticket);
  }, [props.ticket]);
  return html`
<h3>${ticket.title}</h3>
    <div class="container-sm mt-4">
        <div class="card">
            <div class="card-body">
                <div class="row">
                    <div class="col-2"><b>Id</b></div><div class="col-4">${ticket.id}</div>
                </div>
                <div class="row">
                    <div class="col-2"><b>Created At</b></div><div class="col-4">${ticket.created_at}</div>
                </div>
                <div class="row">
                    <div class="col-2"><b>Created By</b></div><div class="col-4">${ticket.created_by}</div>
                </div>
                <div class="row">
                    <div class="col-2"><b>Score</b></div><div class="col-4">${ticket.score}</div>
                </div>
            </div>
        </div>
        <div class="card mt-2">
            <div class="card-body">
                <h5 class="card-title">Description</h5>
                <div class="row">
                    <div class="col-5"><p>${ticket.description}</p></div>
                </div>
            </div>
        </div>
    </div>
    <a class="btn btn-warning btn-sm mt-4" href="/tickets/edit?id=${ticket.id}" type="button"><i class="fa-solid fa-pen"></i> Edit</a>`;
};

const App = function (props) {
  const [tickets, setTickets] = useState([]);

  const getTickets = () =>
    fetch('/api/tickets/' + props.id)
      .then(r => r.json())
      .then(r => setTickets(r))
      .catch(err => console.log(err));

  useEffect(() => {
    getTickets();
  }, []);

  return html`
<nav class="bg-dark-subtle mb-3 ps-5" aria-label="breadcrumb">
  <ol class="breadcrumb">
    <li class="breadcrumb-item"><a href="/" class="icon-link icon-link-hover" style="--bs-icon-link-transform: translate3d(0, -.125rem, 0);text-decoration:none;"><i class="bi fa-solid fa-house"></i> Home</a></li>
    <li class="breadcrumb-item"><a href="/tickets" class="icon-link icon-link-hover" style="--bs-icon-link-transform: translate3d(0, -.125rem, 0);text-decoration:none;"><i class="bi fa-solid fa-ticket"></i> Tickets</a></li>
    <li class="breadcrumb-item active" aria-current="page">${tickets[0]? tickets[0].id : 'View'}</li>
  </ol>
</nav>
<div class="container mt-4">
  ${h(Ticket, { ticket: tickets[0] || {} })}
    <button type="button" class="btn btn-outline-primary btn-sm mt-4 ms-2" onclick=${getTickets}><i class="fa-solid fa-rotate-left"></i> Refresh</button>
</div>
`;
};

window.onload = () => render(h(App, { id: window.ticketId }), document.getElementById('main'));
