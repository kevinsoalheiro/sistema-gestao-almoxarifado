#include <doctest/doctest.h>
#include "InterfaceTerminal.hpp"
#include "Persistencia.hpp"
#include <filesystem>
#include <sstream>

TEST_CASE("InterfaceTerminal cadastra movimenta e recupera os dados") {
    const auto pasta = std::filesystem::temp_directory_path() / "almox_teste_interface";
    std::filesystem::remove_all(pasta);
    const auto arquivo = pasta / "estado.txt";
    // Opcoes 1, 3, 5, 6 e 0 com os campos pedidos no menu.
    std::istringstream entradas(
        "1\n1\n10\nParafuso\n20\n5\n"
        "5\n1\n10\n5\n2026-11-24\nReposicao\n"
        "6\n1\n10\n3\n2026-11-25\nConsumo\n"
        "3\n0\n");
    std::ostringstream saidas, erros;
    InterfaceTerminal interface(arquivo, entradas, saidas, erros);
    interface.executar();
    CHECK(erros.str().empty());
    CHECK(saidas.str().find("Material cadastrado") != std::string::npos);
    CHECK(saidas.str().find("Parafuso: 22") != std::string::npos);
    CHECK(interface.estado().historico().quantidade() == 2);
    CHECK(Persistencia::carregar(arquivo).estoque().buscarMaterial(10)->getQuantidade() == 22);
    std::filesystem::remove_all(pasta);
}

TEST_CASE("InterfaceTerminal trata erro de operacao e continua") {
    const auto pasta = std::filesystem::temp_directory_path() / "almox_teste_interface_erro";
    std::filesystem::remove_all(pasta);
    const auto arquivo = pasta / "estado.txt";
    std::istringstream entradas(
        "1\n1\n10\nParafuso\n1\n1\n"
        "6\n1\n10\n10\n2026-11-24\nExcedente\n"
        "3\n0\n");
    std::ostringstream saidas, erros;
    InterfaceTerminal interface(arquivo, entradas, saidas, erros);
    interface.executar();
    CHECK(erros.str().find("Nao ha material suficiente") != std::string::npos);
    CHECK(interface.estado().estoque().listarMateriais().at(0).getQuantidade() == 1);
    CHECK(saidas.str().find("Programa encerrado") != std::string::npos);
    std::filesystem::remove_all(pasta);
}
