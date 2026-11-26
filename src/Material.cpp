#include "Material.hpp"
#include "Validacao.hpp"
#include <limits>
#include <stdexcept>

Material::Material(int codigo, const std::string& nome, int quantidade, int estoqueMinimo)
    : codigo_(codigo), nome_(nome), quantidade_(quantidade), estoqueMinimo_(estoqueMinimo) {
    Validacao::codigoPositivo(codigo);
    Validacao::nomePreenchido(nome);
    if (quantidade < 0 || estoqueMinimo < 0) {
        throw std::invalid_argument("Quantidade e minimo nao podem ser negativos");
    }
}

int Material::getCodigo() const { return codigo_; }
const std::string& Material::getNome() const { return nome_; }
int Material::getQuantidade() const { return quantidade_; }
int Material::getEstoqueMinimo() const { return estoqueMinimo_; }
bool Material::estoqueBaixo() const { return quantidade_ <= estoqueMinimo_; }

void Material::adicionar(int quantidade) {
    if (quantidade <= 0) {
        throw std::invalid_argument("A quantidade adicionada deve ser positiva");
    }
    if (quantidade > std::numeric_limits<int>::max() - quantidade_) {
        throw std::overflow_error("Quantidade ultrapassa o limite permitido");
    }
    quantidade_ += quantidade;
}

void Material::retirar(int quantidade) {
    if (quantidade <= 0) {
        throw std::invalid_argument("A quantidade retirada deve ser positiva");
    }
    if (quantidade > quantidade_) {
        throw std::invalid_argument("Nao ha material suficiente no estoque");
    }
    quantidade_ -= quantidade;
}
