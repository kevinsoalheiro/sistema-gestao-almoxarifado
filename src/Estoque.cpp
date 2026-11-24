#include "Estoque.hpp"
#include <stdexcept>

void Estoque::cadastrarMaterial(const Material& material) {
    if (buscarMaterial(material.getCodigo()) != nullptr) {
        throw std::invalid_argument("Material com codigo repetido");
    }
    materiais_.push_back(material);
}

void Estoque::cadastrarFerramenta(const Ferramenta& ferramenta) {
    if (buscarFerramenta(ferramenta.getCodigo()) != nullptr) {
        throw std::invalid_argument("Ferramenta com codigo repetido");
    }
    ferramentas_.push_back(ferramenta);
}

Material* Estoque::buscarMaterial(int codigo) {
    for (auto& material : materiais_) {
        if (material.getCodigo() == codigo) {
            return &material;
        }
    }
    return nullptr;
}

Ferramenta* Estoque::buscarFerramenta(int codigo) {
    for (auto& ferramenta : ferramentas_) {
        if (ferramenta.getCodigo() == codigo) {
            return &ferramenta;
        }
    }
    return nullptr;
}

const std::vector<Material>& Estoque::listarMateriais() const {
    return materiais_;
}

const std::vector<Ferramenta>& Estoque::listarFerramentas() const {
    return ferramentas_;
}
