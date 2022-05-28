#!/bin/bash
# ref: https://stackoverflow.com/a/47502029

openssl rand -hex $(( 1000000 * 4 )) | \
while IFS= read -rn64 -d '' r; do
    echo "$r"
done
