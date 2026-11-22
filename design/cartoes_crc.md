# C5 — Cartões CRC

**Projeto:** Sistema de Gestão de Almoxarifado e Ferramentas  
**Equipe:** Kevin Alves Soalheiro e Thiago Torres

## CRC01 — Usuario
**Responsabilidades:**
1. Guardar o identificador do usuário.
2. Guardar o nome do usuário.
3. Guardar o tipo de usuário (almoxarife, funcionário ou gerente).
4. Informar o tipo de usuário.
5. Verificar se o usuário tem permissão para uma operação.

**Colaborações:** Estoque, Emprestimo.

## CRC02 — Material
**Responsabilidades:**
1. Guardar o código do material.
2. Guardar o nome do material.
3. Guardar a quantidade disponível.
4. Guardar a quantidade mínima desejada.
5. Informar se a quantidade está igual ou abaixo do mínimo.

**Colaborações:** Estoque, Relatorio.

## CRC03 — Ferramenta
**Responsabilidades:**
1. Guardar o código da ferramenta.
2. Guardar o nome da ferramenta.
3. Guardar seu estado de disponibilidade.
4. Marcar a ferramenta como emprestada.
5. Marcar a ferramenta como disponível após a devolução.

**Colaborações:** Estoque, Emprestimo.

## CRC04 — Estoque
**Responsabilidades:**
1. Cadastrar materiais.
2. Cadastrar ferramentas.
3. Impedir códigos duplicados em cada categoria de item.
4. Buscar materiais e ferramentas pelo código.
5. Listar materiais com suas quantidades.
6. Listar ferramentas com sua disponibilidade.

**Colaborações:** Material, Ferramenta, Relatorio.

## CRC05 — Emprestimo
**Responsabilidades:**
1. Identificar o funcionário da retirada.
2. Identificar a ferramenta retirada.
3. Registrar a data da retirada.
4. Registrar a devolução.
5. Informar se a retirada ainda está aberta.
6. Impedir devolução duplicada.

**Colaborações:** Usuario, Ferramenta, Estoque.

## CRC06 — Relatorio
**Responsabilidades:**
1. Consultar os dados do estoque.
2. Listar materiais com suas quantidades.
3. Identificar materiais com estoque baixo.
4. Listar ferramentas emprestadas.
5. Apresentar o resultado de forma organizada.

**Colaborações:** Estoque, Material, Ferramenta.
