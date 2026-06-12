#!/bin/sh
set -e

make clean
make

echo "=== Teste 1: exemplo principal + sistema de arquivos ==="
./dispatcher tests/processes.txt tests/files_phase5.txt tests/string.txt > output_final_example.txt
cat output_final_example.txt

echo "=== Teste 2: permissoes de arquivos ==="
./dispatcher tests/processes_phase5_permissions.txt tests/files_phase5_permissions.txt tests/string_phase5_permissions.txt > output_final_permissions.txt
cat output_final_permissions.txt

echo "=== Teste 3: aging do escalonador ==="
./dispatcher tests/processes_phase6_aging.txt tests/files_phase6_empty.txt tests/string_phase6_aging.txt > output_final_aging.txt
cat output_final_aging.txt

echo "=== Teste 4: requisicao impossivel de recursos + operacao de arquivo negada ==="
./dispatcher tests/processes_impossible_resources.txt tests/files_impossible_resources.txt tests/string_impossible_resources.txt > output_final_impossible_resources.txt
cat output_final_impossible_resources.txt
