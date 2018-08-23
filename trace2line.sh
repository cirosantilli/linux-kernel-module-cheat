#!/usr/bin/env bash
set -eu
common_gem5="$1"
shift
common_trace_txt_file="$1"
shift
common_addr2line="$1"
shift
common_vmlinux="$1"
shift
common_run_dir="$1"
shift
(
  if "$common_gem5"; then
    sed -r 's/^.* (0x[^. ]*)[. ].*/\1/' "$common_trace_txt_file"
  else
    sed -r 's/.*pc=//' "$common_trace_txt_file"
  fi
) | \
  xargs "${common_addr2line}" -e "${common_vmlinux}" -fp | \
  sed -E "s|at ${common_vmlinux}/(\./\|)||" | \
  uniq -c \
> "${common_run_dir}/trace-lines.txt"
