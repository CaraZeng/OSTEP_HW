#!/usr/bin/env bash
set -euo pipefail
cc -O2 -std=c11 -Wall -Wextra tlb.c -o tlb
printf 'pages,trials,ns_per_access\n' > tlb_results.csv
for pages in 1 2 4 8 16 32 64 128 256 512 1024 2048 4096; do
  trials=$(( 20000000 / pages ))
  if (( trials < 2000 )); then trials=2000; fi
  ./tlb "$pages" "$trials" >> tlb_results.csv
done
