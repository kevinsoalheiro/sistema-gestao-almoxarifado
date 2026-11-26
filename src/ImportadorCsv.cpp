#include "ImportadorCsv.hpp"
#include <climits>
#include <fstream>
#include <stdexcept>
#include <utility>

namespace fs = std::filesystem;

namespace {
int numeroInteiro(const std::string& valor) {
    if (valor.empty()) throw std::invalid_argument("Numero vazio no CSV");
    std::size_t usados = 0;
    long long numero = 0;
    try {
        numero = std::stoll(valor, &usados);
    } catch (const std::exception&) {
        throw std::invalid_argument("Numero invalido no CSV: " + valor);
    }
    if (usados != valor.size() || numero < INT_MIN || numero > INT_MAX) {
        throw std::invalid_argument("Numero fora do intervalo no CSV");
    }
    return static_cast<int>(numero);
}

std::ifstream abrirCsv(const fs::path& arquivo, const std::string& cabecalho) {
    if (arquivo.extension() != ".csv") {
        throw std::invalid_argument("Arquivo de importacao deve ser CSV");
    }
    std::ifstream entrada(arquivo);
    if (!entrada) throw std::runtime_error("Nao foi possivel ler o CSV");
    std::string primeiraLinha;
    if (!std::getline(entrada, primeiraLinha) || primeiraLinha != cabecalho) {
        throw std::invalid_argument("Cabecalho CSV nao reconhecido");
    }
    return entrada;
}

void validarQuantidadeCampos(const std::vector<std::string>& campos, std::size_t esperado) {
    if (campos.size() != esperado) {
        throw std::invalid_argument("Numero incorreto de colunas no CSV");
    }
}

void semFormulaSuspeita(const std::string& texto) {
    // Evita importar formulas disfarçadas de nomes.
    if (!texto.empty() &&
        (texto[0] == '=' || texto[0] == '+' || texto[0] == '-' || texto[0] == '@')) {
        throw std::invalid_argument("Nome iniciado por operador de planilha");
    }
}

std::string limparApostrofo(const std::string& s) {
    // O exportador adiciona um apostrofo para neutralizar formula no Excel.
    if (s.size() >= 2 && s[0] == '\'' &&
        (s[1] == '=' || s[1] == '+' || s[1] == '-' || s[1] == '@')) {
        return s.substr(1);
    }
    return s;
}
}

std::vector<std::string> ImportadorCsv::separarCampos(const std::string& linha) {
    std::vector<std::string> campos;
    std::string atual;
    bool entreAspas = false;
    bool aspasJaFechadas = false;
    for (std::size_t i = 0; i < linha.size(); ++i) {
        char c = linha[i];
        if (c == '"') {
            if (entreAspas && i + 1 < linha.size() && linha[i + 1] == '"') {
                atual += '"';
                ++i;
            } else if (entreAspas) {
                entreAspas = false;
                aspasJaFechadas = true;
            } else if (atual.empty() && !aspasJaFechadas) {
                entreAspas = true;
            } else {
                throw std::invalid_argument("Aspas fora de posicao no CSV");
            }
        } else if (c == ';' && !entreAspas) {
            campos.push_back(atual);
            atual.clear();
            aspasJaFechadas = false;
        } else {
            if (aspasJaFechadas) {
                throw std::invalid_argument("Caractere apos aspas do CSV");
            }
            atual += c;
        }
    }
    if (entreAspas) throw std::invalid_argument("Campo CSV com aspas abertas");
    campos.push_back(atual);
    return campos;
}

std::size_t ImportadorCsv::importarMateriais(EstadoSistema& estado,
                                             const fs::path& arquivo) {
    auto entrada = abrirCsv(arquivo, "codigo;nome;quantidade;minimo;estoque_baixo");
    Estoque novoEstoque = estado.estoque();
    std::string linha;
    std::size_t quantidade = 0;
    while (std::getline(entrada, linha)) {
        if (linha.empty()) continue;
        const auto campos = separarCampos(linha);
        validarQuantidadeCampos(campos, 5);
        const int codigo = numeroInteiro(campos[0]);
        const std::string nome = limparApostrofo(campos[1]);
        semFormulaSuspeita(nome);
        const int qtd = numeroInteiro(campos[2]);
        const int minimo = numeroInteiro(campos[3]);
        Material material(codigo, nome, qtd, minimo);
        if (campos[4] != "sim" && campos[4] != "nao") {
            throw std::invalid_argument("Coluna de estoque baixo invalida");
        }
        if (material.estoqueBaixo() != (campos[4] == "sim")) {
            throw std::invalid_argument("Flag de estoque baixo inconsistente");
        }
        novoEstoque.cadastrarMaterial(material);
        ++quantidade;
    }
    if (!entrada.eof()) throw std::runtime_error("Erro na leitura de CSV");
    estado.estoque() = std::move(novoEstoque);
    return quantidade;
}

std::size_t ImportadorCsv::importarFerramentas(EstadoSistema& estado,
                                               const fs::path& arquivo) {
    auto entrada = abrirCsv(arquivo, "codigo;nome;situacao");
    Estoque novoEstoque = estado.estoque();
    std::string linha;
    std::size_t quantidade = 0;
    while (std::getline(entrada, linha)) {
        if (linha.empty()) continue;
        const auto campos = separarCampos(linha);
        validarQuantidadeCampos(campos, 3);
        const int codigo = numeroInteiro(campos[0]);
        const std::string nome = limparApostrofo(campos[1]);
        semFormulaSuspeita(nome);
        if (campos[2] != "disponivel") {
            throw std::invalid_argument("Ferramentas emprestadas exigem historico de emprestimo");
        }
        novoEstoque.cadastrarFerramenta(Ferramenta(codigo, nome));
        ++quantidade;
    }
    if (!entrada.eof()) throw std::runtime_error("Erro na leitura de CSV");
    estado.estoque() = std::move(novoEstoque);
    return quantidade;
}
