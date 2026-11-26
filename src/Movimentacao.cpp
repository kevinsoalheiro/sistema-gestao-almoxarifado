#include "Movimentacao.hpp"
#include "Data.hpp"
#include "Validacao.hpp"
#include <sstream>
#include <stdexcept>

Movimentacao::Movimentacao(int id, int codigoMaterial, int idResponsavel,
                           int quantidade, TipoMovimentacao tipo,
                           const std::string& data, const std::string& observacao)
    : id_(id), codigoMaterial_(codigoMaterial), idResponsavel_(idResponsavel),
      quantidade_(quantidade), tipo_(tipo), data_(data), observacao_(observacao) {
    Validacao::codigoPositivo(id);
    Validacao::codigoPositivo(codigoMaterial);
    Validacao::codigoPositivo(idResponsavel);
    Validacao::codigoPositivo(quantidade);
    if (tipo != TipoMovimentacao::ENTRADA && tipo != TipoMovimentacao::SAIDA) {
        throw std::invalid_argument("Tipo de movimentacao invalido");
    }
    Data::exigirValida(data);
    if (observacao.size() > 300) {
        throw std::invalid_argument("Observacao muito longa");
    }
}

int Movimentacao::getId() const { return id_; }
int Movimentacao::getCodigoMaterial() const { return codigoMaterial_; }
int Movimentacao::getIdResponsavel() const { return idResponsavel_; }
int Movimentacao::getQuantidade() const { return quantidade_; }
TipoMovimentacao Movimentacao::getTipo() const { return tipo_; }
const std::string& Movimentacao::getData() const { return data_; }
const std::string& Movimentacao::getObservacao() const { return observacao_; }

std::string Movimentacao::resumo() const {
    std::ostringstream ss;
    ss << '#' << id_ << ' ' << data_ << ' '
       << (tipo_ == TipoMovimentacao::ENTRADA ? "ENTRADA" : "SAIDA")
       << " material=" << codigoMaterial_ << " quantidade=" << quantidade_
       << " responsavel=" << idResponsavel_;
    if (!observacao_.empty()) ss << " (" << observacao_ << ')';
    return ss.str();
}
