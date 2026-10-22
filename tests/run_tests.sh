#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")/.."
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT

${CC:-gcc} -std=c11 -Wall -Wextra -Wpedantic -Werror -g src/*.c -o "$tmp/hackvm"
mkdir "$tmp/simple"
cp tests/simple/*.vm "$tmp/simple/"
for input in "$tmp"/simple/*.vm; do
    name=$(basename "$input" .vm)
    "$tmp/hackvm" "$input"
    diff -u "tests/simple/$name.expected.asm" "$tmp/simple/$name.asm"
    echo "PASS: $name"
done

invalid=(
    'bogus'
    'push'
    'push constant'
    'push nowhere 0'
    'push add 0'
    'push constant -1'
    'push constant 32768'
    'push constant 1x'
    'pop constant 0'
    'push pointer 2'
    'pop temp 8'
    'push static 240'
    'add 1'
    'label 2bad'
    'goto'
    'call Foo -1'
    'function Foo 1 extra'
    'return 0'
    'local'
)
for command in "${invalid[@]}"; do
    printf '%s\n' "$command" > "$tmp/Invalid.vm"
    if "$tmp/hackvm" "$tmp/Invalid.vm" > /dev/null 2> "$tmp/error"; then
        echo "FAIL: accepted $command"
        exit 1
    fi
    grep -q 'Invalid.vm:1: invalid command:' "$tmp/error"
    test ! -e "$tmp/Invalid.asm"
done
echo 'PASS: invalid commands'

mkdir "$tmp/program" "$tmp/empty" "$tmp/program/Ignore.vm"
printf 'push constant 1\npop static 0\n' > "$tmp/program/B.vm"
printf 'push constant 2\npop static 0\n' > "$tmp/program/A.vm"
printf '// ignored\r\n\tpush\tconstant 7  // value\r\npush constant 8\r\nadd' > "$tmp/Whitespace.vm"
"$tmp/hackvm" "$tmp/Whitespace.vm"
diff -u tests/simple/BasicTest.expected.asm "$tmp/Whitespace.asm"
"$tmp/hackvm" "$tmp/program/"
cp "$tmp/program/program.asm" "$tmp/first.asm"
(cd "$tmp/program" && "$tmp/hackvm" .)
diff -u "$tmp/first.asm" "$tmp/program/program.asm"
head -n 4 "$tmp/program/program.asm" > "$tmp/prefix"
printf '@256\nD=A\n@SP\nM=D\n' > "$tmp/want"
diff -u "$tmp/want" "$tmp/prefix"
grep -q '@Sys.init' "$tmp/program/program.asm"
grep -q '@A.0' "$tmp/program/program.asm"
grep -q '@B.0' "$tmp/program/program.asm"
if "$tmp/hackvm" "$tmp/empty" > /dev/null 2>&1; then
    echo 'FAIL: accepted empty directory'
    exit 1
fi
echo 'PASS: paths, directories, whitespace'

if [ "$#" -eq 0 ]; then
    exit 0
fi
suite=$(cd "$1" && pwd)
classpath="$suite/tools/bin/classes:$suite/tools/bin/lib/*"
run_cpu() {
    local result
    if ! result=$(java -Djava.awt.headless=true -cp "$classpath" CPUEmulatorMain "$1" 2>&1); then
        printf '%s\n' "$result"
        exit 1
    fi
    printf '%s\n' "$result"
    if [[ "$result" != *'Comparison ended successfully'* ]]; then
        echo "FAIL: $1"
        exit 1
    fi
}
mkdir "$tmp/official"
cp -R "$suite/projects/07" "$suite/projects/08" "$tmp/official/"
while IFS= read -r test; do
    input="${test%.tst}.vm"
    if [ ! -f "$input" ]; then
        input=$(dirname "$test")
    fi
    "$tmp/hackvm" "$input"
    run_cpu "$test"
done < <(find "$tmp/official" -name '*.tst' ! -iname '*vme*' | sort)
cp -R tests/cpu "$tmp/cpu"
"$tmp/hackvm" "$tmp/cpu/SignedTest.vm"
run_cpu "$tmp/cpu/SignedTest.tst"
"$tmp/hackvm" "$tmp/cpu/ScopeTest"
run_cpu "$tmp/cpu/ScopeTest/ScopeTest.tst"
