#include <doctest/doctest.h>
#include "Relatorio.hpp"
#include <string>

TEST_CASE("Relatorio lista os itens do estoque") {
    Estoque estoque;
    estoque.cadastrarMaterial(Material(10, "Parafuso", 5, 2));
    estoque.cadastrarMaterial(Material(11, "Porca", 1, 2));
    estoque.cadastrarFerramenta(Ferramenta(20, "Martelo"));
    estoque.buscarFerramenta(20)->emprestar();

    Relatorio relatorio;
    CHECK(relatorio.listarMateriais(estoque).find("Parafuso") != std::string::npos);
    CHECK(relatorio.listarEstoqueBaixo(estoque).find("Porca") != std::string::npos);
    CHECK(relatorio.listarEstoqueBaixo(estoque).find("Parafuso") == std::string::npos);
    CHECK(relatorio.listarFerramentasEmprestadas(estoque).find("Martelo") != std::string::npos);
}
