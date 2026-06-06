Fase 5 — Sistema de arquivos

Compilar:
make clean
make

Teste principal da fase 5:
./dispatcher processes.txt files_phase5.txt string.txt

Teste de permissoes:
./dispatcher processes_phase5_permissions.txt files_phase5_permissions.txt string_phase5_permissions.txt

O que foi implementado:
- leitura inicial do disco via InputParser + FileSystemManager;
- alocacao contigua com first-fit;
- createFile();
- deleteFile();
- permissoes:
  * processo de tempo real pode deletar qualquer arquivo;
  * processo de usuario so pode deletar arquivo criado por ele;
  * qualquer processo existente pode criar arquivo se houver espaco contiguo suficiente;
- impressao de sucesso/falha por operacao;
- impressao do mapa final do disco com blocos livres como 0.

Observacao importante:
O arquivo files.txt original da especificacao possui uma aparente inconsistencia no exemplo:
a ultima linha aparece como codigo 0, que pela propria especificacao significa criar, mas a saida
exemplificada trata essa operacao como deletar. Por isso, o teste files_phase5.txt usa codigo 1 na
ultima operacao para reproduzir a saida descrita no exemplo.
