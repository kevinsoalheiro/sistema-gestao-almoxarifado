#include "Estoque.hpp"
#include "Relatorio.hpp"
#include <iostream>

int main() {
    Estoque estoque;
    estoque.cadastrarMaterial(Material(10, "Parafuso", 5, 2));
    estoque.cadastrarFerramenta(Ferramenta(20, "Martelo"));
    std::cout << "Sistema de Gestao de Almoxarifado e Ferramentas\n";
    std::cout << Relatorio().listarMateriais(estoque);
    return 0;
}
