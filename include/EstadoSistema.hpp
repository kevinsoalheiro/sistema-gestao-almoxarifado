#pragma once
#include "Estoque.hpp"
#include "GestorUsuarios.hpp"
#include "GerenciadorEmprestimos.hpp"
#include "HistoricoMovimentacoes.hpp"

// Raiz dos dados que podem ser salvos e restaurados juntos.
class EstadoSistema {
private:
    Estoque estoque_;
    GestorUsuarios usuarios_;
    GerenciadorEmprestimos emprestimos_;
    HistoricoMovimentacoes historico_;

public:
    Estoque& estoque();
    const Estoque& estoque() const;
    GestorUsuarios& usuarios();
    const GestorUsuarios& usuarios() const;
    GerenciadorEmprestimos& emprestimos();
    const GerenciadorEmprestimos& emprestimos() const;
    HistoricoMovimentacoes& historico();
    const HistoricoMovimentacoes& historico() const;

    std::size_t totalItens() const;
    bool vazio() const;
    void validarIntegridade() const;
};
