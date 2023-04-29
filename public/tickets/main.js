'use strict';
import { h, html, render, useEffect, useState } from '../dist/preact.min.js';


const Configuration = function (props) {
  const [url, setUrl] = useState(props.config.url || '');
  const [pub, setPub] = useState(props.config.pub || '');
  const [sub, setSub] = useState(props.config.sub || '');

  useEffect(() => {
    setUrl(props.config.url);
    setPub(props.config.pub);
    setSub(props.config.sub);
  }, [props.config]);

  const update = (name, val) =>
    fetch('/api/config/set', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json'
      },
      body: JSON.stringify({
        [name]: val
      })
    })
      .catch(err => {
        console.log(err);
        enable(false);
      });

  const updateurl = ev => update('url', url);
  const updatepub = ev => update('pub', pub);
  const updatesub = ev => update('sub', sub);

  return html`
<h3 style="background: #c03434; color: #fff; padding: 0.4em;">
  Device Configuration</h3>
<div style="margin: 0.5em 0; display: flex;">
  <span class="addon nowrap">MQTT server:</span>
  <input type="text" style="flex: 1 100%;"
        value=${url} onchange=${updateurl}
        oninput=${ev => setUrl(ev.target.value)} />
  <button class="btn" disabled=${!url} onclick=${updateurl}
    style="margin-left: 1em; background: #8aa;">Update</button>
</div>
<div style="margin: 0.5em 0; display: flex; ">
  <span class="addon nowrap">Subscribe topic:</span>
  <input type="text" style="flex: 1 100%;"
      value=${sub} onchange=${updatesub}
      oninput=${ev => setSub(ev.target.value)} />
  <button class="btn" disabled=${!sub} onclick=${updatesub}
    style="margin-left: 1em; background: #8aa;">Update</button>
</div>
<div style="margin: 0.5em 0; display: flex;">
  <span class="addon nowrap">Publish topic:</span>
  <input type="text" style="flex: 1 100%;"
        value=${pub} onchange=${updatepub}
        oninput=${ev => setPub(ev.target.value)} />
  <button class="btn" disabled=${!pub} onclick=${updatepub}
    style="margin-left: 1em; background: #8aa;">Update</button>
</div>`;
};

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
<h1>Tickets</h1>
<div class="container">
  ${h(Tickets, { tickets })}
</div>
<button type="button" class="btn btn-primary" onclick=${getTickets}>Refresh tickets</button>`;
};

window.onload = () => render(h(App), document.getElementById('main'));

