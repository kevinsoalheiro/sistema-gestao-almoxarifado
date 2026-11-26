#include <doctest/doctest.h>
#include "Auditoria.hpp"
#include "Backup.hpp"
#include "Data.hpp"
#include "EstadoSistema.hpp"
#include "ExportadorCsv.hpp"
#include "GestorUsuarios.hpp"
#include "ImportadorCsv.hpp"
#include "InterfaceTerminal.hpp"
#include "Persistencia.hpp"
#include "RelatoriosAvancados.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

TEST_CASE("Datas cobrem limites do calendario e intervalos") {
    CHECK(Data::valida("2000-02-29"));
    CHECK_FALSE(Data::valida("1900-02-29"));
    CHECK(Data::valida("2024-02-29"));
    CHECK_FALSE(Data::valida("2025-02-29"));
    CHECK_FALSE(Data::valida("2026-04-31"));
    CHECK_FALSE(Data::valida("2026-00-15"));
    CHECK_FALSE(Data::valida("2026-13-15"));
    CHECK_FALSE(Data::valida("0000-01-01"));
    CHECK_FALSE(Data::valida("2026-01-00"));
    CHECK_FALSE(Data::valida("2026/11/26"));
    CHECK_FALSE(Data::valida("2026-11-2x"));
    CHECK_FALSE(Data::valida("26-11-2026"));
    CHECK(Data::valida("2026-11-30"));
    CHECK(Data::anteriorOuIgual("2026-11-25", "2026-11-26"));
    CHECK(Data::anteriorOuIgual("2026-11-26", "2026-11-26"));
    CHECK_FALSE(Data::anteriorOuIgual("2026-11-27", "2026-11-26"));
    CHECK(Data::dentroDoPeriodo("2026-11-25", "2026-11-25", "2026-11-26"));
    CHECK(Data::dentroDoPeriodo("2026-11-26", "2026-11-25", "2026-11-26"));
    CHECK_FALSE(Data::dentroDoPeriodo("2026-11-24", "2026-11-25", "2026-11-26"));
    CHECK_FALSE(Data::dentroDoPeriodo("2026-11-27", "2026-11-25", "2026-11-26"));
    CHECK_THROWS_AS(Data::dentroDoPeriodo("2026-11-25", "2026-11-26", "2026-11-25"), std::invalid_argument);
    CHECK_THROWS_AS(Data::exigirValida("2026-02-30"), std::invalid_argument);
}

TEST_CASE("CSV rejeita registros malformados sem alterar o estoque") {
    const fs::path pasta = fs::temp_directory_path() / "almox_teste_branches_60_csv";
    fs::remove_all(pasta);
    fs::create_directories(pasta);
    const auto arquivo = pasta / "entrada.csv";
    const std::string cabecalho = "codigo;nome;quantidade;minimo;estoque_baixo\n";
    EstadoSistema estado;
    estado.estoque().cadastrarMaterial(Material(1, "Existente", 3, 2));
    auto salvar = [&](const std::string& dados) {
        std::ofstream saida(arquivo, std::ios::trunc);
        saida << cabecalho << dados;
    };
    auto verificarFalha = [&](const std::string& dados) {
        salvar(dados);
        CHECK_THROWS_AS(ImportadorCsv::importarMateriais(estado, arquivo), std::exception);
        CHECK(estado.estoque().listarMateriais().size() == 1);
    };
    verificarFalha(";Teste;5;1;nao\n"); // codigo vazio
    verificarFalha("2x;Teste;5;1;nao\n"); // texto apos o numero
    verificarFalha("2147483648;Teste;5;1;nao\n"); // int acima do limite
    verificarFalha("-2147483649;Teste;5;1;nao\n"); // int abaixo do limite
    verificarFalha("2;Teste;;1;nao\n"); // quantidade vazia
    verificarFalha("2;Teste;5;;nao\n"); // minimo vazio
    verificarFalha("2;Teste;5;1;sim\n"); // flag inconsistente
    verificarFalha("2;Teste;1;5;nao\n"); // flag inconsistente
    verificarFalha("1;Duplicado;5;1;nao\n"); // codigo ja cadastrado
    verificarFalha("2;Primeiro;5;1;nao\n3;Segundo;5;1;talvez\n"); // importacao atomica
    CHECK(estado.estoque().buscarMaterial(2) == nullptr);
    salvar("\n2;\"Porca; especial\";1;2;sim\n\n3;\"Parafuso \"\"A\"\"\";5;2;nao\n");
    CHECK(ImportadorCsv::importarMateriais(estado, arquivo) == 2);
    CHECK(estado.estoque().buscarMaterial(2)->getNome() == "Porca; especial");
    CHECK(estado.estoque().buscarMaterial(3)->getNome() == "Parafuso \"A\"");
    CHECK(estado.estoque().listarMateriais().size() == 3);
    CHECK_THROWS_AS(ImportadorCsv::separarCampos("1;\"ab;cd"), std::invalid_argument);
    CHECK_THROWS_AS(ImportadorCsv::separarCampos("1;\"a\"x;2"), std::invalid_argument);
    fs::remove_all(pasta);
}

TEST_CASE("Permissoes e buscas de usuarios usam diferentes perfis") {
    GestorUsuarios usuarios;
    CHECK_FALSE(usuarios.existe(100));
    CHECK(usuarios.buscar(100) == nullptr);
    CHECK_THROWS_AS(usuarios.exigirCadastrado(100), std::invalid_argument);
    CHECK_THROWS_AS(usuarios.exigirPermissaoEstoque(100), std::invalid_argument);
    CHECK_THROWS_AS(usuarios.exigirPermissaoRelatorio(100), std::invalid_argument);
    usuarios.cadastrar(1, "Ana", TipoUsuario::ALMOXARIFE);
    usuarios.cadastrar(2, "Bruno", TipoUsuario::FUNCIONARIO);
    usuarios.cadastrar(3, "Carla", TipoUsuario::GERENTE);
    CHECK_NOTHROW(usuarios.exigirPermissaoEstoque(1));
    CHECK_NOTHROW(usuarios.exigirPermissaoEstoque(3));
    CHECK_THROWS_AS(usuarios.exigirPermissaoEstoque(2), std::logic_error);
    CHECK_NOTHROW(usuarios.exigirPermissaoRelatorio(1));
    CHECK_NOTHROW(usuarios.exigirPermissaoRelatorio(3));
    CHECK_THROWS_AS(usuarios.exigirPermissaoRelatorio(2), std::logic_error);
    CHECK(usuarios.quantidadePorPerfil(TipoUsuario::GERENTE) == 1);
    CHECK(usuarios.buscarPorNome("an").size() == 1);
    CHECK(usuarios.buscarPorNome("BRU").size() == 1);
    CHECK(usuarios.buscarPorNome("inexistente").empty());
    CHECK_THROWS_AS(usuarios.alterarNome(99, "Nao existe"), std::invalid_argument);
    CHECK_THROWS_AS(usuarios.alterarPerfil(99, TipoUsuario::GERENTE), std::invalid_argument);
    CHECK_THROWS_AS(usuarios.excluir(99), std::invalid_argument);
    CHECK_THROWS_AS(usuarios.cadastrar(1, "Duplicada", TipoUsuario::FUNCIONARIO), std::invalid_argument);
    usuarios.alterarNome(2, "Bia");
    usuarios.alterarPerfil(2, TipoUsuario::GERENTE);
    CHECK(usuarios.buscar(2)->getNome() == "Bia");
    CHECK(usuarios.quantidadePorPerfil(TipoUsuario::GERENTE) == 2);
    usuarios.excluir(3);
    CHECK_FALSE(usuarios.existe(3));
}

TEST_CASE("Backups filtram nomes invalidos e restauro nao aceita conteudo danificado") {
    const fs::path pasta = fs::temp_directory_path() / "almox_teste_branches_60_backup";
    fs::remove_all(pasta);
    fs::create_directories(pasta);
    const fs::path origem = pasta / "estado.snapshot";
    const fs::path backups = pasta / "backups";
    EstadoSistema estado;
    estado.usuarios().cadastrar(1, "Admin", TipoUsuario::ALMOXARIFE);
    Persistencia::salvar(estado, origem);
    fs::path copia = Backup::criar(origem, backups);
    CHECK(fs::exists(copia));
    { std::ofstream f(backups / "backup-errado.txt"); f << "ruim"; }
    { std::ofstream f(backups / "qualquer.snapshot"); f << "ruim"; }
    { std::ofstream f(backups / "backup-corrompido.snapshot"); f << "invalido"; }
    CHECK(Backup::listar(backups).size() == 2);
    CHECK_THROWS_AS(Backup::restaurar(backups / "backup-corrompido.snapshot", origem), std::runtime_error);
    CHECK(Persistencia::carregar(origem).usuarios().existe(1));
    CHECK_THROWS_AS(Backup::restaurar(backups / "qualquer.snapshot", origem), std::invalid_argument);
    Backup::restaurar(copia, origem);
    CHECK(Persistencia::carregar(origem).usuarios().existe(1));
    fs::remove_all(pasta);
}

TEST_CASE("Relatorios com estado vazio e contendo registros alternam saidas") {
    RelatoriosAvancados relatorios;
    EstadoSistema estado;
    CHECK(relatorios.materiaisBaixos(estado).find("Nenhum material") != std::string::npos);
    CHECK(relatorios.materiaisPorNome(estado, "x").find("Nenhum material") != std::string::npos);
    CHECK(relatorios.ferramentasPorNome(estado, "x").find("Nenhuma ferramenta") != std::string::npos);
    CHECK(relatorios.emprestimosAtivos(estado).find("Nenhum emprestimo") != std::string::npos);
    CHECK(relatorios.historicoEmprestimos(estado).find("Nenhum emprestimo") != std::string::npos);
    CHECK(relatorios.movimentacoesNoPeriodo(estado, "2026-11-01", "2026-11-30").find("Nenhuma movimentacao") != std::string::npos);
    CHECK(relatorios.rankingEmTexto(estado).find("Ainda nao") != std::string::npos);
    CHECK(Auditoria::avisos(estado).size() >= 2);
    estado.usuarios().cadastrar(1, "Ana", TipoUsuario::ALMOXARIFE);
    estado.usuarios().cadastrar(2, "Bruno", TipoUsuario::FUNCIONARIO);
    estado.estoque().cadastrarMaterial(Material(10, "Parafuso", 1, 3));
    estado.estoque().cadastrarMaterial(Material(11, "Porca", 20, 3));
    estado.estoque().cadastrarFerramenta(Ferramenta(20, "Furadeira"));
    CHECK(relatorios.materiaisBaixos(estado).find("Parafuso") != std::string::npos);
    CHECK(relatorios.materiaisBaixos(estado).find("Porca") == std::string::npos);
    CHECK(relatorios.materiaisPorNome(estado, "PARA").find("Parafuso") != std::string::npos);
    CHECK(relatorios.materiaisPorNome(estado, "Naoexiste").find("Nenhum") != std::string::npos);
    CHECK(relatorios.ferramentasPorNome(estado, "FURA").find("Furadeira") != std::string::npos);
    CHECK(relatorios.ferramentasPorNome(estado, "nada").find("Nenhuma") != std::string::npos);
    CHECK(relatorios.estatisticas(estado).ferramentasDisponiveis == 1);
    CHECK(relatorios.maisMovimentados(estado, 5).empty());
    estado.historico().registrarEntrada(estado.estoque(), estado.usuarios(), 10, 3, 1, "2026-11-25", "Reposicao");
    estado.historico().registrarSaida(estado.estoque(), estado.usuarios(), 11, 4, 1, "2026-11-26", "Consumo");
    CHECK(estado.historico().totalEntradas(10) == 3);
    CHECK(estado.historico().totalSaidas(11) == 4);
    CHECK(estado.historico().porMaterial(11).size() == 1);
    CHECK(estado.historico().porUsuario(1).size() == 2);
    CHECK(estado.historico().porPeriodo("2026-11-26", "2026-11-26").size() == 1);
    CHECK(relatorios.maisMovimentados(estado, 1).size() == 1);
    CHECK(relatorios.movimentacoesNoPeriodo(estado, "2026-11-25", "2026-11-26").find("SAIDA") != std::string::npos);
    CHECK_THROWS_AS(estado.historico().porPeriodo("2026-11-26", "2026-11-25"), std::invalid_argument);
    CHECK(Auditoria::semInconsistencias(estado));
}

TEST_CASE("Emprestimo e devolucao validam usuario ferramenta e datas") {
    EstadoSistema estado;
    estado.usuarios().cadastrar(1, "Operador", TipoUsuario::FUNCIONARIO);
    estado.estoque().cadastrarFerramenta(Ferramenta(20, "Alicate"));
    CHECK_THROWS_AS(estado.emprestimos().emprestar(estado.estoque(), estado.usuarios(), 99, 1, "2026-11-26"), std::invalid_argument);
    CHECK_THROWS_AS(estado.emprestimos().emprestar(estado.estoque(), estado.usuarios(), 20, 99, "2026-11-26"), std::invalid_argument);
    CHECK_THROWS_AS(estado.emprestimos().devolver(estado.estoque(), 99, "2026-11-26"), std::invalid_argument);
    CHECK_THROWS_AS(estado.emprestimos().devolver(estado.estoque(), 20, "2026-11-26"), std::logic_error);
    CHECK_THROWS_AS(estado.emprestimos().emprestar(estado.estoque(), estado.usuarios(), 20, 1, "2026-02-30"), std::invalid_argument);
    CHECK(estado.emprestimos().quantidadeAbertos() == 0);
    estado.emprestimos().emprestar(estado.estoque(), estado.usuarios(), 20, 1, "2026-11-25");
    CHECK(estado.emprestimos().quantidadeAbertos() == 1);
    CHECK_THROWS_AS(estado.emprestimos().emprestar(estado.estoque(), estado.usuarios(), 20, 1, "2026-11-26"), std::logic_error);
    CHECK_THROWS_AS(estado.emprestimos().devolver(estado.estoque(), 20, "2026-11-24"), std::invalid_argument);
    CHECK(estado.emprestimos().quantidadeAbertos() == 1);
    estado.emprestimos().devolver(estado.estoque(), 20, "2026-11-26");
    CHECK(estado.emprestimos().quantidadeAbertos() == 0);
    CHECK(estado.emprestimos().ativos().empty());
    CHECK(estado.emprestimos().porUsuario(1).size() == 1);
    CHECK(estado.estoque().buscarFerramenta(20)->estaDisponivel());
}

TEST_CASE("Persistencia detecta registros truncados em diferentes secoes") {
    const fs::path pasta = fs::temp_directory_path() / "almox_teste_branches_60_truncados";
    fs::remove_all(pasta);
    fs::create_directories(pasta);
    const fs::path arquivo = pasta / "dados.snapshot";
    auto esperarErro = [&](const std::string& conteudo) {
        { std::ofstream saida(arquivo, std::ios::trunc); saida << conteudo; }
        CHECK_THROWS_AS(Persistencia::carregar(arquivo), std::exception);
    };
    esperarErro("ALMOX1\nUSUARIOS 1\n");
    esperarErro("ALMOX1\nUSUARIOS 1\n1 99 \"Perfil\"\nMATERIAIS 0\nFERRAMENTAS 0\nEMPRESTIMOS 0\nMOVIMENTACOES 0\nFIM\n");
    esperarErro("ALMOX1\nUSUARIOS 0\nMATERIAIS 1\n");
    esperarErro("ALMOX1\nUSUARIOS 0\nMATERIAIS 0\nFERRAMENTAS 1\n");
    esperarErro("ALMOX1\nUSUARIOS 0\nMATERIAIS 0\nFERRAMENTAS 0\nEMPRESTIMOS 1\n");
    esperarErro("ALMOX1\nUSUARIOS 0\nMATERIAIS 0\nFERRAMENTAS 0\nEMPRESTIMOS 0\nMOVIMENTACOES 1\n");
    esperarErro("ALMOX1\nUSUARIOS 0\nMATERIAIS 0\nFERRAMENTAS 0\nEMPRESTIMOS 0\nMOVIMENTACOES 0\nFIM\nEXTRA\n");
    CHECK_FALSE(Persistencia::existeArquivo(pasta / "nao_existe.snapshot"));
    CHECK_THROWS_AS(Persistencia::carregar(pasta / "nao_existe.snapshot"), std::runtime_error);
    fs::remove_all(pasta);
}

TEST_CASE("CSV de ferramentas importa dados validos de modo atomico") {
    const fs::path pasta = fs::temp_directory_path() / "almox_teste_branches_60_ferramentas";
    fs::remove_all(pasta);
    fs::create_directories(pasta);
    const fs::path arquivo = pasta / "ferramentas.csv";
    EstadoSistema estado;
    estado.estoque().cadastrarFerramenta(Ferramenta(1, "Existente"));
    const std::string header = "codigo;nome;situacao\n";
    auto salvar = [&](const std::string& corpo) {
        std::ofstream saida(arquivo, std::ios::trunc);
        saida << header << corpo;
    };
    auto erro = [&](const std::string& corpo) {
        salvar(corpo);
        CHECK_THROWS_AS(ImportadorCsv::importarFerramentas(estado, arquivo), std::exception);
        CHECK(estado.estoque().listarFerramentas().size() == 1);
    };
    erro(";Chave;disponivel\n");
    erro("2;;disponivel\n");
    erro("2;@atalho;disponivel\n");
    erro("2;Chave;emprestada\n");
    erro("2;Chave;disponivel\n2;Outra;disponivel\n");
    erro("2;Chave;disponivel\n3;Martelo;quebrada\n");
    salvar("\n2;\"Chave; de fenda\";disponivel\n\n3;Martelo;disponivel\n");
    CHECK(ImportadorCsv::importarFerramentas(estado, arquivo) == 2);
    CHECK(estado.estoque().buscarFerramenta(2)->getNome() == "Chave; de fenda");
    CHECK(estado.estoque().buscarFerramenta(3)->estaDisponivel());
    fs::remove_all(pasta);
}

TEST_CASE("Menu recusa operacoes sem permissao e continua depois de erros") {
    const fs::path pasta = fs::temp_directory_path() / "almox_teste_branches_60_menu";
    fs::remove_all(pasta);
    fs::create_directories(pasta);
    const fs::path dados = pasta / "dados.snapshot";
    EstadoSistema estado;
    estado.usuarios().cadastrar(1, "Administrador", TipoUsuario::ALMOXARIFE);
    estado.usuarios().cadastrar(2, "Funcionario", TipoUsuario::FUNCIONARIO);
    estado.usuarios().cadastrar(3, "Gerente", TipoUsuario::GERENTE);
    Persistencia::salvar(estado, dados);
    std::istringstream entrada(
        "12\n" // mostra os tres perfis
        "1\n2\n" // funcionario sem permissao para cadastrar material
        "2\n2\n" // funcionario sem permissao para cadastrar ferramenta
        "14\n2\n" // funcionario sem permissao para relatorio
        "13\n2\n" // funcionario sem permissao para alterar perfil
        "15\n1\nnao-existe\n"
        "15\n2\nnao-existe\n"
        "17\n18\n24\n28\n0\n");
    std::ostringstream saida, erros;
    InterfaceTerminal terminal(dados, entrada, saida, erros);
    terminal.executar();
    CHECK(erros.str().find("Este perfil nao pode modificar o estoque") != std::string::npos);
    CHECK(erros.str().find("Este perfil nao pode emitir relatorios") != std::string::npos);
    CHECK(saida.str().find("Nenhum material encontrado") != std::string::npos);
    CHECK(saida.str().find("Nenhuma ferramenta encontrada") != std::string::npos);
    CHECK(saida.str().find("AUDITORIA DO ALMOXARIFADO") != std::string::npos);
    CHECK(terminal.estado().totalItens() == 0);
    CHECK(terminal.estado().usuarios().quantidade() == 3);
    fs::remove_all(pasta);
}

TEST_CASE("Movimentacoes recusam IDs quantidades tipos datas e observacoes invalidas") {
    CHECK_THROWS_AS(Movimentacao(0, 10, 1, 5, TipoMovimentacao::ENTRADA, "2026-11-26", ""), std::invalid_argument);
    CHECK_THROWS_AS(Movimentacao(1, 0, 1, 5, TipoMovimentacao::ENTRADA, "2026-11-26", ""), std::invalid_argument);
    CHECK_THROWS_AS(Movimentacao(1, 10, 0, 5, TipoMovimentacao::ENTRADA, "2026-11-26", ""), std::invalid_argument);
    CHECK_THROWS_AS(Movimentacao(1, 10, 1, 0, TipoMovimentacao::ENTRADA, "2026-11-26", ""), std::invalid_argument);
    CHECK_THROWS_AS(Movimentacao(1, 10, 1, 5, static_cast<TipoMovimentacao>(9), "2026-11-26", ""), std::invalid_argument);
    CHECK_THROWS_AS(Movimentacao(1, 10, 1, 5, TipoMovimentacao::ENTRADA, "2026-02-30", ""), std::invalid_argument);
    CHECK_THROWS_AS(Movimentacao(1, 10, 1, 5, TipoMovimentacao::SAIDA, "2026-11-26", std::string(301, 'x')), std::invalid_argument);
    Movimentacao entrada(1, 10, 1, 5, TipoMovimentacao::ENTRADA, "2026-11-26", "");
    Movimentacao saida(2, 10, 1, 2, TipoMovimentacao::SAIDA, "2026-11-26", "consumo");
    CHECK(entrada.resumo().find("ENTRADA") != std::string::npos);
    CHECK(saida.resumo().find("SAIDA") != std::string::npos);
    CHECK(entrada.resumo().find("(consumo)") == std::string::npos);
    CHECK(saida.resumo().find("(consumo)") != std::string::npos);
}

TEST_CASE("Persistencia valida cadastros e relacionamentos ao carregar registros") {
    const fs::path pasta = fs::temp_directory_path() / "almox_teste_branches_60_relacionamentos";
    fs::remove_all(pasta);
    fs::create_directories(pasta);
    const fs::path arquivo = pasta / "estado.snapshot";
    const std::string usuario = "ALMOX1\nUSUARIOS 1\n1 0 \"Admin\"\n";
    auto conferir = [&](const std::string& corpo) {
        { std::ofstream arq(arquivo); arq << usuario << corpo; }
        CHECK_THROWS_AS(Persistencia::carregar(arquivo), std::exception);
    };
    conferir("MATERIAIS 1\n2 -1 1 \"Parafuso\"\nFERRAMENTAS 0\nEMPRESTIMOS 0\nMOVIMENTACOES 0\nFIM\n");
    conferir("MATERIAIS 0\nFERRAMENTAS 1\n0 \"Martelo\"\nEMPRESTIMOS 0\nMOVIMENTACOES 0\nFIM\n");
    conferir("MATERIAIS 0\nFERRAMENTAS 0\nEMPRESTIMOS 1\n1 99 \"2026-11-25\" \"\"\nMOVIMENTACOES 0\nFIM\n");
    conferir("MATERIAIS 0\nFERRAMENTAS 1\n20 \"Martelo\"\nEMPRESTIMOS 1\n77 20 \"2026-11-25\" \"\"\nMOVIMENTACOES 0\nFIM\n");
    conferir("MATERIAIS 0\nFERRAMENTAS 1\n20 \"Martelo\"\nEMPRESTIMOS 1\n1 20 \"2026-02-30\" \"\"\nMOVIMENTACOES 0\nFIM\n");
    conferir("MATERIAIS 0\nFERRAMENTAS 0\nEMPRESTIMOS 0\nMOVIMENTACOES 1\n1 22 1 5 1 \"2026-11-26\" \"Entrada\"\nFIM\n");
    conferir("MATERIAIS 1\n2 5 1 \"Porca\"\nFERRAMENTAS 0\nEMPRESTIMOS 0\nMOVIMENTACOES 1\n1 2 77 5 1 \"2026-11-26\" \"Entrada\"\nFIM\n");
    { std::ofstream arq(arquivo); arq << usuario
        << "MATERIAIS 1\n2 5 1 \"Porca\"\nFERRAMENTAS 1\n20 \"Martelo\"\n"
        << "EMPRESTIMOS 1\n1 20 \"2026-11-25\" \"2026-11-26\"\n"
        << "MOVIMENTACOES 1\n1 2 1 5 1 \"2026-11-26\" \"Entrada\"\nFIM\n"; }
    EstadoSistema restaurado = Persistencia::carregar(arquivo);
    CHECK(restaurado.estoque().listarMateriais().size() == 1);
    CHECK(restaurado.estoque().buscarFerramenta(20)->estaDisponivel());
    CHECK(restaurado.historico().quantidade() == 1);
    CHECK(restaurado.emprestimos().quantidade() == 1);
    CHECK_NOTHROW(restaurado.validarIntegridade());
    fs::remove_all(pasta);
}
