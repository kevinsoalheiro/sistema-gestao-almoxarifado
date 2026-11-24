## C5 — Cartões CRC

**Projeto: Sistema de Gestão de Almoxarifado e Ferramentas**
**Integrante: Kevin Alves Soalheiro**

## CRC01 — Usuario

**Responsabilidades:**

Guardar o ID do usuário.

Guardar o nome do usuário.

Saber se o usuário é almoxarife, funcionário ou gerente.

Informar o nome e o tipo do usuário.

Verificar se o usuário pode gerenciar o estoque.

Colaborações: Estoque e Emprestimo.

## CRC02 — Material

**Responsabilidades:**

Guardar o código do material.

Guardar o nome do material.

Controlar a quantidade disponível.

Guardar a quantidade mínima que deve ter no estoque.

Avisar quando a quantidade estiver baixa.

Colaborações: Estoque e Relatorio.

## CRC03 — Ferramenta

**Responsabilidades:**:

Guardar o código da ferramenta.

Guardar o nome da ferramenta.

Informar se está disponível ou emprestada.

Marcar a ferramenta como emprestada quando alguém retirar.

Deixar a ferramenta disponível novamente quando for devolvida.

Colaborações: Estoque e Emprestimo.

## CRC04 — Estoque

**Responsabilidades:**

Cadastrar novos materiais.

Cadastrar novas ferramentas.

Evitar que dois materiais ou duas ferramentas tenham o mesmo código.

Procurar materiais e ferramentas pelo código.

Mostrar os materiais cadastrados e suas quantidades.

Mostrar quais ferramentas estão disponíveis ou emprestadas.

Colaborações: Material, Ferramenta e Relatorio.

## CRC05 — Emprestimo

**Responsabilidades:**

Guardar quem pegou a ferramenta.

Guardar qual ferramenta foi retirada.

Registrar a data da retirada.

Registrar quando a ferramenta for devolvida.

Informar se o empréstimo ainda está em aberto.

Evitar que uma mesma devolução seja registrada duas vezes.

Colaborações: Usuario, Ferramenta e Estoque.

## CRC06 — Relatorio

**Responsabilidades:**

Consultar as informações do estoque.

Mostrar os materiais e suas quantidades.

Mostrar quais materiais estão com estoque baixo.

Mostrar quais ferramentas estão emprestadas.

Organizar essas informações para facilitar a consulta.

Colaborações: Estoque, Material e Ferramenta.