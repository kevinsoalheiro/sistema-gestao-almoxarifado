#include <doctest/doctest.h>
#include "Estoque.hpp"
#include <stdexcept>

TEST_CASE("Estoque cadastra e consulta materiais e ferramentas") {
    Estoque estoque;
    CHECK(estoque.listarMateriais().empty());
    CHECK(estoque.listarFerramentas().empty());

    estoque.cadastrarMaterial(Material(10, "Parafuso", 5, 2));
    estoque.cadastrarFerramenta(Ferramenta(20, "Martelo"));
    CHECK(estoque.buscarMaterial(10) != nullptr);
    CHECK(estoque.buscarFerramenta(20) != nullptr);
    CHECK(estoque.buscarMaterial(99) == nullptr);
    CHECK(estoque.buscarFerramenta(99) == nullptr);
    CHECK(estoque.listarMateriais().size() == 1);
    CHECK(estoque.listarFerramentas().size() == 1);

    CHECK_THROWS_AS(estoque.cadastrarMaterial(Material(10, "Outro", 1, 0)),
                    std::invalid_argument);
    CHECK_THROWS_AS(estoque.cadastrarFerramenta(Ferramenta(20, "Outra")),
                    std::invalid_argument);
}
