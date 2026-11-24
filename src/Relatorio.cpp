#include "Relatorio.hpp"
#include <sstream>

std::string Relatorio::listarMateriais(const Estoque& estoque) const {
    std::ostringstream texto;
    for (const auto& material : estoque.listarMateriais()) {
        texto << material.getNome() << ": " << material.getQuantidade() << "\n";
    }
    return texto.str();
}

std::string Relatorio::listarEstoqueBaixo(const Estoque& estoque) const {
    std::ostringstream texto;
    for (const auto& material : estoque.listarMateriais()) {
        if (material.estoqueBaixo()) {
            texto << material.getNome() << "\n";
        }
    }
    return texto.str();
}

std::string Relatorio::listarFerramentasEmprestadas(const Estoque& estoque) const {
    std::ostringstream texto;
    for (const auto& ferramenta : estoque.listarFerramentas()) {
        if (!ferramenta.estaDisponivel()) {
            texto << ferramenta.getNome() << "\n";
        }
    }
    return texto.str();
}
