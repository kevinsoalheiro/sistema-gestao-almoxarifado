#include <doctest/doctest.h>
#include "Ferramenta.hpp"
#include <stdexcept>

TEST_CASE("Ferramenta pode ser emprestada e devolvida") {
    Ferramenta ferramenta(20, "Martelo");
    CHECK(ferramenta.getCodigo() == 20);
    CHECK(ferramenta.getNome() == "Martelo");
    CHECK(ferramenta.estaDisponivel());

    ferramenta.emprestar();
    CHECK_FALSE(ferramenta.estaDisponivel());
    CHECK_THROWS_AS(ferramenta.emprestar(), std::logic_error);
    ferramenta.devolver();
    CHECK(ferramenta.estaDisponivel());
    CHECK_THROWS_AS(ferramenta.devolver(), std::logic_error);
}
