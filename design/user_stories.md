# C5 — User Stories

**Projeto:** Sistema de Gestão de Almoxarifado e Ferramentas  
**Equipe:** Kevin Alves Soalheiro e Thiago Torres

## US01 — Cadastrar material
**Como** almoxarife, **quero** cadastrar materiais **para** controlar o estoque.

**Critérios de aceitação:**
1. Informar código, nome e quantidade do material.
2. Impedir cadastro com código repetido.
3. Confirmar quando o cadastro for realizado.

## US02 — Cadastrar ferramenta
**Como** almoxarife, **quero** cadastrar ferramentas **para** acompanhar sua disponibilidade.

**Critérios de aceitação:**
1. Informar código e nome da ferramenta.
2. Impedir cadastro com código repetido.
3. Registrar a ferramenta como disponível ao cadastrar.

## US03 — Consultar estoque
**Como** funcionário, **quero** consultar o estoque **para** saber o que está disponível.

**Critérios de aceitação:**
1. Listar os materiais cadastrados e suas quantidades.
2. Listar as ferramentas e sua disponibilidade.
3. Permitir buscar um item pelo código.

## US04 — Retirar ferramenta
**Como** funcionário, **quero** retirar uma ferramenta **para** usá-la no trabalho.

**Critérios de aceitação:**
1. Informar o funcionário e a ferramenta da retirada.
2. Permitir a retirada somente se a ferramenta estiver disponível.
3. Registrar a retirada e marcar a ferramenta como indisponível.

## US05 — Devolver ferramenta
**Como** funcionário, **quero** devolver uma ferramenta **para** liberá-la para outros usos.

**Critérios de aceitação:**
1. Localizar uma retirada ainda não devolvida.
2. Registrar a devolução da ferramenta.
3. Marcar a ferramenta como disponível novamente.

## US06 — Consultar relatório
**Como** gerente, **quero** consultar um relatório **para** acompanhar o almoxarifado.

**Critérios de aceitação:**
1. Mostrar os materiais com suas quantidades.
2. Destacar os materiais com quantidade igual ou inferior ao estoque mínimo.
3. Mostrar as ferramentas que estão emprestadas.
