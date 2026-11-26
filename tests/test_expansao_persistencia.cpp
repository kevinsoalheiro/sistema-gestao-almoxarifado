#include <doctest/doctest.h>
#include "Backup.hpp"
#include "Persistencia.hpp"
#include <filesystem>
#include <fstream>
#include <stdexcept>

TEST_CASE("Persistencia preserva usuarios ferramentas materiais e historico") {
    const auto base = std::filesystem::temp_directory_path() / "almox_teste_persistencia";
    std::filesystem::remove_all(base);
    EstadoSistema e;
    e.usuarios().cadastrar(1, "Ana Maria", TipoUsuario::ALMOXARIFE);
    e.estoque().cadastrarMaterial(Material(10, "Parafuso", 10, 3));
    e.estoque().cadastrarFerramenta(Ferramenta(20, "Martelo"));
    e.historico().registrarSaida(e.estoque(), e.usuarios(), 10, 4, 1,
                                 "2026-11-24", "Manutencao");
    e.emprestimos().emprestar(e.estoque(), e.usuarios(), 20, 1, "2026-11-24");
    Persistencia::salvar(e, base / "estado.txt");
    CHECK(Persistencia::existeArquivo(base / "estado.txt"));
    EstadoSistema carregado = Persistencia::carregar(base / "estado.txt");
    CHECK(carregado.estoque().buscarMaterial(10)->getQuantidade() == 6);
    CHECK(carregado.estoque().buscarMaterial(10)->getEstoqueMinimo() == 3);
    CHECK(carregado.usuarios().buscar(1)->getNome() == "Ana Maria");
    CHECK_FALSE(carregado.estoque().buscarFerramenta(20)->estaDisponivel());
    CHECK(carregado.emprestimos().quantidadeAbertos() == 1);
    CHECK(carregado.historico().quantidade() == 1);
    CHECK_NOTHROW(carregado.validarIntegridade());
    std::filesystem::remove_all(base);
}

TEST_CASE("Persistencia nao aceita arquivo truncado e backup restaura dados") {
    const auto pasta = std::filesystem::temp_directory_path() / "almox_teste_backup";
    std::filesystem::remove_all(pasta);
    EstadoSistema e;
    e.usuarios().cadastrar(1, "Admin", TipoUsuario::ALMOXARIFE);
    e.estoque().cadastrarMaterial(Material(10, "Porca", 4, 1));
    const auto dados = pasta / "dados.txt";
    Persistencia::salvar(e, dados);
    const auto arquivoBackup = Backup::criar(dados, pasta / "backups");
    CHECK(std::filesystem::exists(arquivoBackup));
    CHECK(Backup::listar(pasta / "backups").size() == 1);
    e.estoque().buscarMaterial(10)->adicionar(10);
    Persistencia::salvar(e, dados);
    CHECK(Persistencia::carregar(dados).estoque().buscarMaterial(10)->getQuantidade() == 14);
    Backup::restaurar(arquivoBackup, dados);
    CHECK(Persistencia::carregar(dados).estoque().buscarMaterial(10)->getQuantidade() == 4);
    { std::ofstream arquivo(pasta / "ruim.txt"); arquivo << "ALMOX1\nUSUARIOS 2\n"; }
    CHECK_THROWS_AS(Persistencia::carregar(pasta / "ruim.txt"), std::runtime_error);
    CHECK_THROWS_AS(Persistencia::carregar(pasta / "inexistente.txt"), std::runtime_error);
    std::filesystem::remove_all(pasta);
}
