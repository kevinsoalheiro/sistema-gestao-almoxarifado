#include <doctest/doctest.h>
#include "Emprestimo.hpp"
#include "Estoque.hpp"
#include "Ferramenta.hpp"
#include "Material.hpp"
#include "Usuario.hpp"

#include <limits>
#include <stdexcept>

TEST_CASE("Usuario rejeita identificador nome e tipo invalidos") {
    CHECK_THROWS_AS(Usuario(0, "Ana", TipoUsuario::FUNCIONARIO), std::invalid_argument);
    CHECK_THROWS_AS(Usuario(1, "  ", TipoUsuario::FUNCIONARIO), std::invalid_argument);
    CHECK_THROWS_AS(Usuario(1, "Ana", static_cast<TipoUsuario>(99)), std::invalid_argument);
    CHECK_NOTHROW(Usuario(1, "Ana", TipoUsuario::GERENTE));
}

TEST_CASE("Material valida dados e nao altera saldo em caso de erro") {
    CHECK_THROWS_AS(Material(0, "Parafuso", 1, 0), std::invalid_argument);
    CHECK_THROWS_AS(Material(5, "\t", 1, 0), std::invalid_argument);
    CHECK_THROWS_AS(Material(5, "Parafuso", -1, 0), std::invalid_argument);
    CHECK_THROWS_AS(Material(5, "Parafuso", 1, -1), std::invalid_argument);

    Material material(5, "Parafuso", 3, 2);
    CHECK_THROWS_AS(material.retirar(0), std::invalid_argument);
    CHECK_THROWS_AS(material.retirar(-2), std::invalid_argument);
    CHECK_THROWS_AS(material.retirar(4), std::invalid_argument);
    CHECK_THROWS_AS(material.adicionar(-1), std::invalid_argument);
    CHECK(material.getQuantidade() == 3);

    Material cheio(6, "Porca", std::numeric_limits<int>::max(), 0);
    CHECK_THROWS_AS(cheio.adicionar(1), std::overflow_error);
    CHECK(cheio.getQuantidade() == std::numeric_limits<int>::max());
}

TEST_CASE("Ferramenta valida cadastro e protege transicoes") {
    CHECK_THROWS_AS(Ferramenta(0, "Martelo"), std::invalid_argument);
    CHECK_THROWS_AS(Ferramenta(10, "  "), std::invalid_argument);
    Ferramenta ferramenta(10, "Martelo");
    CHECK_THROWS_AS(ferramenta.devolver(), std::logic_error);
    CHECK(ferramenta.estaDisponivel());
    ferramenta.emprestar();
    CHECK_THROWS_AS(ferramenta.emprestar(), std::logic_error);
    CHECK_FALSE(ferramenta.estaDisponivel());
    ferramenta.devolver();
    CHECK(ferramenta.estaDisponivel());
}

TEST_CASE("Emprestimo valida identificadores e datas") {
    CHECK_THROWS_AS(Emprestimo(0, 10, "2026-11-24"), std::invalid_argument);
    CHECK_THROWS_AS(Emprestimo(1, 0, "2026-11-24"), std::invalid_argument);
    CHECK_THROWS_AS(Emprestimo(1, 10, "24/11/2026"), std::invalid_argument);
    CHECK_THROWS_AS(Emprestimo(1, 10, "2026-13-01"), std::invalid_argument);
    CHECK_THROWS_AS(Emprestimo(1, 10, "2026-02-29"), std::invalid_argument);
    CHECK_THROWS_AS(Emprestimo(1, 10, "0000-01-01"), std::invalid_argument);
    CHECK_NOTHROW(Emprestimo(1, 10, "2024-02-29"));

    Emprestimo emprestimo(1, 10, "2026-11-24");
    CHECK_THROWS_AS(emprestimo.registrarDevolucao("2026-11-23"), std::invalid_argument);
    CHECK_THROWS_AS(emprestimo.registrarDevolucao("2026-11-31"), std::invalid_argument);
    CHECK(emprestimo.estaAberto());
    emprestimo.registrarDevolucao("2026-11-24");
    CHECK_FALSE(emprestimo.estaAberto());
    CHECK_THROWS_AS(emprestimo.registrarDevolucao("2026-11-25"), std::logic_error);
}

TEST_CASE("Estoque nao aceita codigos repetidos e devolve nullptr se nao existir") {
    Estoque estoque;
    estoque.cadastrarMaterial(Material(1, "Parafuso", 3, 1));
    estoque.cadastrarFerramenta(Ferramenta(2, "Martelo"));
    CHECK_THROWS_AS(estoque.cadastrarMaterial(Material(1, "Outro", 3, 1)), std::invalid_argument);
    CHECK_THROWS_AS(estoque.cadastrarFerramenta(Ferramenta(2, "Outra")), std::invalid_argument);
    CHECK(estoque.listarMateriais().size() == 1);
    CHECK(estoque.listarFerramentas().size() == 1);
    CHECK(estoque.buscarMaterial(-1) == nullptr);
    CHECK(estoque.buscarFerramenta(-1) == nullptr);
}
