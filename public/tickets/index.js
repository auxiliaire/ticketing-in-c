'use strict';
import { h, html, render, useEffect, useState } from '../dist/preact.min.js';

const Tickets = function (props) {
  const [ tickets, setTickets ] = useState(props.tickets || []);

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
        <th scope="col"><i class="fa-solid fa-wrench"></i></th>
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
        <td>
          <form id="tdf_${ticket.id}" method="post" action="/tickets/delete">
            <input type="hidden" name="id" value=${ticket.id} />
            <div class="btn-toolbar" role="toolbar">
              <div class="btn-group btn-group-sm me-1" role="group">
                <a href="/tickets/${ticket.id}" type="button" class="col btn btn-outline-primary btn-sm"><i class="fa-solid fa-binoculars"></i></a>
              </div>
              <div class="btn-group btn-group-sm mt-1" role="group">
                <button type="button" class="col btn btn-outline-danger btn-sm" onclick=${() => props.setCurrent(ticket)} data-bs-toggle="modal" data-bs-target="#deleteConfirmModal"><i class="fa-solid fa-trash"></i></button>
              </div>
            </div>
          </form>
        </td>
      </tr>
    `)}
    </tbody>
    </table>`;
};

const Modal = function (props) {
  const [ currentTicket, setCurrentTicket ] = useState(props.currentTicket || { id: null, title: '' });

  useEffect(() => {
    setCurrentTicket(props.currentTicket);
  }, [props.currentTicket]);

  const doDelete = () => {
    const form = document.getElementById('tdf_' + currentTicket.id);
    const dcModal = bootstrap.Modal.getInstance('#deleteConfirmModal');
    dcModal.hide();
    if (form) {
      form.submit();
    }
  };

  return html`
    <div class="modal fade" id="deleteConfirmModal" tabindex="-1" aria-labelledby="dcModalLabel" aria-hidden="true">
      <div class="modal-dialog modal-dialog-centered">
        <div class="modal-content">
          <div class="modal-header">
            <h1 class="modal-title fs-5" id="dcModalLabel">Are you sure you want to delete this ticket?</h1>
            <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
          </div>
          <div class="modal-body">
            ${currentTicket.title} (${currentTicket.id})
          </div>
          <div class="modal-footer">
            <button type="button" class="btn btn-secondary" data-bs-dismiss="modal">Cancel</button>
            <button id="dcBtnDelete" type="button" class="btn btn-danger" onclick=${doDelete}>Delete</button>
          </div>
        </div>
      </div>
    </div>
  `;
};

const App = function (props) {
  const [ state, setState ] = useState({ tickets: [], current: { id: null, title: '' }});

  const getTickets = () =>
    fetch('/api/tickets/')
      .then(r => r.json())
      .then(r => setState({ tickets: r, current: state.current }))
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
        ${h(Tickets, { tickets: state.tickets, setCurrent: (ticket) => setState({ tickets: state.tickets, current: ticket }) })}
        <button type="button" class="btn btn-outline-primary btn-sm my-4" onclick=${getTickets}><i class="fa-solid fa-rotate-left"></i> Refresh</button>
        <a href="/tickets/new" class="btn btn-primary btn-sm my-4 ms-2"><i class="fa-solid fa-plus"></i> New Ticket</a>
      </div>
    </div>
    ${h(Modal, { currentTicket: state.current })}
  </div>
  `;
};

window.onload = () => render(h(App), document.getElementById('main'));

