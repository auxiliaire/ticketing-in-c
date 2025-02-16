# Ticketing in C

## A Web Application Case Study

This is a web application written in C to demonstrate the enormous power and applicability of this venerable language.

![Build](https://github.com/auxiliaire/ticketing-in-c/actions/workflows/cmake.yml/badge.svg)

## Requirements

### Backend

* GLib
* SQLite3
* Mongoose (/lib)
* Mustach
* Json-C

### Frontend

* Bootstrap (/public/dist/bootstrap)
* Preact (/pubic/dist)

See CMakeList.txt for further details.

## Build

1. `mkdir build`

2. `cd build`

3. `cmake ../`

4. `cd ..`

5. `cd build && make && cd ..`

## Run

6. `build/ticketing -d $(realpath public)`

## Rebuild and run

5, 6

## Stop

7. Ctrl-C

## Demo

### The application homepage

![Preview Image](https://raw.githubusercontent.com/auxiliaire/ticketing-in-c/master/gallery/screenshot-home-2025-02-16.jpeg)

### Tickets list

![Preview Image](https://raw.githubusercontent.com/auxiliaire/ticketing-in-c/master/gallery/screenshot-tickets-2025-02-16.jpeg)

### Performance

![Preview Image](https://raw.githubusercontent.com/auxiliaire/ticketing-in-c/master/gallery/monitor-out-2025-02-16.png)

#### Measured with Siege

`siege --time=30s --file ./urls.txt`

where `urls.txt`:

```
http://localhost:8000/
http://localhost:8000/tickets
http://localhost:8000/tickets/1
http://localhost:8000/tickets/2
http://localhost:8000/tickets/3
http://localhost:8000/tickets/4
http://localhost:8000/tickets/edit/1
http://localhost:8000/tickets/edit/2
http://localhost:8000/tickets/edit/3
http://localhost:8000/tickets/edit/4
```

```json
{       "transactions":                        16129,
        "availability":                       100.00,
        "elapsed_time":                        29.74,
        "data_transferred":                  1005.04,
        "response_time":                        0.02,
        "transaction_rate":                   542.33,
        "throughput":                          33.79,
        "concurrency":                         11.19,
        "successful_transactions":             16129,
        "failed_transactions":                     0,
        "longest_transaction":                  7.35,
        "shortest_transaction":                 0.00
}
```
