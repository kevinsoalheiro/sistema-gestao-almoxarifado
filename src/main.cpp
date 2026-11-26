#include "InterfaceTerminal.hpp"
#include <exception>
#include <iostream>

int main() {
    try {
        InterfaceTerminal aplicacao("dados/estado.txt", std::cin, std::cout, std::cerr);
        aplicacao.executar();
        return 0;
    } catch (const std::exception& erro) {
        std::cerr << "Falha ao iniciar o sistema: " << erro.what() << '\n';
        return 1;
    }
}
