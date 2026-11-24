#include "Material.hpp"
#include <stdexcept>

Material::Material(int codigo, const std::string& nome, int quantidade, int estoqueMinimo)
    : codigo_(codigo), nome_(nome), quantidade_(quantidade), estoqueMinimo_(estoqueMinimo) {
    if (quantidade < 0 || estoqueMinimo < 0) {
        throw std::invalid_argument("Quantidade nao pode ser negativa");
    }
}

int Material::getCodigo() const { return codigo_; }
const std::string& Material::getNome() const { return nome_; }
int Material::getQuantidade() const { return quantidade_; }
bool Material::estoqueBaixo() const { return quantidade_ <= estoqueMinimo_; }

void Material::adicionar(int quantidade) {
    if (quantidade <= 0) {
        throw std::invalid_argument("Adicao deve ser positiva");
    }
    quantidade_ += quantidade;
}

void Material::retirar(int quantidade) {
    if (quantidade <= 0 || quantidade > quantidade_) {
        throw std::invalid_argument("Retirada invalida");
    }
    quantidade_ -= quantidade;
}
