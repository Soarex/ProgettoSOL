#!/bin/bash

gcc Client/src/client.c -o bin/client.out -lpthread
gcc Server/src/server.c -o bin/server.out -lpthread