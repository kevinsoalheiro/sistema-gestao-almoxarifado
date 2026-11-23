#pragma once
#include <string>

/** @brief Representa uma ferramenta reutilizavel. */
class Ferramenta {
private:
    int codigo_;
    std::string nome_;
    bool disponivel_;

public:
    /** @brief Cria uma ferramenta inicialmente disponivel. @param codigo Codigo unico. @param nome Nome da ferramenta. */
    Ferramenta(int codigo, const std::string& nome);

    /** @return Codigo da ferramenta. */
    int getCodigo() const;

    /** @return Nome da ferramenta. */
    const std::string& getNome() const;

    /** @return Verdadeiro se a ferramenta estiver disponivel. */
    bool estaDisponivel() const;

    /** @brief Marca a ferramenta como emprestada, se disponivel. */
    void emprestar();

    /** @brief Marca a ferramenta como disponivel apos devolucao. */
    void devolver();
};
