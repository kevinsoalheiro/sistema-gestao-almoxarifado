#include <doctest/doctest.h>
#include "Usuario.hpp"

TEST_CASE("Usuario guarda nome e identificador") {
    Usuario usuario(1, "Ana", TipoUsuario::FUNCIONARIO);
    CHECK(usuario.getId() == 1);
    CHECK(usuario.getNome() == "Ana");
    CHECK(usuario.getTipo() == TipoUsuario::FUNCIONARIO);
    CHECK_FALSE(usuario.podeGerenciarEstoque());

    Usuario almoxarife(2, "Bia", TipoUsuario::ALMOXARIFE);
    CHECK(almoxarife.podeGerenciarEstoque());
    Usuario gerente(3, "Carlos", TipoUsuario::GERENTE);
    CHECK(gerente.podeGerenciarEstoque());
}
