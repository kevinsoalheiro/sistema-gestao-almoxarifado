#include <doctest/doctest.h>
#include "Material.hpp"
#include <stdexcept>

TEST_CASE("Material armazena e movimenta quantidades") {
    Material material(10, "Parafuso", 2, 5);
    CHECK(material.getCodigo() == 10);
    CHECK(material.getNome() == "Parafuso");
    CHECK(material.getQuantidade() == 2);
    CHECK(material.estoqueBaixo());

    material.adicionar(5);
    CHECK(material.getQuantidade() == 7);
    CHECK_FALSE(material.estoqueBaixo());
    material.retirar(2);
    CHECK(material.getQuantidade() == 5);
    CHECK(material.estoqueBaixo());

    CHECK_THROWS_AS(material.retirar(20), std::invalid_argument);
    CHECK_THROWS_AS(material.adicionar(0), std::invalid_argument);
    CHECK_THROWS_AS(Material(20, "Invalido", -1, 1), std::invalid_argument);
}
