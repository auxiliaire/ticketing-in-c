# Ticketing in C

## A Web Application Case Study

This is a web application written in C to demonstrate the enormous power and applicability of this venerable language.

![Build](https://github.com/auxiliaire/ticketing-in-c/actions/workflows/CMake.yml/badge.svg)

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
