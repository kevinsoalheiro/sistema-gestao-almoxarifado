#pragma once
#include <string>

enum class TipoMovimentacao { ENTRADA = 1, SAIDA = 2 };

// Registro imutavel de uma entrada ou retirada de material.
class Movimentacao {
private:
    int id_;
    int codigoMaterial_;
    int idResponsavel_;
    int quantidade_;
    TipoMovimentacao tipo_;
    std::string data_;
    std::string observacao_;

public:
    Movimentacao(int id, int codigoMaterial, int idResponsavel,
                 int quantidade, TipoMovimentacao tipo, const std::string& data,
                 const std::string& observacao);
    int getId() const;
    int getCodigoMaterial() const;
    int getIdResponsavel() const;
    int getQuantidade() const;
    TipoMovimentacao getTipo() const;
    const std::string& getData() const;
    const std::string& getObservacao() const;
    std::string resumo() const;
};
