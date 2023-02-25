#!/bin/bash
clang-15 \
  -I./ \
  --target=wasm32 \
  --no-standard-libraries \
  -Wl,--export-all -Wl,--no-entry \
  -o libinjection.wasm \
  string.c \
  ./libinjection_html5.c \
  ./libinjection_xss.c \
  ./libinjection_sqli.c

