#!/bin/bash

gcc src/main.c -o server.out -lpthread
gcc Client/src/main.c -o client.out -lpthread

