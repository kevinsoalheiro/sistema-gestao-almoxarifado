#pragma once
#include "EstadoSistema.hpp"
#include <cstddef>
#include <string>
#include <vector>

struct EstatisticasEstoque {
    std::size_t materiais = 0;
    std::size_t ferramentas = 0;
    std::size_t ferramentasDisponiveis = 0;
    std::size_t ferramentasEmprestadas = 0;
    std::size_t materiaisBaixos = 0;
    long long unidadesEmEstoque = 0;
    std::size_t usuarios = 0;
    std::size_t movimentacoes = 0;
};

struct ItemRanking {
    int codigo = 0;
    std::string nome;
    int total = 0;
};

// Consultas sem alterar os dados (responsabilidade separada do estoque).
class RelatoriosAvancados {
public:
    EstatisticasEstoque estatisticas(const EstadoSistema& estado) const;
    std::string resumoGeral(const EstadoSistema& estado) const;
    std::string materiaisBaixos(const EstadoSistema& estado) const;
    std::string materiaisPorNome(const EstadoSistema& estado,
                                const std::string& trecho) const;
    std::string ferramentasPorNome(const EstadoSistema& estado,
                                  const std::string& trecho) const;
    std::string emprestimosAtivos(const EstadoSistema& estado) const;
    std::string historicoEmprestimos(const EstadoSistema& estado) const;
    std::string movimentacoesNoPeriodo(const EstadoSistema& estado,
                                       const std::string& inicio,
                                       const std::string& fim) const;
    std::vector<ItemRanking> maisMovimentados(const EstadoSistema& estado,
                                              std::size_t limite) const;
    std::string rankingEmTexto(const EstadoSistema& estado,
                              std::size_t limite = 10) const;
    std::string resumoPorUsuario(const EstadoSistema& estado, int id) const;
};
