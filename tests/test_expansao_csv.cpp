#include <doctest/doctest.h>
#include "ExportadorCsv.hpp"
#include "ImportadorCsv.hpp"
#include <filesystem>
#include <fstream>
#include <stdexcept>

TEST_CASE("CSV exporta e reimporta materiais e ferramentas") {
    const auto pasta = std::filesystem::temp_directory_path() / "almox_teste_csv";
    std::filesystem::remove_all(pasta);
    EstadoSistema e;
    e.estoque().cadastrarMaterial(Material(10, "Parafuso; 10mm", 20, 5));
    e.estoque().cadastrarFerramenta(Ferramenta(20, "Chave \"estrela\""));
    ExportadorCsv::materiais(e, pasta / "materiais.csv");
    ExportadorCsv::ferramentas(e, pasta / "ferramentas.csv");
    ExportadorCsv::usuarios(e, pasta / "usuarios.csv");
    ExportadorCsv::emprestimos(e, pasta / "emprestimos.csv");
    ExportadorCsv::movimentacoes(e, pasta / "movimentacoes.csv");
    EstadoSistema restaurado;
    CHECK(ImportadorCsv::importarMateriais(restaurado, pasta / "materiais.csv") == 1);
    CHECK(ImportadorCsv::importarFerramentas(restaurado, pasta / "ferramentas.csv") == 1);
    CHECK(restaurado.estoque().buscarMaterial(10)->getNome() == "Parafuso; 10mm");
    CHECK(restaurado.estoque().buscarFerramenta(20)->getNome() == "Chave \"estrela\"");
    CHECK_THROWS_AS(ImportadorCsv::importarMateriais(restaurado, pasta / "materiais.csv"), std::invalid_argument);
    CHECK(ImportadorCsv::separarCampos("1;\"A;B\";3").size() == 3);
    CHECK_THROWS_AS(ImportadorCsv::separarCampos("1;\"Sem fechar"), std::invalid_argument);
    std::filesystem::remove_all(pasta);
}

TEST_CASE("CSV invalido nao altera estoque") {
    const auto pasta = std::filesystem::temp_directory_path() / "almox_teste_csv_invalido";
    std::filesystem::remove_all(pasta);
    std::filesystem::create_directories(pasta);
    const auto caminho = pasta / "materiais.csv";
    { std::ofstream o(caminho);
      o << "codigo;nome;quantidade;minimo;estoque_baixo\n";
      o << "10;\"Parafuso\";10;2;nao\n";
      o << "11;\"Porca\";-1;2;sim\n";
    }
    EstadoSistema e;
    CHECK_THROWS_AS(ImportadorCsv::importarMateriais(e, caminho), std::invalid_argument);
    CHECK(e.estoque().listarMateriais().empty());
    std::filesystem::remove_all(pasta);
}
