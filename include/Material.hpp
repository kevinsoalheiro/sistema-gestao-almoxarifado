#pragma once
#include <string>

/** @brief Representa um material consumivel armazenado. */
class Material {
private:
    int codigo_;
    std::string nome_;
    int quantidade_;
    int estoqueMinimo_;

public:
    /** @brief Cria um material. @param codigo Codigo unico. @param nome Nome do material. @param quantidade Quantidade inicial. @param estoqueMinimo Quantidade minima desejada. */
    Material(int codigo, const std::string& nome, int quantidade, int estoqueMinimo);

    /** @return Codigo do material. */
    int getCodigo() const;

    /** @return Nome do material. */
    const std::string& getNome() const;

    /** @return Quantidade em estoque. */
    int getQuantidade() const;

    /** @brief Verifica se a quantidade atingiu o minimo. @return Verdadeiro se estiver igual ou abaixo do minimo. */
    bool estoqueBaixo() const;

    /** @brief Acrescenta unidades ao estoque. @param quantidade Unidades a acrescentar (valor positivo). */
    void adicionar(int quantidade);

    /** @brief Retira unidades do estoque. @param quantidade Unidades a retirar (sem gerar saldo negativo). */
    void retirar(int quantidade);
};
