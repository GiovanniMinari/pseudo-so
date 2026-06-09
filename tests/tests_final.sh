#!/bin/sh
set -e

make clean
make

printf '\n=== Teste 1: exemplo principal + sistema de arquivos ===\n'
./dispatcher processes.txt files.txt string.txt > output_final_example.txt
cat output_final_example.txt

printf '\n=== Teste 2: permissoes de arquivos ===\n'
./dispatcher processes_phase5_permissions.txt files_phase5_permissions.txt string_phase5_permissions.txt > output_final_permissions.txt
cat output_final_permissions.txt

printf '\n=== Teste 3: aging do escalonador ===\n'
./dispatcher processes_phase6_aging.txt files_phase6_empty.txt string_phase6_aging.txt > output_final_aging.txt
cat output_final_aging.txt
