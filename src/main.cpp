#include "Emprestimo.hpp"
#include "Estoque.hpp"
#include "Relatorio.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

std::string lerTexto(const std::string& pergunta) {
    std::cout << pergunta;
    std::string texto;
    if (!std::getline(std::cin, texto)) {
        throw std::runtime_error("Entrada encerrada");
    }
    return texto;
}

int lerNumero(const std::string& pergunta) {
    std::istringstream entrada(lerTexto(pergunta));
    int numero;
    std::string sobra;
    if (!(entrada >> numero) || (entrada >> sobra)) {
        throw std::invalid_argument("Digite um numero inteiro valido");
    }
    return numero;
}

void mostrarTexto(const std::string& texto) {
    std::cout << (texto.empty() ? "Nenhum item encontrado.\n" : texto);
}

} // namespace

int main() {
    Estoque estoque;
    std::vector<Emprestimo> emprestimos; // RAII: sem new/delete manual
    Relatorio relatorio;

    std::cout << "Sistema de Gestao de Almoxarifado e Ferramentas\n";

    while (true) {
        std::cout << "\n1 - Cadastrar material\n"
                  << "2 - Cadastrar ferramenta\n"
                  << "3 - Listar materiais\n"
                  << "4 - Listar ferramentas\n"
                  << "5 - Adicionar material\n"
                  << "6 - Retirar material\n"
                  << "7 - Emprestar ferramenta\n"
                  << "8 - Devolver ferramenta\n"
                  << "9 - Relatorio de estoque baixo\n"
                  << "10 - Relatorio de ferramentas emprestadas\n"
                  << "0 - Sair\n";

        try {
            const int opcao = lerNumero("Opcao: ");
            if (opcao == 0) break;

            switch (opcao) {
                case 1: {
                    int codigo = lerNumero("Codigo: ");
                    std::string nome = lerTexto("Nome: ");
                    int qtd = lerNumero("Quantidade: ");
                    int minimo = lerNumero("Estoque minimo: ");
                    estoque.cadastrarMaterial(Material(codigo, nome, qtd, minimo));
                    std::cout << "Material cadastrado.\n";
                    break;
                }
                case 2: {
                    int codigo = lerNumero("Codigo: ");
                    std::string nome = lerTexto("Nome: ");
                    estoque.cadastrarFerramenta(Ferramenta(codigo, nome));
                    std::cout << "Ferramenta cadastrada.\n";
                    break;
                }
                case 3:
                    mostrarTexto(relatorio.listarMateriais(estoque));
                    break;
                case 4:
                    if (estoque.listarFerramentas().empty()) {
                        std::cout << "Nenhuma ferramenta cadastrada.\n";
                    }
                    for (const auto& ferramenta : estoque.listarFerramentas()) {
                        std::cout << ferramenta.getCodigo() << " - " << ferramenta.getNome()
                                  << (ferramenta.estaDisponivel() ? " (disponivel)\n" : " (emprestada)\n");
                    }
                    break;
                case 5:
                case 6: {
                    int codigo = lerNumero("Codigo do material: ");
                    Material* material = estoque.buscarMaterial(codigo);
                    if (material == nullptr) throw std::invalid_argument("Material nao encontrado");
                    int qtd = lerNumero("Quantidade: ");
                    if (opcao == 5) material->adicionar(qtd);
                    else material->retirar(qtd);
                    std::cout << "Estoque atualizado.\n";
                    break;
                }
                case 7: {
                    int codigo = lerNumero("Codigo da ferramenta: ");
                    Ferramenta* ferramenta = estoque.buscarFerramenta(codigo);
                    if (ferramenta == nullptr) throw std::invalid_argument("Ferramenta nao encontrada");
                    if (!ferramenta->estaDisponivel()) throw std::logic_error("Ferramenta ja emprestada");
                    int id = lerNumero("ID do usuario: ");
                    std::string data = lerTexto("Data da retirada (AAAA-MM-DD): ");
                    // Criamos o registro antes de alterar o estado da ferramenta.
                    emprestimos.emplace_back(id, codigo, data);
                    ferramenta->emprestar();
                    std::cout << "Emprestimo registrado.\n";
                    break;
                }
                case 8: {
                    int codigo = lerNumero("Codigo da ferramenta: ");
                    Ferramenta* ferramenta = estoque.buscarFerramenta(codigo);
                    if (ferramenta == nullptr) throw std::invalid_argument("Ferramenta nao encontrada");
                    if (ferramenta->estaDisponivel()) throw std::logic_error("Ferramenta nao esta emprestada");
                    Emprestimo* aberto = nullptr;
                    for (auto& emprestimo : emprestimos) {
                        if (emprestimo.getFerramentaCodigo() == codigo && emprestimo.estaAberto()) {
                            aberto = &emprestimo;
                            break;
                        }
                    }
                    if (aberto == nullptr) throw std::logic_error("Emprestimo aberto nao encontrado");
                    std::string data = lerTexto("Data da devolucao (AAAA-MM-DD): ");
                    aberto->registrarDevolucao(data);
                    ferramenta->devolver();
                    std::cout << "Devolucao registrada.\n";
                    break;
                }
                case 9:
                    mostrarTexto(relatorio.listarEstoqueBaixo(estoque));
                    break;
                case 10:
                    mostrarTexto(relatorio.listarFerramentasEmprestadas(estoque));
                    break;
                default:
                    std::cout << "Opcao desconhecida.\n";
            }
        } catch (const std::exception& erro) {
            if (!std::cin) break;
            std::cerr << "Erro: " << erro.what() << '\n';
        }
    }

    std::cout << "Programa encerrado.\n";
    return 0;
}
