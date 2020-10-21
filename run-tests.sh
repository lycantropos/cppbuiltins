#!/usr/bin/env bash

set -e

docker-compose up --build --exit-code-from cppbuiltins

STATUS=$?

docker-compose down --remove-orphans

if [[ "$STATUS" -eq "0" ]]; then
	echo "tests passed";
else
	echo "tests failed"
fi

exit ${STATUS}
