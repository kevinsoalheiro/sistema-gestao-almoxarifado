#include <doctest/doctest.h>
#include "EstadoSistema.hpp"
#include <stdexcept>

TEST_CASE("GerenciadorEmprestimos empresta e devolve mantendo historico") {
    EstadoSistema e;
    e.usuarios().cadastrar(1, "Ana", TipoUsuario::FUNCIONARIO);
    e.estoque().cadastrarFerramenta(Ferramenta(20, "Martelo"));
    auto& g = e.emprestimos();
    g.emprestar(e.estoque(), e.usuarios(), 20, 1, "2026-11-24");
    CHECK_FALSE(e.estoque().buscarFerramenta(20)->estaDisponivel());
    CHECK(g.quantidadeAbertos() == 1);
    CHECK(g.buscarAberto(20) != nullptr);
    CHECK_THROWS_AS(g.emprestar(e.estoque(), e.usuarios(), 20, 1,
                               "2026-11-24"), std::logic_error);
    CHECK_THROWS_AS(g.devolver(e.estoque(), 20, "2026-11-23"), std::invalid_argument);
    CHECK(g.quantidadeAbertos() == 1);
    g.devolver(e.estoque(), 20, "2026-11-25");
    CHECK(e.estoque().buscarFerramenta(20)->estaDisponivel());
    CHECK(g.quantidadeAbertos() == 0);
    CHECK(g.listar()[0].getDataDevolucao() == "2026-11-25");
    CHECK(g.porUsuario(1).size() == 1);
    CHECK_THROWS_AS(g.devolver(e.estoque(), 20, "2026-11-25"), std::logic_error);
    CHECK_NOTHROW(e.validarIntegridade());
}

TEST_CASE("GerenciadorEmprestimos valida usuarios e ferramentas") {
    EstadoSistema e;
    e.usuarios().cadastrar(1, "Ana", TipoUsuario::FUNCIONARIO);
    e.estoque().cadastrarFerramenta(Ferramenta(20, "Martelo"));
    CHECK_THROWS_AS(e.emprestimos().emprestar(e.estoque(), e.usuarios(),
                      20, 99, "2026-11-24"), std::invalid_argument);
    CHECK_THROWS_AS(e.emprestimos().emprestar(e.estoque(), e.usuarios(),
                      99, 1, "2026-11-24"), std::invalid_argument);
    CHECK(e.emprestimos().quantidade() == 0);
}
