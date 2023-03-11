#!/bin/bash

# color outputs for better visual
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

echo
# Trigger all your test cases with this script
for TEST in $(find tests -iregex '.*\.in' | xargs basename -s .in); do
    echo ""
    IN=tests/$TEST.in
    OUT=tests/$TEST.out
    ./gitm < $IN | diff -su - $OUT && echo -e "testing $TEST: ${GREEN}SUCCESS!${NC}" || echo -e "testing $TEST: ${RED}FAILURE.${NC}"
done

echo