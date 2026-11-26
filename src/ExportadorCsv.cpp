#include "ExportadorCsv.hpp"
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

namespace {
std::string campo(std::string valor) {
    if (!valor.empty() &&
        (valor[0] == '=' || valor[0] == '+' || valor[0] == '-' || valor[0] == '@')) {
        valor.insert(valor.begin(), '\'');
    }
    std::string resultado = "\"";
    for (char caractere : valor) {
        if (caractere == '"') resultado += "\"\"";
        else resultado.push_back(caractere);
    }
    resultado += '"';
    return resultado;
}

std::ofstream criarArquivo(const fs::path& arquivo) {
    if (arquivo.extension() != ".csv") {
        throw std::invalid_argument("O relatorio precisa ter extensao .csv");
    }
    if (!arquivo.parent_path().empty()) fs::create_directories(arquivo.parent_path());
    std::ofstream saida(arquivo, std::ios::trunc);
    if (!saida) throw std::runtime_error("Nao foi possivel criar arquivo CSV");
    return saida;
}

void conferirSaida(const std::ofstream& arquivo) {
    if (!arquivo) throw std::runtime_error("Falha ao gravar arquivo CSV");
}

const char* perfil(TipoUsuario p) {
    if (p == TipoUsuario::ALMOXARIFE) return "almoxarife";
    if (p == TipoUsuario::GERENTE) return "gerente";
    return "funcionario";
}
}

void ExportadorCsv::materiais(const EstadoSistema& estado, const fs::path& arquivo) {
    std::ofstream saida = criarArquivo(arquivo);
    saida << "codigo;nome;quantidade;minimo;estoque_baixo\n";
    for (const auto& m : estado.estoque().listarMateriais()) {
        saida << m.getCodigo() << ';' << campo(m.getNome()) << ';'
              << m.getQuantidade() << ';' << m.getEstoqueMinimo() << ';'
              << (m.estoqueBaixo() ? "sim" : "nao") << '\n';
    }
    conferirSaida(saida);
}

void ExportadorCsv::ferramentas(const EstadoSistema& estado, const fs::path& arquivo) {
    std::ofstream saida = criarArquivo(arquivo);
    saida << "codigo;nome;situacao\n";
    for (const auto& f : estado.estoque().listarFerramentas()) {
        saida << f.getCodigo() << ';' << campo(f.getNome()) << ';'
              << (f.estaDisponivel() ? "disponivel" : "emprestada") << '\n';
    }
    conferirSaida(saida);
}

void ExportadorCsv::usuarios(const EstadoSistema& estado, const fs::path& arquivo) {
    std::ofstream saida = criarArquivo(arquivo);
    saida << "id;nome;perfil\n";
    for (const auto& u : estado.usuarios().listar()) {
        saida << u.getId() << ';' << campo(u.getNome()) << ';'
              << campo(perfil(u.getTipo())) << '\n';
    }
    conferirSaida(saida);
}

void ExportadorCsv::emprestimos(const EstadoSistema& estado, const fs::path& arquivo) {
    std::ofstream saida = criarArquivo(arquivo);
    saida << "usuario;ferramenta;retirada;devolucao;situacao\n";
    for (const auto& e : estado.emprestimos().listar()) {
        saida << e.getUsuarioId() << ';' << e.getFerramentaCodigo() << ';'
              << campo(e.getDataRetirada()) << ';' << campo(e.getDataDevolucao()) << ';'
              << (e.estaAberto() ? "aberto" : "encerrado") << '\n';
    }
    conferirSaida(saida);
}

void ExportadorCsv::movimentacoes(const EstadoSistema& estado, const fs::path& arquivo) {
    std::ofstream saida = criarArquivo(arquivo);
    saida << "id;data;tipo;material;quantidade;responsavel;observacao\n";
    for (const auto& m : estado.historico().listar()) {
        saida << m.getId() << ';' << campo(m.getData()) << ';'
              << (m.getTipo() == TipoMovimentacao::ENTRADA ? "entrada" : "saida") << ';'
              << m.getCodigoMaterial() << ';' << m.getQuantidade() << ';'
              << m.getIdResponsavel() << ';' << campo(m.getObservacao()) << '\n';
    }
    conferirSaida(saida);
}
