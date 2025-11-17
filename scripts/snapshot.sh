#!/bin/bash
set -e
GREEN="\033[0;32m"
RED="\033[0;31m"
NC="\033[0m" # No Color

trace() {
    echo -e "${GREEN}$1${NC}"
}

# Snapshot test:
# - Dump DB and diff with last known good version
#
# The next two lines set the source and build directories:
# - If running under Meson, MESON_SOURCE_ROOT and MESON_BUILD_ROOT are set automatically.
# - If running manually, fallback to one directory up from this script (for source), and 'build' inside that (for build).
SOURCE_ROOT="${MESON_SOURCE_ROOT:-$(cd "$(dirname "$0")/.." && pwd)}"
BUILD_ROOT="${MESON_BUILD_ROOT:-$SOURCE_ROOT/build}"

cd ${SOURCE_ROOT}
"${BUILD_ROOT}/sqlite-demo"

sqlite3 ${BUILD_ROOT}/demo.db .dump >"${BUILD_ROOT}/current_dump.sql"

if [ ! -f "scripts/lkg-demo.db" ]; then
    trace "${RED}No snapshot found"
    echo "Create one with: cp ${BUILD_ROOT}/demo.db scripts/lkg-demo.db"
    exit 1
fi

sqlite3 scripts/lkg-demo.db .dump | diff -u - "${BUILD_ROOT}/current_dump.sql"
trace "Snapshot test passed"
