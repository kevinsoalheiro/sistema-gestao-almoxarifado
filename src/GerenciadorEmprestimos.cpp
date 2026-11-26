#include "GerenciadorEmprestimos.hpp"
#include <stdexcept>

const std::vector<Emprestimo>& GerenciadorEmprestimos::listar() const {
    return emprestimos_;
}

std::size_t GerenciadorEmprestimos::quantidade() const {
    return emprestimos_.size();
}

std::size_t GerenciadorEmprestimos::quantidadeAbertos() const {
    std::size_t quantidade = 0;
    for (const auto& emprestimo : emprestimos_) {
        if (emprestimo.estaAberto()) ++quantidade;
    }
    return quantidade;
}

const Emprestimo* GerenciadorEmprestimos::buscarAberto(int codigoFerramenta) const {
    for (const auto& emprestimo : emprestimos_) {
        if (emprestimo.getFerramentaCodigo() == codigoFerramenta &&
            emprestimo.estaAberto()) return &emprestimo;
    }
    return nullptr;
}

std::vector<Emprestimo> GerenciadorEmprestimos::porUsuario(int usuarioId) const {
    std::vector<Emprestimo> resultado;
    for (const auto& emprestimo : emprestimos_) {
        if (emprestimo.getUsuarioId() == usuarioId) resultado.push_back(emprestimo);
    }
    return resultado;
}

std::vector<Emprestimo> GerenciadorEmprestimos::ativos() const {
    std::vector<Emprestimo> resultado;
    for (const auto& emprestimo : emprestimos_) {
        if (emprestimo.estaAberto()) resultado.push_back(emprestimo);
    }
    return resultado;
}

void GerenciadorEmprestimos::emprestar(Estoque& estoque,
                                    const GestorUsuarios& usuarios,
                                    int codigoFerramenta, int usuarioId,
                                    const std::string& data) {
    usuarios.exigirCadastrado(usuarioId);
    Ferramenta* ferramenta = estoque.buscarFerramenta(codigoFerramenta);
    if (!ferramenta) throw std::invalid_argument("Ferramenta nao encontrada");
    if (!ferramenta->estaDisponivel() || buscarAberto(codigoFerramenta)) {
        throw std::logic_error("Ferramenta indisponivel para emprestimo");
    }
    Emprestimo emprestimo(usuarioId, codigoFerramenta, data);
    emprestimos_.push_back(emprestimo);
    ferramenta->emprestar();
}

void GerenciadorEmprestimos::devolver(Estoque& estoque, int codigoFerramenta,
                                     const std::string& data) {
    Ferramenta* ferramenta = estoque.buscarFerramenta(codigoFerramenta);
    if (!ferramenta) throw std::invalid_argument("Ferramenta nao encontrada");
    for (auto& emprestimo : emprestimos_) {
        if (emprestimo.getFerramentaCodigo() == codigoFerramenta &&
            emprestimo.estaAberto()) {
            if (ferramenta->estaDisponivel()) {
                throw std::logic_error("Estoque e emprestimos inconsistentes");
            }
            // Se a data for invalida, nenhuma devolucao sera registrada.
            emprestimo.registrarDevolucao(data);
            ferramenta->devolver();
            return;
        }
    }
    throw std::logic_error("Nao existe emprestimo aberto desta ferramenta");
}

void GerenciadorEmprestimos::restaurar(Estoque& estoque,
                                      const GestorUsuarios& usuarios,
                                      const Emprestimo& emprestimo) {
    usuarios.exigirCadastrado(emprestimo.getUsuarioId());
    Ferramenta* ferramenta = estoque.buscarFerramenta(emprestimo.getFerramentaCodigo());
    if (!ferramenta) throw std::invalid_argument("Emprestimo referencia ferramenta inexistente");
    if (emprestimo.estaAberto()) {
        if (buscarAberto(emprestimo.getFerramentaCodigo()) ||
            !ferramenta->estaDisponivel()) {
            throw std::invalid_argument("Emprestimo ativo duplicado no arquivo");
        }
    }
    emprestimos_.push_back(emprestimo);
    if (emprestimo.estaAberto()) ferramenta->emprestar();
}
