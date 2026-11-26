#pragma once
#include "EstadoSistema.hpp"
#include "RelatoriosAvancados.hpp"
#include <filesystem>
#include <iosfwd>
#include <string>

// Apresenta as operacoes do sistema sem misturar regras de negocio com o menu.
class InterfaceTerminal {
private:
    EstadoSistema estado_;
    RelatoriosAvancados relatorios_;
    std::filesystem::path arquivoDados_;
    std::filesystem::path pastaBackups_;
    std::istream& entrada_;
    std::ostream& saida_;
    std::ostream& erros_;

    std::string lerTexto(const std::string& pergunta);
    int lerInteiro(const std::string& pergunta);
    TipoUsuario lerPerfil();
    void menu() const;
    void mostrarMateriais() const;
    void mostrarFerramentas() const;
    void mostrarUsuarios() const;
    void mostrarHistorico() const;
    void imprimir(const std::string& texto) const;
    bool executarOpcao(int opcao);
    void cadastrarMaterial();
    void cadastrarFerramenta();
    void movimentar(bool entrada);
    void emprestar();
    void devolver();
    void cadastrarUsuario();
    void alterarPerfil();
    void pesquisar();
    void relatorioPeriodo();
    void relatorioUsuario();
    void restaurarBackup();
    void listarBackups() const;
    void exportarCsv();
    void importarCsv();

public:
    InterfaceTerminal(const std::filesystem::path& arquivoDados,
                      std::istream& entrada, std::ostream& saida, std::ostream& erros);
    void executar();
    const EstadoSistema& estado() const;
};
