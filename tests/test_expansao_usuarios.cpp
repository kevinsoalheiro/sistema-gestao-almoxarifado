#include <doctest/doctest.h>
#include "GestorUsuarios.hpp"
#include <stdexcept>

TEST_CASE("GestorUsuarios cadastra procura e altera perfis") {
    GestorUsuarios g;
    CHECK(g.quantidade() == 0);
    g.cadastrar(1, "Ana", TipoUsuario::ALMOXARIFE);
    g.cadastrar(2, "Bruno", TipoUsuario::FUNCIONARIO);
    CHECK(g.buscar(1)->getNome() == "Ana");
    CHECK(g.buscar(3) == nullptr);
    CHECK(g.quantidadePorPerfil(TipoUsuario::FUNCIONARIO) == 1);
    CHECK(g.buscarPorNome("an").size() == 1);
    CHECK_THROWS_AS(g.cadastrar(1, "Duplicado", TipoUsuario::GERENTE), std::invalid_argument);
    CHECK_THROWS_AS(g.exigirPermissaoEstoque(2), std::logic_error);
    CHECK_NOTHROW(g.exigirPermissaoEstoque(1));
    g.alterarPerfil(2, TipoUsuario::GERENTE);
    CHECK_NOTHROW(g.exigirPermissaoRelatorio(2));
    g.alterarNome(1, "Amanda");
    CHECK(g.buscar(1)->getNome() == "Amanda");
    g.excluir(1);
    CHECK_FALSE(g.existe(1));
    CHECK_THROWS_AS(g.excluir(100), std::invalid_argument);
}

TEST_CASE("GestorUsuarios recusa ID e perfil inexistentes") {
    GestorUsuarios g;
    CHECK_THROWS_AS(g.exigirCadastrado(44), std::invalid_argument);
    CHECK_THROWS_AS(g.alterarPerfil(44, TipoUsuario::GERENTE), std::invalid_argument);
    CHECK_THROWS_AS(g.alterarNome(44, "Novo"), std::invalid_argument);
    CHECK_THROWS_AS(g.cadastrar(0, "Teste", TipoUsuario::GERENTE), std::invalid_argument);
    g.cadastrar(1, "Funcionario", TipoUsuario::FUNCIONARIO);
    CHECK_THROWS_AS(g.exigirPermissaoRelatorio(1), std::logic_error);
}
