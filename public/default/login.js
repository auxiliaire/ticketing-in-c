'use strict';
import { h, html, render, useEffect, useState } from '../dist/preact.min.js';

const Login = function (props) {
  const [user, setUser] = useState('');
  const [pass, setPass] = useState('');
  const login = () =>
    fetch(
      '/login',
      { headers: { Accept: 'application/json', Authorization: 'Basic ' + btoa(user + ':' + pass) } })
      .then(r => r.json())
      .then(r => r && props.login(r))
      .catch(err => err);
  return html`
    <div class="d-flex flex-row justify-content-center">
        <div class="card bg-light-subtle col-4">
            <div class="card-header">Please log in</div>
            <div class="card-body my-1 mx-0">
                <input type="text" placeholder="Name" class="form-control mb-2" oninput=${ev => setUser(ev.target.value)} value=${user} />
                <input type="password" placeholder="Password" class="form-control mb-2" oninput=${ev => setPass(ev.target.value)} value=${pass}
                    onchange=${login} />
                <button class="btn btn-primary w-100" disabled=${!user || !pass} onclick=${login}> Login </button>
            </div>
            <div class="card-footer text-body-secondary">
                Valid logins: admin:admin, viktor:viktor
            </div>
        </div>
    </div>
`;
};

const App = function (props) {
  const [user, setUser] = useState('');
  const [data, setData] = useState({});

  const getIn = () =>
    fetch('/login', { method: "GET", headers: { Accept: "application/json", Authorization: '' }})
      .then(r => r.json())
      .then(r => login(r))
      .catch(err => setUser(''));

  const login = function (u) {
    document.cookie = `access_token=${u.token};path=/;max-age=3600;SameSite=None;Secure`;
    setUser(u.user);
    return getin();
  };

  const logout = () => {
    document.cookie = `access_token=;path=/;max-age=0`;
    setUser('');
  };

  useEffect(() => {
    // Called once at init time
    fetch('/login', { headers: { Accept: "application/json", Authorization: '' } })
      .then(r => r.json())
      .then(r => login(r))
      .catch(() => setUser(''));
  }, []);

  if (user) window.location.replace(window.location.toLocaleString().replace('/login', '/tickets'));

  return html`
  <div>
    <nav class="bg-dark-subtle mb-3" aria-label="breadcrumb">
      <div class="container-xxl">
        <ol class="breadcrumb">
          <li class="breadcrumb-item"><a href="/" class="icon-link icon-link-hover" style="--bs-icon-link-transform: translate3d(0, -.125rem, 0);text-decoration:none;"><i class="bi fa-solid fa-house"></i> Home</a></li>
          <li class="breadcrumb-item active" aria-current="page"><i class="fa-solid fa-right-to-bracket"></i> Login</li>
        </ol>
      </div>
    </nav>
    <div class="container mt-4">
      <${Login} login=${login} />
    </div>
  </div>
`;
};

window.onload = () => render(h(App), document.getElementById('main'));

