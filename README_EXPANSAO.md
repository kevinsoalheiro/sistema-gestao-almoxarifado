# Expansao do Sistema de Gestao de Almoxarifado e Ferramentas


## Funcionalidades acrescentadas

1. **Persistencia:** grava e recupera materiais, ferramentas, usuarios, emprestimos e movimentacoes em `dados/estado.txt`.
2. **Gestao de usuarios:** cadastro, consulta e alteracao de perfis; conferencias de permissao em operacoes de estoque.
3. **Historico de movimentacoes:** registra entradas e saidas de materiais, com data, quantidade, responsavel e observacao.
4. **Gerenciamento de emprestimos:** registra retirada e devolucao; verifica disponibilidade e usuario; consulta historico e pendencias.
5. **Relatorios:** estoque baixo, emprestimos, resumo geral, consulta por nome, ranking de materiais, periodo e usuario.
6. **Menu ampliado:** operacoes do sistema com validacoes e mensagens de erro, incluindo geracao de backups e CSV.
7. **Backup/restauracao:** copias dos dados com validacao antes de restaurar.
8. **CSV:** exportacao de materiais, ferramentas, usuarios, emprestimos e movimentacoes; importacao de materiais e ferramentas.
9. **Auditoria:** verificacoes de consistencia e alertas do estoque.

## Organizacao

- `include/`: contratos e classes de dominio e infraestrutura.
- `src/`: implementacoes, menu (`InterfaceTerminal`) e `main.cpp`.
- `tests/`: testes anteriores do C9 e testes novos da expansao.
- `design/expansao.md`: requisitos resumidos e novas colaboracoes.
- `dados/`: dados locais e backups; so `dados/.gitkeep` e versionado.