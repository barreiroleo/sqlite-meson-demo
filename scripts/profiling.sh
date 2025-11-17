#!/bin/bash
GREEN="\033[0;32m"
NC="\033[0m" # No Color

PROCESS="$2"
CWD="$(cd "$(dirname "$0")/.." && pwd)"
OUTPUT_DIR="${CWD}/build/perf-data-$(date +%Y%m%d-%H%M%S)"

while [[ $# -gt 0 ]]; do
    case $1 in
    --listen)
        MODE="listen"
        shift
        ;;
    --launch)
        MODE="launch"
        shift
        ;;
    *)
        shift
        ;;
    esac
done

if [[ -z "$MODE" || -z "$PROCESS" ]]; then
    echo "Usage: $0 --listen|--launch <process_name>"
    echo "  --listen <process_name>  Listen for an existing process"
    echo "  --launch <process_name>  Launch and profile a new process"
    exit 1
fi

trace() {
    echo -e "${GREEN}$1${NC}"
}

tweak_kernel() {
    # kptr_restrict controls the visibility of kernel pointer addresses. Allow perf to resolve kernel symbols
    # perf_event_paranoid controls the level of access to perf events for unprivileged users.
    trace "Tweaking kernel..."
    sudo sh -c 'echo 0 > /proc/sys/kernel/kptr_restrict'
    sudo sh -c 'echo -1 > /proc/sys/kernel/perf_event_paranoid'
}

launch_and_record() {
    perf record -g -o ${OUTPUT_DIR}/perf.data --call-graph=dwarf ${CWD}/build/${PROCESS}
}

listen_and_record() {
    trace "Listening for process ${PROCESS}..."
    PID=""
    while [[ -z "$PID" ]]; do
        PID=$(pgrep ${PROCESS})
        if [[ -z "$PID" ]]; then
            sleep 0.5
        fi
    done
    trace "Recording process with ID ${PID}..."
    perf record -F 997 -e cpu-clock -e cs -a -g -o ${OUTPUT_DIR}/perf.data --call-graph=dwarf -p ${PID}
}

process() {
    cd ${OUTPUT_DIR}
    trace "Dump raw trace data"
    perf script -D -i perf.data >perf.trace
    trace "Generating flamegraph..."
    perf script report flamegraph -- --allow-download
    trace "Generating gecko report..."
    perf script report gecko
}

main() {
    mkdir -p ${OUTPUT_DIR};

    tweak_kernel

    if [[ "$MODE" == "launch" ]]; then
        launch_and_record
    elif [[ "$MODE" == "listen" ]]; then
        listen_and_record
    fi

    process

    trace "Output saved in ${OUTPUT_DIR}"
}
main
