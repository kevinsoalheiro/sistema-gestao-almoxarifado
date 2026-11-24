#include "Ferramenta.hpp"
#include "Validacao.hpp"
#include <stdexcept>

Ferramenta::Ferramenta(int codigo, const std::string& nome)
    : codigo_(codigo), nome_(nome), disponivel_(true) {
    Validacao::codigoPositivo(codigo);
    Validacao::nomePreenchido(nome);
}

int Ferramenta::getCodigo() const { return codigo_; }
const std::string& Ferramenta::getNome() const { return nome_; }
bool Ferramenta::estaDisponivel() const { return disponivel_; }

void Ferramenta::emprestar() {
    if (!disponivel_) {
        throw std::logic_error("A ferramenta ja esta emprestada");
    }
    disponivel_ = false;
}

void Ferramenta::devolver() {
    if (disponivel_) {
        throw std::logic_error("A ferramenta ja esta disponivel");
    }
    disponivel_ = true;
}
