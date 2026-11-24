#include "Ferramenta.hpp"
#include <stdexcept>

Ferramenta::Ferramenta(int codigo, const std::string& nome)
    : codigo_(codigo), nome_(nome), disponivel_(true) {}

int Ferramenta::getCodigo() const { return codigo_; }
const std::string& Ferramenta::getNome() const { return nome_; }
bool Ferramenta::estaDisponivel() const { return disponivel_; }

void Ferramenta::emprestar() {
    if (!disponivel_) {
        throw std::logic_error("Ferramenta ja emprestada");
    }
    disponivel_ = false;
}

void Ferramenta::devolver() {
    if (disponivel_) {
        throw std::logic_error("Ferramenta ja disponivel");
    }
    disponivel_ = true;
}
