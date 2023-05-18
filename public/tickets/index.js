'use strict';
import { h, html, render, useEffect, useLayoutEffect, useState } from '../dist/preact.min.js';

const Caret = function (props) {
  const [ sort, setSort ] = useState(props.sort || null);
  const [ order, setOrder ] = useState(props.order || 1);
  const [ dir, setDir ] = useState('up');
  const [ display, setDisplay ] = useState('visually-hidden');

  const getDisplay = () => {
    return props.id === sort ? '' : 'visually-hidden';
  };

  useEffect(() => {
    setSort(props.sort);
  }, [props.sort]);

  useEffect(() => {
    setOrder(props.order);
  }, [props.order]);

  useEffect(() => {
    setDisplay(getDisplay());
  }, [sort, order]);

  useEffect(() => {
    setDir(order > 0 ? 'up' : 'down');
  }, [order]);

  return html`
    <i class="fa-solid fa-caret-${dir} ${display}"></i>
  `;
};

const SortHead = function (props) {
  const [ sort, setSort ] = useState(props.sort);
  const [ order, setOrder ] = useState(props.order || 1);

  const sortBy = (column) => {
    if (sort === column) {
      setOrder(-1 * order);
    }
    setSort(column);
  };

  useLayoutEffect(() => {
    props.resort(sort, order);
  }, [sort, order]);

  return html`
    <thead>
      <tr>
        <th scope="col"><a href="#" class="text-nowrap link-body-emphasis link-underline-opacity-0 link-underline-opacity-75-hover" onclick=${(e) => { sortBy('id'); e.preventDefault(); }}>Id ${h(Caret, { id: 'id', sort, order })}</a></th>
        <th scope="col"><a href="#" class="text-nowrap link-body-emphasis link-underline-opacity-0 link-underline-opacity-75-hover" onclick=${(e) => { sortBy('title'); e.preventDefault(); }}>Title ${h(Caret, { id: 'title', sort, order })}</a></th>
        <th scope="col"><a href="#" class="text-nowrap link-body-emphasis link-underline-opacity-0 link-underline-opacity-75-hover" onclick=${(e) => { sortBy('description'); e.preventDefault(); }}>Description ${h(Caret, { id: 'description', sort, order })}</a></th>
        <th scope="col"><a href="#" class="text-nowrap link-body-emphasis link-underline-opacity-0 link-underline-opacity-75-hover" onclick=${(e) => { sortBy('created_at'); e.preventDefault(); }}>Created At ${h(Caret, { id: 'created_at', sort, order })}</a></th>
        <th scope="col"><a href="#" class="text-nowrap link-body-emphasis link-underline-opacity-0 link-underline-opacity-75-hover" onclick=${(e) => { sortBy('created_by'); e.preventDefault(); }}>Created By ${h(Caret, { id: 'created_by', sort, order })}</a></th>
        <th scope="col"><a href="#" class="text-nowrap link-body-emphasis link-underline-opacity-0 link-underline-opacity-75-hover" onclick=${(e) => { sortBy('score'); e.preventDefault(); }}>Score ${h(Caret, { id: 'score', sort, order })}</a></th>
        <th scope="col"><i class="fa-solid fa-wrench"></i></th>
      </tr>
    </thead>
  `;
}

const Tickets = function (props) {
  const [ tickets, setTickets ] = useState(props.tickets || []);

  const resort = (column, order) => {
    tickets.sort((a, b) => {
      if (Number.isFinite(a[column]) && Number.isFinite(b[column])) {
        return order * (a[column] - b[column]);
      } else {
        return order * String(a[column]).localeCompare(String(b[column]));
      }
    });
    setTickets([...tickets]);
  };

  useEffect(() => {
    setTickets(props.tickets);
  }, [props.tickets]);

  return html`<table class="table table-striped table-hover">
    ${h(SortHead, { resort })}
    <tbody>
    ${tickets.map(ticket => html`
      <tr key=${ticket.id}>
        <th scope="row">${ticket.id}</th>
        <td>${ticket.title}</td>
        <td>${ticket.description}</td>
        <td>${ticket.created_at}</td>
        <td><span class="badge bg-secondary">${ticket.created_by}</span></td>
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

const Spinner = function (props) {
  const [ display, setDisplay ] = useState('d-block');

  useLayoutEffect(() => {
    setDisplay(props.isShown() ? 'd-block' : 'd-none');
  }, [props.isShown]);
  return html`
    <div class="d-flex justify-content-center ${display}">
      <div class="spinner-border" role="status">
        <span class="visually-hidden">Loading...</span>
      </div>
    </div>
  `;
};

const App = function (props) {
  const [ state, setState ] = useState({ tickets: [], current: { id: null, title: '' }});

  const getTickets = () => {
    setState({ tickets: [], current: { id: null, title: '' }});
    fetch('/tickets', { method: "GET", headers: { "Accept": "application/json" }})
      .then(r => r.json())
      .then(r => setState({ tickets: r, current: state.current }))
      .catch(err => console.log(err));
  };

  useEffect(() => {
    getTickets();
  }, []);

  return html`
  <div>
    <nav class="bg-dark-subtle mb-3" aria-label="breadcrumb">
      <div class="container-xxl">
        <ol class="breadcrumb">
          <li class="breadcrumb-item"><a href="/" class="icon-link icon-link-hover" style="--bs-icon-link-transform: translate3d(0, -.125rem, 0);text-decoration:none;"><i class="bi fa-solid fa-house"></i> Home</a></li>
          <li class="breadcrumb-item active" aria-current="page"><i class="fa-solid fa-ticket"></i> Tickets</li>
        </ol>
      </div>
    </nav>
    <div class="container-xxl mt-4">
      <h3>Tickets</h3>
      <div class="container">
        ${h(Spinner, { isShown: () => state.tickets.length === 0})}
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

