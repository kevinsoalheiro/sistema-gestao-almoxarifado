#pragma once
#include <vector>
#include "Material.hpp"
#include "Ferramenta.hpp"

/** @brief Organiza os materiais e as ferramentas do almoxarifado. */
class Estoque {
private:
    std::vector<Material> materiais_;
    std::vector<Ferramenta> ferramentas_;

public:
    /** @brief Cadastra material sem repetir seu codigo. @param material Material a cadastrar. */
    void cadastrarMaterial(const Material& material);

    /** @brief Cadastra ferramenta sem repetir seu codigo. @param ferramenta Ferramenta a cadastrar. */
    void cadastrarFerramenta(const Ferramenta& ferramenta);

    /** @brief Procura um material pelo codigo. @return Ponteiro para o material ou nullptr. */
    Material* buscarMaterial(int codigo);

    /** @brief Procura uma ferramenta pelo codigo. @return Ponteiro para a ferramenta ou nullptr. */
    Ferramenta* buscarFerramenta(int codigo);

    /** @return Materiais cadastrados. */
    const std::vector<Material>& listarMateriais() const;

    /** @return Ferramentas cadastradas. */
    const std::vector<Ferramenta>& listarFerramentas() const;
};
