#pragma once
#include "Emprestimo.hpp"
#include "Estoque.hpp"
#include "GestorUsuarios.hpp"
#include <string>
#include <vector>

// Coordena os emprestimos e mantem o historico, inclusive apos devolucao.
class GerenciadorEmprestimos {
private:
    std::vector<Emprestimo> emprestimos_;

public:
    const std::vector<Emprestimo>& listar() const;
    std::size_t quantidade() const;
    std::size_t quantidadeAbertos() const;
    const Emprestimo* buscarAberto(int codigoFerramenta) const;
    std::vector<Emprestimo> porUsuario(int usuarioId) const;
    std::vector<Emprestimo> ativos() const;
    void emprestar(Estoque& estoque, const GestorUsuarios& usuarios,
                  int codigoFerramenta, int usuarioId, const std::string& data);
    void devolver(Estoque& estoque, int codigoFerramenta,
                 const std::string& data);
    void restaurar(Estoque& estoque, const GestorUsuarios& usuarios,
                   const Emprestimo& emprestimo);
};
