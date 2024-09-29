#!/bin/bash

gcc -o dtls_server dtls_server.c -lssl -lcrypto
gcc -o dtls_client dtls_client.c -lssl -lcrypto
