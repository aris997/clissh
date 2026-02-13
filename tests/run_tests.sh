#!/usr/bin/env bash
set -euo pipefail

BINARY=./clissh
FIXTURE=tests/fixtures/ssh_config
PASS=0
FAIL=0

check() {
    local name="$1"
    local result="$2"
    if [ "$result" = "ok" ]; then
        printf "  PASS: %s\n" "$name"
        PASS=$((PASS + 1))
    else
        printf "  FAIL: %s\n" "$name"
        FAIL=$((FAIL + 1))
    fi
}

if [ ! -f "$BINARY" ]; then
    printf "Binary not found: %s\nRun 'make' first.\n" "$BINARY"
    exit 1
fi

echo "Running clissh tests..."
echo ""

# -h shows help
output=$("$BINARY" -h 2>&1)
if echo "$output" | grep -q "Usage: clissh"; then
    check "-h shows help" "ok"
else
    check "-h shows help" "fail"
fi

# --help shows help
output=$("$BINARY" --help 2>&1)
if echo "$output" | grep -q "Usage: clissh"; then
    check "--help shows help" "ok"
else
    check "--help shows help" "fail"
fi

# cancel with 'c' exits 0
output=$(echo "c" | "$BINARY" -f "$FIXTURE" 2>&1)
ret=$?
if [ $ret -eq 0 ]; then
    check "cancel with 'c' exits 0" "ok"
else
    check "cancel with 'c' exits 0" "fail"
fi

# hosts are listed in the menu
output=$(echo "c" | "$BINARY" -f "$FIXTURE" 2>&1)
if echo "$output" | grep -q "server1"; then
    check "menu lists server1" "ok"
else
    check "menu lists server1" "fail"
fi

if echo "$output" | grep -q "server2"; then
    check "menu lists server2" "ok"
else
    check "menu lists server2" "fail"
fi

if echo "$output" | grep -q "myserver"; then
    check "menu lists myserver" "ok"
else
    check "menu lists myserver" "fail"
fi

# invalid text input → "Unrecognized host"
output=$(echo "badvalue" | "$BINARY" -f "$FIXTURE" 2>&1 || true)
if echo "$output" | grep -q "Unrecognized host"; then
    check "invalid text → Unrecognized host" "ok"
else
    check "invalid text → Unrecognized host" "fail"
fi

# number 0 (out of range) → "Unrecognized host"
output=$(echo "0" | "$BINARY" -f "$FIXTURE" 2>&1 || true)
if echo "$output" | grep -q "Unrecognized host"; then
    check "0 (out of range) → Unrecognized host" "ok"
else
    check "0 (out of range) → Unrecognized host" "fail"
fi

# number too large → "Unrecognized host"
output=$(echo "999" | "$BINARY" -f "$FIXTURE" 2>&1 || true)
if echo "$output" | grep -q "Unrecognized host"; then
    check "999 (out of range) → Unrecognized host" "ok"
else
    check "999 (out of range) → Unrecognized host" "fail"
fi

# missing file → error message
output=$(echo "c" | "$BINARY" -f /nonexistent/path/ssh_config 2>&1 || true)
if echo "$output" | grep -q "Error"; then
    check "missing file shows error" "ok"
else
    check "missing file shows error" "fail"
fi

# valid selection prints connecting message (SSH itself may fail, that is expected)
output=$(echo "1" | "$BINARY" -f "$FIXTURE" 2>&1 || true)
if echo "$output" | grep -q "Connecting soon"; then
    check "valid selection shows connecting message" "ok"
else
    check "valid selection shows connecting message" "fail"
fi

echo ""
printf "Results: %d passed, %d failed\n" "$PASS" "$FAIL"

[ $FAIL -eq 0 ]
