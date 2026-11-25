#! /usr/bin/env bash

set -euo pipefail
non_ascii=$(find include src singleheader tests examples \
	-name '*.hpp' -o -name '*.cpp' \
	-type f -exec file '{}' \; | grep -v ASCII)
if [ -n "$non_ascii" ]; then
	echo 'The following files contain non-ASCII characters:'
	echo "$non_ascii"
	exit 1
fi
