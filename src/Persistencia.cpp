#include "Persistencia.hpp"
#include "Movimentacao.hpp"
#include <fstream>
#include <iomanip>
#include <limits>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

namespace {
constexpr std::size_t MAX_REGISTROS = 100000;

void verificarLeitura(const std::istream& entrada, const std::string& contexto) {
    if (!entrada) throw std::runtime_error("Arquivo invalido: " + contexto);
}

std::size_t lerSecao(std::istream& entrada, const std::string& esperada) {
    std::string titulo;
    unsigned long long quantidade = 0;
    entrada >> titulo >> quantidade;
    verificarLeitura(entrada, esperada);
    if (titulo != esperada || quantidade > MAX_REGISTROS) {
        throw std::runtime_error("Secao invalida: " + esperada);
    }
    return static_cast<std::size_t>(quantidade);
}

void escreverConteudo(const EstadoSistema& estado, std::ostream& saida) {
    saida << "ALMOX1\n";
    const auto& usuarios = estado.usuarios().listar();
    saida << "USUARIOS " << usuarios.size() << '\n';
    for (const auto& u : usuarios) {
        saida << u.getId() << ' ' << static_cast<int>(u.getTipo()) << ' '
              << std::quoted(u.getNome()) << '\n';
    }

    const auto& materiais = estado.estoque().listarMateriais();
    saida << "MATERIAIS " << materiais.size() << '\n';
    for (const auto& m : materiais) {
        // O cabecalho original nao expunha estoqueMinimo_.
        // Gravamos o minimo usando o accessor adicionado para persistencia.
        saida << m.getCodigo() << ' ' << m.getQuantidade() << ' '
              << m.getEstoqueMinimo() << ' ' << std::quoted(m.getNome()) << '\n';
    }

    const auto& ferramentas = estado.estoque().listarFerramentas();
    saida << "FERRAMENTAS " << ferramentas.size() << '\n';
    for (const auto& f : ferramentas) {
        saida << f.getCodigo() << ' ' << std::quoted(f.getNome()) << '\n';
    }

    const auto& emprestimos = estado.emprestimos().listar();
    saida << "EMPRESTIMOS " << emprestimos.size() << '\n';
    for (const auto& e : emprestimos) {
        saida << e.getUsuarioId() << ' ' << e.getFerramentaCodigo() << ' '
              << std::quoted(e.getDataRetirada()) << ' '
              << std::quoted(e.getDataDevolucao()) << '\n';
    }

    const auto& historico = estado.historico().listar();
    saida << "MOVIMENTACOES " << historico.size() << '\n';
    for (const auto& m : historico) {
        saida << m.getId() << ' ' << m.getCodigoMaterial() << ' '
              << m.getIdResponsavel() << ' ' << m.getQuantidade() << ' '
              << static_cast<int>(m.getTipo()) << ' '
              << std::quoted(m.getData()) << ' '
              << std::quoted(m.getObservacao()) << '\n';
    }
    saida << "FIM\n";
    if (!saida) throw std::runtime_error("Falha na gravacao dos dados");
}

EstadoSistema lerConteudo(std::istream& entrada) {
    std::string versao;
    entrada >> versao;
    if (versao != "ALMOX1") throw std::runtime_error("Formato de dados desconhecido");
    EstadoSistema estado;

    const auto nUsuarios = lerSecao(entrada, "USUARIOS");
    for (std::size_t i = 0; i < nUsuarios; ++i) {
        int id = 0, tipo = -1;
        std::string nome;
        entrada >> id >> tipo >> std::quoted(nome);
        verificarLeitura(entrada, "usuario");
        estado.usuarios().cadastrar(id, nome, static_cast<TipoUsuario>(tipo));
    }

    const auto nMateriais = lerSecao(entrada, "MATERIAIS");
    for (std::size_t i = 0; i < nMateriais; ++i) {
        int codigo = 0, quantidade = -1, minimo = -1;
        std::string nome;
        entrada >> codigo >> quantidade >> minimo >> std::quoted(nome);
        verificarLeitura(entrada, "material");
        estado.estoque().cadastrarMaterial(Material(codigo, nome, quantidade, minimo));
    }

    const auto nFerramentas = lerSecao(entrada, "FERRAMENTAS");
    for (std::size_t i = 0; i < nFerramentas; ++i) {
        int codigo = 0;
        std::string nome;
        entrada >> codigo >> std::quoted(nome);
        verificarLeitura(entrada, "ferramenta");
        estado.estoque().cadastrarFerramenta(Ferramenta(codigo, nome));
    }

    const auto nEmprestimos = lerSecao(entrada, "EMPRESTIMOS");
    for (std::size_t i = 0; i < nEmprestimos; ++i) {
        int idUsuario = 0, codigo = 0;
        std::string retirada, devolucao;
        entrada >> idUsuario >> codigo >> std::quoted(retirada) >> std::quoted(devolucao);
        verificarLeitura(entrada, "emprestimo");
        Emprestimo emprestimo(idUsuario, codigo, retirada);
        if (!devolucao.empty()) emprestimo.registrarDevolucao(devolucao);
        estado.emprestimos().restaurar(estado.estoque(), estado.usuarios(), emprestimo);
    }

    const auto nMovimentacoes = lerSecao(entrada, "MOVIMENTACOES");
    for (std::size_t i = 0; i < nMovimentacoes; ++i) {
        int id = 0, codigo = 0, usuario = 0, qtd = 0, tipo = -1;
        std::string data, observacao;
        entrada >> id >> codigo >> usuario >> qtd >> tipo
                >> std::quoted(data) >> std::quoted(observacao);
        verificarLeitura(entrada, "movimentacao");
        estado.historico().restaurarRegistro(
            Movimentacao(id, codigo, usuario, qtd,
                         static_cast<TipoMovimentacao>(tipo), data, observacao));
    }
    std::string fim;
    entrada >> fim;
    if (fim != "FIM") throw std::runtime_error("Arquivo truncado");
    entrada >> std::ws;
    if (entrada.peek() != std::char_traits<char>::eof()) {
        throw std::runtime_error("Dados adicionais apos final do arquivo");
    }
    estado.validarIntegridade();
    return estado;
}

void trocarArquivo(const fs::path& temporario, const fs::path& destino) {
    const fs::path anterior = destino.string() + ".old";
    bool moveuAnterior = false;
    if (fs::exists(destino)) {
        if (fs::exists(anterior)) fs::remove(anterior);
        fs::rename(destino, anterior);
        moveuAnterior = true;
    }
    try {
        fs::rename(temporario, destino);
    } catch (...) {
        if (moveuAnterior) fs::rename(anterior, destino);
        throw;
    }
    if (moveuAnterior) fs::remove(anterior);
}
}

void Persistencia::salvar(const EstadoSistema& estado, const fs::path& arquivo) {
    estado.validarIntegridade();
    if (arquivo.empty()) throw std::invalid_argument("Caminho de arquivo vazio");
    if (!arquivo.parent_path().empty()) fs::create_directories(arquivo.parent_path());
    const fs::path temporario = arquivo.string() + ".tmp";
    try {
        {
            std::ofstream saida(temporario, std::ios::out | std::ios::trunc);
            if (!saida) throw std::runtime_error("Nao foi possivel criar arquivo temporario");
            escreverConteudo(estado, saida);
            saida.flush();
            if (!saida) throw std::runtime_error("Erro ao salvar dados");
        }
        // Recarregamos a copia antes de substituir a versao existente.
        (void)carregar(temporario);
        trocarArquivo(temporario, arquivo);
    } catch (...) {
        if (fs::exists(temporario)) fs::remove(temporario);
        throw;
    }
}

EstadoSistema Persistencia::carregar(const fs::path& arquivo) {
    std::ifstream entrada(arquivo);
    if (!entrada) throw std::runtime_error("Arquivo de dados nao encontrado");
    return lerConteudo(entrada);
}

bool Persistencia::existeArquivo(const fs::path& arquivo) {
    return fs::is_regular_file(arquivo);
}
