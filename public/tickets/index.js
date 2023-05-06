'use strict';
import { h, html, render, useEffect, useState } from '../dist/preact.min.js';


const Tickets = function (props) {
  const [tickets, setTickets] = useState(props.tickets || []);

  useEffect(() => {
    setTickets(props.tickets);
  }, [props.tickets]);
  return html`<table class="table table-striped table-hover">
    <thead>
      <tr>
        <th scope="col">Id</th>
        <th scope="col">Title</th>
        <th scope="col">Description</th>
        <th scope="col">Created At</th>
        <th scope="col">Created By</th>
        <th scope="col">Score</th>
        <th></th>
      </tr>
    </thead>
    <tbody>
    ${tickets.map(ticket => html`
      <tr key=${ticket.id}>
        <th scope="row">${ticket.id}</th>
        <td>${ticket.title}</td>
        <td>${ticket.description}</td>
        <td>${ticket.created_at}</td>
        <td>${ticket.created_by}</td>
        <td>${ticket.score}</td>
        <td><a href="/tickets/${ticket.id}" type="button" class="btn btn-outline-primary btn-sm"><i class="fa-solid fa-binoculars"></i></a></td>
      </tr>
    `)}
    </tbody>
    </table>`;
};

const App = function (props) {
  const [tickets, setTickets] = useState([]);

  const getTickets = () =>
    fetch('/api/tickets/')
      .then(r => r.json())
      .then(r => setTickets(r))
      .catch(err => console.log(err));

  useEffect(() => {
    getTickets();
  }, []);

  return html`
  <div>
    <nav class="bg-dark-subtle mb-3 ps-5" aria-label="breadcrumb">
      <ol class="breadcrumb">
        <li class="breadcrumb-item"><a href="/" class="icon-link icon-link-hover" style="--bs-icon-link-transform: translate3d(0, -.125rem, 0);text-decoration:none;"><i class="bi fa-solid fa-house"></i> Home</a></li>
        <li class="breadcrumb-item active" aria-current="page"><i class="fa-solid fa-ticket"></i> Tickets</li>
      </ol>
    </nav>
    <div class="container mt-4">
      <h3>Tickets</h3>
      <div class="container">
        ${h(Tickets, { tickets })}
        <button type="button" class="btn btn-outline-primary btn-sm mt-4" onclick=${getTickets}><i class="fa-solid fa-rotate-left"></i> Refresh</button>
      </div>
    </div>
  </div>
  `;
};

window.onload = () => render(h(App), document.getElementById('main'));

