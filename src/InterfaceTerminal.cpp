#include "InterfaceTerminal.hpp"
#include "Backup.hpp"
#include "Auditoria.hpp"
#include "Data.hpp"
#include "ExportadorCsv.hpp"
#include "ImportadorCsv.hpp"
#include "Persistencia.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

InterfaceTerminal::InterfaceTerminal(const fs::path& arquivoDados,
                                     std::istream& entrada, std::ostream& saida,
                                     std::ostream& erros)
    : arquivoDados_(arquivoDados),
      pastaBackups_(arquivoDados.parent_path() / "backups"),
      entrada_(entrada), saida_(saida), erros_(erros) {
    if (Persistencia::existeArquivo(arquivoDados_)) {
        estado_ = Persistencia::carregar(arquivoDados_);
    } else {
        // Perfil inicial de demonstracao, sem autenticacao de senha.
        estado_.usuarios().cadastrar(1, "Administrador", TipoUsuario::ALMOXARIFE);
        Persistencia::salvar(estado_, arquivoDados_);
    }
}

const EstadoSistema& InterfaceTerminal::estado() const { return estado_; }

std::string InterfaceTerminal::lerTexto(const std::string& pergunta) {
    saida_ << pergunta;
    std::string resposta;
    if (!std::getline(entrada_, resposta)) throw std::runtime_error("Entrada encerrada");
    return resposta;
}

int InterfaceTerminal::lerInteiro(const std::string& pergunta) {
    const std::string texto = lerTexto(pergunta);
    std::istringstream conversor(texto);
    int numero = 0;
    std::string sobra;
    if (!(conversor >> numero) || (conversor >> sobra)) {
        throw std::invalid_argument("Digite um numero inteiro valido");
    }
    return numero;
}

TipoUsuario InterfaceTerminal::lerPerfil() {
    const int opcao = lerInteiro("Perfil (0 almoxarife, 1 funcionario, 2 gerente): ");
    if (opcao < 0 || opcao > 2) throw std::invalid_argument("Perfil inexistente");
    return static_cast<TipoUsuario>(opcao);
}

void InterfaceTerminal::imprimir(const std::string& texto) const {
    saida_ << texto;
    if (texto.empty() || texto.back() != '\n') saida_ << '\n';
}

void InterfaceTerminal::menu() const {
    saida_ << "\n===== ALMOXARIFADO =====\n"
           << " 1  Cadastrar material\n"
           << " 2  Cadastrar ferramenta\n"
           << " 3  Listar materiais\n"
           << " 4  Listar ferramentas\n"
           << " 5  Entrada de material\n"
           << " 6  Saida de material\n"
           << " 7  Emprestar ferramenta\n"
           << " 8  Devolver ferramenta\n"
           << " 9  Relatorio de estoque baixo\n"
           << "10  Relatorio de ferramentas emprestadas\n"
           << "11  Cadastrar usuario\n"
           << "12  Listar usuarios\n"
           << "13  Alterar perfil de usuario\n"
           << "14  Resumo geral\n"
           << "15  Pesquisar item por nome\n"
           << "16  Historico de entradas e saidas\n"
           << "17  Emprestimos ativos\n"
           << "18  Ranking de materiais movimentados\n"
           << "19  Salvar dados manualmente\n"
           << "20  Criar backup\n"
           << "21  Restaurar backup\n"
           << "22  Listar backups\n"
           << "23  Relatorio de usuario\n"
           << "24  Historico de emprestimos\n"
           << "25  Movimentacoes por periodo\n"
           << "26  Exportar dados em CSV\n"
           << "27  Importar materiais e ferramentas do CSV\n"
           << "28  Auditoria e alertas do sistema\n"
           << " 0  Sair\n";
}

void InterfaceTerminal::mostrarMateriais() const {
    if (estado_.estoque().listarMateriais().empty()) {
        imprimir("Nenhum material cadastrado.");
        return;
    }
    for (const auto& m : estado_.estoque().listarMateriais()) {
        saida_ << m.getCodigo() << " - " << m.getNome()
               << ": " << m.getQuantidade()
               << " (minimo: " << m.getEstoqueMinimo() << ")\n";
    }
}

void InterfaceTerminal::mostrarFerramentas() const {
    if (estado_.estoque().listarFerramentas().empty()) {
        imprimir("Nenhuma ferramenta cadastrada.");
        return;
    }
    for (const auto& f : estado_.estoque().listarFerramentas()) {
        saida_ << f.getCodigo() << " - " << f.getNome()
               << (f.estaDisponivel() ? " (disponivel)" : " (emprestada)") << '\n';
    }
}

void InterfaceTerminal::mostrarUsuarios() const {
    if (estado_.usuarios().listar().empty()) {
        imprimir("Nenhum usuario cadastrado.");
        return;
    }
    for (const auto& u : estado_.usuarios().listar()) {
        const char* tipo = "funcionario";
        if (u.getTipo() == TipoUsuario::ALMOXARIFE) tipo = "almoxarife";
        if (u.getTipo() == TipoUsuario::GERENTE) tipo = "gerente";
        saida_ << u.getId() << " - " << u.getNome() << " (" << tipo << ")\n";
    }
}

void InterfaceTerminal::mostrarHistorico() const {
    if (estado_.historico().listar().empty()) {
        imprimir("Nenhuma movimentacao registrada.");
        return;
    }
    for (const auto& m : estado_.historico().listar()) imprimir(m.resumo());
}

void InterfaceTerminal::cadastrarMaterial() {
    const int id = lerInteiro("ID do responsavel: ");
    estado_.usuarios().exigirPermissaoEstoque(id);
    const int codigo = lerInteiro("Codigo do material: ");
    const std::string nome = lerTexto("Nome: ");
    const int qtd = lerInteiro("Quantidade inicial: ");
    const int minimo = lerInteiro("Estoque minimo: ");
    estado_.estoque().cadastrarMaterial(Material(codigo, nome, qtd, minimo));
    imprimir("Material cadastrado.");
}

void InterfaceTerminal::cadastrarFerramenta() {
    const int id = lerInteiro("ID do responsavel: ");
    estado_.usuarios().exigirPermissaoEstoque(id);
    const int codigo = lerInteiro("Codigo da ferramenta: ");
    const std::string nome = lerTexto("Nome: ");
    estado_.estoque().cadastrarFerramenta(Ferramenta(codigo, nome));
    imprimir("Ferramenta cadastrada.");
}

void InterfaceTerminal::movimentar(bool entrada) {
    const int responsavel = lerInteiro("ID do responsavel: ");
    const int codigo = lerInteiro("Codigo do material: ");
    const int quantidade = lerInteiro("Quantidade: ");
    const std::string data = lerTexto("Data (AAAA-MM-DD): ");
    const std::string motivo = lerTexto("Observacao (opcional): ");
    if (entrada) {
        estado_.historico().registrarEntrada(estado_.estoque(), estado_.usuarios(),
                                             codigo, quantidade, responsavel, data, motivo);
    } else {
        estado_.historico().registrarSaida(estado_.estoque(), estado_.usuarios(),
                                           codigo, quantidade, responsavel, data, motivo);
    }
    imprimir("Movimentacao registrada no historico.");
}

void InterfaceTerminal::emprestar() {
    const int codigo = lerInteiro("Codigo da ferramenta: ");
    const int usuario = lerInteiro("ID do usuario: ");
    const std::string data = lerTexto("Data da retirada (AAAA-MM-DD): ");
    estado_.emprestimos().emprestar(estado_.estoque(), estado_.usuarios(),
                                   codigo, usuario, data);
    imprimir("Emprestimo registrado.");
}

void InterfaceTerminal::devolver() {
    const int codigo = lerInteiro("Codigo da ferramenta: ");
    const std::string data = lerTexto("Data da devolucao (AAAA-MM-DD): ");
    estado_.emprestimos().devolver(estado_.estoque(), codigo, data);
    imprimir("Devolucao registrada.");
}

void InterfaceTerminal::cadastrarUsuario() {
    const int id = lerInteiro("ID do novo usuario: ");
    const std::string nome = lerTexto("Nome do usuario: ");
    const TipoUsuario tipo = lerPerfil();
    estado_.usuarios().cadastrar(id, nome, tipo);
    imprimir("Usuario cadastrado.");
}

void InterfaceTerminal::alterarPerfil() {
    const int idResponsavel = lerInteiro("ID de quem autoriza a mudanca: ");
    estado_.usuarios().exigirPermissaoEstoque(idResponsavel);
    const int id = lerInteiro("ID do usuario a alterar: ");
    const TipoUsuario tipo = lerPerfil();
    estado_.usuarios().alterarPerfil(id, tipo);
    imprimir("Perfil atualizado.");
}

void InterfaceTerminal::pesquisar() {
    const int categoria = lerInteiro("Categoria (1 material, 2 ferramenta): ");
    const std::string termo = lerTexto("Trecho do nome: ");
    if (categoria == 1) imprimir(relatorios_.materiaisPorNome(estado_, termo));
    else if (categoria == 2) imprimir(relatorios_.ferramentasPorNome(estado_, termo));
    else throw std::invalid_argument("Categoria invalida");
}

void InterfaceTerminal::relatorioPeriodo() {
    const std::string inicio = lerTexto("Inicio (AAAA-MM-DD): ");
    const std::string fim = lerTexto("Fim (AAAA-MM-DD): ");
    imprimir(relatorios_.movimentacoesNoPeriodo(estado_, inicio, fim));
}

void InterfaceTerminal::relatorioUsuario() {
    const int id = lerInteiro("ID do usuario: ");
    imprimir(relatorios_.resumoPorUsuario(estado_, id));
}

void InterfaceTerminal::listarBackups() const {
    const auto arquivos = Backup::listar(pastaBackups_);
    if (arquivos.empty()) {
        imprimir("Nenhum backup encontrado.");
        return;
    }
    for (const auto& arquivo : arquivos) imprimir(arquivo.filename().string());
}

void InterfaceTerminal::restaurarBackup() {
    const auto arquivos = Backup::listar(pastaBackups_);
    if (arquivos.empty()) throw std::runtime_error("Nao existem backups");
    for (std::size_t i = 0; i < arquivos.size(); ++i) {
        saida_ << i + 1 << " - " << arquivos[i].filename().string() << '\n';
    }
    const int numero = lerInteiro("Numero do backup: ");
    if (numero < 1 || static_cast<std::size_t>(numero) > arquivos.size()) {
        throw std::invalid_argument("Backup inexistente");
    }
    if (lerTexto("Confirmar restauracao? (SIM para confirmar): ") != "SIM") {
        imprimir("Restauracao cancelada.");
        return;
    }
    // Um backup do estado atual permite desfazer a restauracao manualmente.
    (void)Backup::criar(arquivoDados_, pastaBackups_);
    Backup::restaurar(arquivos[static_cast<std::size_t>(numero - 1)], arquivoDados_);
    estado_ = Persistencia::carregar(arquivoDados_);
    imprimir("Backup restaurado.");
}

void InterfaceTerminal::exportarCsv() {
    const int opcao = lerInteiro("Exportar (1 materiais, 2 ferramentas, 3 usuarios, 4 emprestimos, 5 movimentacoes): ");
    const fs::path pasta = arquivoDados_.parent_path() / "relatorios";
    fs::path destino;
    switch (opcao) {
        case 1:
            destino = pasta / "materiais.csv";
            ExportadorCsv::materiais(estado_, destino);
            break;
        case 2:
            destino = pasta / "ferramentas.csv";
            ExportadorCsv::ferramentas(estado_, destino);
            break;
        case 3:
            destino = pasta / "usuarios.csv";
            ExportadorCsv::usuarios(estado_, destino);
            break;
        case 4:
            destino = pasta / "emprestimos.csv";
            ExportadorCsv::emprestimos(estado_, destino);
            break;
        case 5:
            destino = pasta / "movimentacoes.csv";
            ExportadorCsv::movimentacoes(estado_, destino);
            break;
        default: throw std::invalid_argument("Tipo de exportacao desconhecido");
    }
    imprimir("Arquivo exportado: " + destino.string());
}

void InterfaceTerminal::importarCsv() {
    const int id = lerInteiro("ID do responsavel: ");
    estado_.usuarios().exigirPermissaoEstoque(id);
    const int categoria = lerInteiro("Importar (1 materiais, 2 ferramentas): ");
    const std::string caminho = lerTexto("Caminho completo do arquivo CSV: ");
    std::size_t quantidade = 0;
    if (categoria == 1) {
        quantidade = ImportadorCsv::importarMateriais(estado_, caminho);
    } else if (categoria == 2) {
        quantidade = ImportadorCsv::importarFerramentas(estado_, caminho);
    } else {
        throw std::invalid_argument("Categoria invalida");
    }
    imprimir("Itens importados: " + std::to_string(quantidade));
}

bool InterfaceTerminal::executarOpcao(int opcao) {
    switch (opcao) {
        case 1: cadastrarMaterial(); return true;
        case 2: cadastrarFerramenta(); return true;
        case 3: mostrarMateriais(); return false;
        case 4: mostrarFerramentas(); return false;
        case 5: movimentar(true); return true;
        case 6: movimentar(false); return true;
        case 7: emprestar(); return true;
        case 8: devolver(); return true;
        case 9: imprimir(relatorios_.materiaisBaixos(estado_)); return false;
        case 10: imprimir(relatorios_.emprestimosAtivos(estado_)); return false;
        case 11: cadastrarUsuario(); return true;
        case 12: mostrarUsuarios(); return false;
        case 13: alterarPerfil(); return true;
        case 14: {
            int id = lerInteiro("ID para emitir relatorio: ");
            estado_.usuarios().exigirPermissaoRelatorio(id);
            imprimir(relatorios_.resumoGeral(estado_));
            return false;
        }
        case 15: pesquisar(); return false;
        case 16: mostrarHistorico(); return false;
        case 17: imprimir(relatorios_.emprestimosAtivos(estado_)); return false;
        case 18: imprimir(relatorios_.rankingEmTexto(estado_)); return false;
        case 19:
            Persistencia::salvar(estado_, arquivoDados_);
            imprimir("Dados salvos.");
            return false;
        case 20: {
            Persistencia::salvar(estado_, arquivoDados_);
            const auto backup = Backup::criar(arquivoDados_, pastaBackups_);
            imprimir("Backup criado: " + backup.filename().string());
            return false;
        }
        case 21: restaurarBackup(); return false;
        case 22: listarBackups(); return false;
        case 23: relatorioUsuario(); return false;
        case 24: imprimir(relatorios_.historicoEmprestimos(estado_)); return false;
        case 25: relatorioPeriodo(); return false;
        case 26: exportarCsv(); return false;
        case 27: importarCsv(); return true;
        case 28: imprimir(Auditoria::relatorio(estado_)); return false;
        default: imprimir("Opcao desconhecida."); return false;
    }
}

void InterfaceTerminal::executar() {
    imprimir("Sistema de Gestao de Almoxarifado e Ferramentas");
    imprimir("Perfil de exemplo inicial: Administrador (ID 1). Sem senha nesta versao.");
    while (true) {
        menu();
        try {
            const int opcao = lerInteiro("Opcao: ");
            if (opcao == 0) break;
            const bool alterado = executarOpcao(opcao);
            if (alterado) Persistencia::salvar(estado_, arquivoDados_);
        } catch (const std::exception& erro) {
            erros_ << "Erro: " << erro.what() << '\n';
            if (!entrada_) break;
        }
    }
    Persistencia::salvar(estado_, arquivoDados_);
    imprimir("Dados salvos. Programa encerrado.");
}
