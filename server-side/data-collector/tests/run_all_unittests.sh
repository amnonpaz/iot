#!/bin/bash

if [[ $# < 1 ]] ; then
    echo "[ERROR] "$0 ": Must take one parameter (unittests location)"
    exit 1
fi

TEST_DIR=$1

for EXEC in $(find ${TEST_DIR} -name "*_unittest") ; do
    ./${EXEC}
done
