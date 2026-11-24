#pragma once

#include <stdexcept>
#include <string>

// Funcoes pequenas compartilhadas pelas classes.
namespace Validacao {

inline void codigoPositivo(int codigo) {
    if (codigo <= 0) {
        throw std::invalid_argument("O codigo ou ID deve ser positivo");
    }
}

inline void nomePreenchido(const std::string& nome) {
    if (nome.find_first_not_of(" \t\n\r") == std::string::npos) {
        throw std::invalid_argument("O nome nao pode ficar vazio");
    }
}

} // namespace Validacao
