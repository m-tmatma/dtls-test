#!/bin/bash

openssl genpkey -algorithm ED25519 -out server-key.pem
openssl req -new -key server-key.pem -out server-csr.pem
openssl x509 -req -days 365 -in server-csr.pem -signkey server-key.pem -out server-cert.pem
openssl x509 -in server-cert.pem -outform der -out server-cert.der
