#include "Usuario.hpp"
#include "Validacao.hpp"

Usuario::Usuario(int id, const std::string& nome, TipoUsuario tipo)
    : id_(id), nome_(nome), tipo_(tipo) {
    Validacao::codigoPositivo(id);
    Validacao::nomePreenchido(nome);
    if (tipo != TipoUsuario::ALMOXARIFE &&
        tipo != TipoUsuario::FUNCIONARIO &&
        tipo != TipoUsuario::GERENTE) {
        throw std::invalid_argument("Tipo de usuario invalido");
    }
}

int Usuario::getId() const { return id_; }
const std::string& Usuario::getNome() const { return nome_; }
TipoUsuario Usuario::getTipo() const { return tipo_; }

bool Usuario::podeGerenciarEstoque() const {
    return tipo_ == TipoUsuario::ALMOXARIFE || tipo_ == TipoUsuario::GERENTE;
}
