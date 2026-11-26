# Requisitos adicionais — expansao do almoxarifado

Esta documentacao complementa as User Stories e os cartoes CRC originais do C5. Nao substitui os artefatos anteriores.

## User Stories adicionais

### US07 — Salvar e recuperar dados
Como almoxarife, quero que os dados sejam mantidos entre as execucoes para nao perder o cadastro ao fechar o programa.

Criterios de aceitacao:
- Salvar os cadastros e movimentacoes em arquivo.
- Recuperar os dados ao iniciar.
- Rejeitar arquivos corrompidos com mensagem de erro.

### US08 — Gerenciar usuarios
Como almoxarife, quero cadastrar usuarios e atribuir perfis para identificar quem realiza cada operacao.

Criterios de aceitacao:
- Cadastrar usuario com identificador unico.
- Alterar o perfil de um usuario cadastrado.
- Impedir operacoes protegidas de usuarios sem permissao.

### US09 — Acompanhar movimentacoes
Como gerente, quero consultar entradas e saidas para entender como o estoque foi utilizado.

Criterios de aceitacao:
- Guardar data, material, quantidade e responsavel.
- Listar as movimentacoes em ordem de registro.
- Consultar movimentacoes por periodo.

### US10 — Consultar emprestimos
Como funcionario, quero consultar emprestimos ativos para saber quais ferramentas ainda nao foram devolvidas.

Criterios de aceitacao:
- Registrar retirada com usuario e data.
- Impedir duas retiradas simultaneas da mesma ferramenta.
- Registrar devolucao e manter historico.

### US11 — Emitir relatorios
Como gerente, quero gerar relatorios para acompanhar materiais e ferramentas.

Criterios de aceitacao:
- Listar estoque baixo e ferramentas emprestadas.
- Consultar ranking de movimentacoes.
- Exportar relatorios em CSV.

### US12 — Fazer backup
Como almoxarife, quero guardar copias do estado para recuperar dados em caso de erro.

Criterios de aceitacao:
- Criar uma copia dos dados salvos.
- Listar backups existentes.
- Validar os arquivos antes de restaurar.

## Novas classes e colaboracoes (resumo CRC)

| Classe | Papel principal | Colabora com |
|---|---|---|
| `EstadoSistema` | Agrupa e valida os modulos | `Estoque`, `GestorUsuarios`, `GerenciadorEmprestimos`, `HistoricoMovimentacoes` |
| `GestorUsuarios` | Cadastra usuarios e controla perfis | `Usuario`, `HistoricoMovimentacoes` |
| `Movimentacao` | Representa uma entrada ou saida | `Material`, `HistoricoMovimentacoes` |
| `HistoricoMovimentacoes` | Registra entradas e saidas | `Estoque`, `GestorUsuarios`, `Movimentacao` |
| `GerenciadorEmprestimos` | Controla emprestimos e devolucoes | `Emprestimo`, `Estoque`, `GestorUsuarios` |
| `Persistencia` | Salva e recarrega estado | `EstadoSistema`, `Backup` |
| `Backup` | Faz e restaura copias | `Persistencia`, `EstadoSistema` |
| `RelatoriosAvancados` | Consultas e resumos | `EstadoSistema`, `HistoricoMovimentacoes` |
| `ExportadorCsv` | Grava relatorios CSV | `EstadoSistema`, `RelatoriosAvancados` |
| `ImportadorCsv` | Importa cadastros CSV | `EstadoSistema`, `Estoque` |
| `InterfaceTerminal` | Recebe comandos do usuario | `EstadoSistema`, `Persistencia`, `RelatoriosAvancados` |
