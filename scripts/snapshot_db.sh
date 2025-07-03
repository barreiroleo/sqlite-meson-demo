#!/bin/bash
set -e

# Snapshot test:
# - Dump DB and diff with last known good version
#
# The next two lines set the source and build directories:
# - If running under Meson, MESON_SOURCE_ROOT and MESON_BUILD_ROOT are set automatically.
# - If running manually, fallback to one directory up from this script (for source), and 'build' inside that (for build).
SOURCE_ROOT="${MESON_SOURCE_ROOT:-$(cd "$(dirname "$0")/.." && pwd)}"
BUILD_ROOT="${MESON_BUILD_ROOT:-$SOURCE_ROOT/build}"

cd ${SOURCE_ROOT}
"${BUILD_ROOT}/client_test"

sqlite3 library.db .dump > "${BUILD_ROOT}/current_dump.sql"

if [ -f "scripts/lkg_library.db" ]; then
    sqlite3 scripts/lkg_library.db .dump | diff -u - "${BUILD_ROOT}/current_dump.sql"
    echo "✔️ Snapshot test passed"
else
    echo "❌ No snapshot found. Create one with: cp library.db scripts/lkg_library.db"
    exit 1
fi
