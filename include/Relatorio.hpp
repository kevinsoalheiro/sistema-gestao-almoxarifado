#pragma once
#include <string>
#include "Estoque.hpp"

/** @brief Prepara consultas simples sobre a situacao do almoxarifado. */
class Relatorio {
public:
    /** @brief Lista materiais com suas quantidades. @param estoque Estoque consultado. @return Texto do relatorio. */
    std::string listarMateriais(const Estoque& estoque) const;

    /** @brief Lista materiais com quantidade igual ou abaixo do minimo. @param estoque Estoque consultado. @return Texto do relatorio. */
    std::string listarEstoqueBaixo(const Estoque& estoque) const;

    /** @brief Lista ferramentas que estao emprestadas. @param estoque Estoque consultado. @return Texto do relatorio. */
    std::string listarFerramentasEmprestadas(const Estoque& estoque) const;
};
