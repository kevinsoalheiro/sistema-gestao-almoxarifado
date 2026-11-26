#pragma once
#include <string>

// Validacoes de datas compartilhadas entre emprestimos e movimentacoes.
namespace Data {
    bool valida(const std::string& texto);
    void exigirValida(const std::string& texto);
    bool anteriorOuIgual(const std::string& primeira, const std::string& segunda);
    bool dentroDoPeriodo(const std::string& data,
                         const std::string& inicio, const std::string& fim);
    std::string hoje();
}
