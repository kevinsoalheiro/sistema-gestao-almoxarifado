#include "EstadoSistema.hpp"
#include <stdexcept>

Estoque& EstadoSistema::estoque() { return estoque_; }
const Estoque& EstadoSistema::estoque() const { return estoque_; }
GestorUsuarios& EstadoSistema::usuarios() { return usuarios_; }
const GestorUsuarios& EstadoSistema::usuarios() const { return usuarios_; }
GerenciadorEmprestimos& EstadoSistema::emprestimos() { return emprestimos_; }
const GerenciadorEmprestimos& EstadoSistema::emprestimos() const { return emprestimos_; }
HistoricoMovimentacoes& EstadoSistema::historico() { return historico_; }
const HistoricoMovimentacoes& EstadoSistema::historico() const { return historico_; }

std::size_t EstadoSistema::totalItens() const {
    return estoque_.listarMateriais().size() + estoque_.listarFerramentas().size();
}

bool EstadoSistema::vazio() const {
    return totalItens() == 0 && usuarios_.quantidade() == 0 &&
           emprestimos_.quantidade() == 0 && historico_.quantidade() == 0;
}

void EstadoSistema::validarIntegridade() const {
    // Cada ferramenta indisponivel deve possuir um emprestimo ativo correspondente.
    for (const auto& ferramenta : estoque_.listarFerramentas()) {
        const bool emAberto = emprestimos_.buscarAberto(ferramenta.getCodigo()) != nullptr;
        if (emAberto == ferramenta.estaDisponivel()) {
            throw std::logic_error("Inconsistencia entre ferramenta e emprestimo");
        }
    }
    // Nao deixar um emprestimo apontar para um usuario removido.
    for (const auto& emprestimo : emprestimos_.listar()) {
        if (!usuarios_.existe(emprestimo.getUsuarioId())) {
            throw std::logic_error("Emprestimo referencia usuario inexistente");
        }
    }
    for (const auto& movimento : historico_.listar()) {
        bool existeMaterial = false;
        for (const auto& material : estoque_.listarMateriais()) {
            if (material.getCodigo() == movimento.getCodigoMaterial()) {
                existeMaterial = true;
                break;
            }
        }
        if (!existeMaterial || !usuarios_.existe(movimento.getIdResponsavel())) {
            throw std::logic_error("Historico referencia item ou usuario inexistente");
        }
    }
}
