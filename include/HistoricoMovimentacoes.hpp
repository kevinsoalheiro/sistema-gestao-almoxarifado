#pragma once
#include "Estoque.hpp"
#include "GestorUsuarios.hpp"
#include "Movimentacao.hpp"
#include <vector>

// Mantem o registro do que entrou e saiu, com data e responsavel.
class HistoricoMovimentacoes {
private:
    std::vector<Movimentacao> registros_;
    int proximoId_ = 1;

public:
    const std::vector<Movimentacao>& listar() const;
    int proximoId() const;
    void registrarEntrada(Estoque& estoque, const GestorUsuarios& usuarios,
                         int codigo, int quantidade, int responsavel,
                         const std::string& data, const std::string& observacao);
    void registrarSaida(Estoque& estoque, const GestorUsuarios& usuarios,
                       int codigo, int quantidade, int responsavel,
                       const std::string& data, const std::string& observacao);
    void restaurarRegistro(const Movimentacao& registro);
    std::vector<Movimentacao> porMaterial(int codigo) const;
    std::vector<Movimentacao> porUsuario(int id) const;
    std::vector<Movimentacao> porPeriodo(const std::string& inicio,
                                        const std::string& fim) const;
    int totalEntradas(int codigo) const;
    int totalSaidas(int codigo) const;
    std::size_t quantidade() const;
    void limpar();
};
