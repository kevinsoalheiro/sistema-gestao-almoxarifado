# Sistema de Gestão de Almoxarifado e Ferramentas

## Integrnte
- Kevin Alves Soalheiro

## Descrição e motivação
O projeto propõe um sistema em C++ para organizar o controle de materiais e ferramentas de um almoxarifado como cadastrar itens, consultar quantidades disponíveis, registrar empréstimos e devoluções de ferramentas e emitir relatórios simples

Como o registro manual pode dificultar o acompanhamento de estoques e de ferramentas retiradas, utiliza-se o sistema para que a busca torne essas informações mais organizadas e de facíl identificação

## Objetivos
- Cadastrar materiais, ferramentas e usuários.
- Consultar a disponibilidade dos itens.
- Registrar empréstimos e devoluções de ferramentas.
- Gerar relatórios básicos do estoque.

## Estrutura do projeto
- `design/`: User Stories e cartões CRC (C5).
- `include/`: contratos das classes em arquivos `.hpp` (C6).
- `src/`: implementações das classes (etapas seguintes).
- `tests/`: testes do projeto (etapas seguintes).
- `Doxyfile`: configuração da documentação Doxygen.

## Documentação
Os métodos e classes estão descritos com comentários Doxygen nos arquivos de `include/`.

Para gerar a documentação HTML, com o Doxygen instalado, execute na raiz do projeto:

```bash
doxygen Doxyfile
```

A documentação será criada em `docs/doxygen/html/`.
