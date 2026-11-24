#include <doctest/doctest.h>
#include "Emprestimo.hpp"
#include <stdexcept>

TEST_CASE("Emprestimo abre e fecha ao devolver") {
    Emprestimo emprestimo(1, 20, "2026-11-23");
    CHECK(emprestimo.getUsuarioId() == 1);
    CHECK(emprestimo.getFerramentaCodigo() == 20);
    CHECK(emprestimo.getDataRetirada() == "2026-11-23");
    CHECK(emprestimo.estaAberto());
    emprestimo.registrarDevolucao("2026-11-24");
    CHECK_FALSE(emprestimo.estaAberto());
    CHECK_THROWS_AS(emprestimo.registrarDevolucao("2026-11-25"),
                    std::logic_error);
}
