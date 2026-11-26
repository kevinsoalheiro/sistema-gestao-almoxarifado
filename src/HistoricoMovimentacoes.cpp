#include "HistoricoMovimentacoes.hpp"
#include "Data.hpp"
#include <algorithm>
#include <limits>
#include <stdexcept>

const std::vector<Movimentacao>& HistoricoMovimentacoes::listar() const {
    return registros_;
}

int HistoricoMovimentacoes::proximoId() const { return proximoId_; }

void HistoricoMovimentacoes::registrarEntrada(Estoque& estoque,
                       const GestorUsuarios& usuarios, int codigo, int quantidade,
                       int responsavel, const std::string& data,
                       const std::string& observacao) {
    usuarios.exigirPermissaoEstoque(responsavel);
    Data::exigirValida(data);
    Material* material = estoque.buscarMaterial(codigo);
    if (!material) throw std::invalid_argument("Material nao encontrado");
    Movimentacao registro(proximoId_, codigo, responsavel, quantidade,
                          TipoMovimentacao::ENTRADA, data, observacao);
    if (proximoId_ == std::numeric_limits<int>::max()) {
        throw std::overflow_error("Limite de registros atingido");
    }
    // Reservar espaco antes da alteracao reduz o risco de estado parcial.
    registros_.reserve(registros_.size() + 1);
    material->adicionar(quantidade);
    registros_.push_back(registro);
    ++proximoId_;
}

void HistoricoMovimentacoes::registrarSaida(Estoque& estoque,
                       const GestorUsuarios& usuarios, int codigo, int quantidade,
                       int responsavel, const std::string& data,
                       const std::string& observacao) {
    usuarios.exigirPermissaoEstoque(responsavel);
    Data::exigirValida(data);
    Material* material = estoque.buscarMaterial(codigo);
    if (!material) throw std::invalid_argument("Material nao encontrado");
    Movimentacao registro(proximoId_, codigo, responsavel, quantidade,
                          TipoMovimentacao::SAIDA, data, observacao);
    if (proximoId_ == std::numeric_limits<int>::max()) {
        throw std::overflow_error("Limite de registros atingido");
    }
    registros_.reserve(registros_.size() + 1);
    material->retirar(quantidade);
    registros_.push_back(registro);
    ++proximoId_;
}

void HistoricoMovimentacoes::restaurarRegistro(const Movimentacao& registro) {
    if (registro.getId() != proximoId_) {
        throw std::invalid_argument("Sequencia do historico inconsistente");
    }
    registros_.push_back(registro);
    ++proximoId_;
}

std::vector<Movimentacao> HistoricoMovimentacoes::porMaterial(int codigo) const {
    std::vector<Movimentacao> resultado;
    for (const auto& registro : registros_) {
        if (registro.getCodigoMaterial() == codigo) resultado.push_back(registro);
    }
    return resultado;
}

std::vector<Movimentacao> HistoricoMovimentacoes::porUsuario(int id) const {
    std::vector<Movimentacao> resultado;
    for (const auto& registro : registros_) {
        if (registro.getIdResponsavel() == id) resultado.push_back(registro);
    }
    return resultado;
}

std::vector<Movimentacao> HistoricoMovimentacoes::porPeriodo(
        const std::string& inicio, const std::string& fim) const {
    Data::exigirValida(inicio);
    Data::exigirValida(fim);
    if (inicio > fim) throw std::invalid_argument("Periodo invertido");
    std::vector<Movimentacao> resultado;
    for (const auto& registro : registros_) {
        if (registro.getData() >= inicio && registro.getData() <= fim) {
            resultado.push_back(registro);
        }
    }
    return resultado;
}

int HistoricoMovimentacoes::totalEntradas(int codigo) const {
    int soma = 0;
    for (const auto& registro : registros_) {
        if (registro.getCodigoMaterial() == codigo &&
            registro.getTipo() == TipoMovimentacao::ENTRADA) {
            soma += registro.getQuantidade();
        }
    }
    return soma;
}

int HistoricoMovimentacoes::totalSaidas(int codigo) const {
    int soma = 0;
    for (const auto& registro : registros_) {
        if (registro.getCodigoMaterial() == codigo &&
            registro.getTipo() == TipoMovimentacao::SAIDA) {
            soma += registro.getQuantidade();
        }
    }
    return soma;
}

std::size_t HistoricoMovimentacoes::quantidade() const { return registros_.size(); }

void HistoricoMovimentacoes::limpar() {
    registros_.clear();
    proximoId_ = 1;
}
