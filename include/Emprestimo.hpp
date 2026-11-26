#pragma once
#include <string>

/** @brief Registra a retirada e a devolucao de uma ferramenta. */
class Emprestimo {
private:
    int usuarioId_;
    int ferramentaCodigo_;
    std::string dataRetirada_;
    std::string dataDevolucao_;
    bool aberto_;

public:
    /** @brief Cria um emprestimo aberto. @param usuarioId Identificador de quem retirou. @param ferramentaCodigo Codigo da ferramenta. @param dataRetirada Data da retirada. */
    Emprestimo(int usuarioId, int ferramentaCodigo, const std::string& dataRetirada);

    /** @return Identificador do usuario. */
    int getUsuarioId() const;

    /** @return Codigo da ferramenta. */
    int getFerramentaCodigo() const;

    /** @return Data de retirada. */
    const std::string& getDataRetirada() const;

    /** @return Verdadeiro quando a devolucao ainda nao foi registrada. */
    bool estaAberto() const;

    /** @return Data de devolucao ou string vazia quando aberto. */
    const std::string& getDataDevolucao() const;

    /** @brief Registra devolucao e encerra emprestimo aberto. @param dataDevolucao Data da devolucao. */
    void registrarDevolucao(const std::string& dataDevolucao);
};
