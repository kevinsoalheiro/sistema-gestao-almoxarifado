#include "RelatoriosAvancados.hpp"
#include "Data.hpp"
#include <algorithm>
#include <cctype>
#include <map>
#include <sstream>
#include <stdexcept>

namespace {
std::string minusculo(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return s;
}

bool contem(const std::string& nome, const std::string& trecho) {
    return minusculo(nome).find(minusculo(trecho)) != std::string::npos;
}
}

EstatisticasEstoque RelatoriosAvancados::estatisticas(const EstadoSistema& estado) const {
    EstatisticasEstoque e;
    e.materiais = estado.estoque().listarMateriais().size();
    e.ferramentas = estado.estoque().listarFerramentas().size();
    e.usuarios = estado.usuarios().quantidade();
    e.movimentacoes = estado.historico().quantidade();
    for (const auto& material : estado.estoque().listarMateriais()) {
        e.unidadesEmEstoque += material.getQuantidade();
        if (material.estoqueBaixo()) ++e.materiaisBaixos;
    }
    for (const auto& ferramenta : estado.estoque().listarFerramentas()) {
        if (ferramenta.estaDisponivel()) ++e.ferramentasDisponiveis;
        else ++e.ferramentasEmprestadas;
    }
    return e;
}

std::string RelatoriosAvancados::resumoGeral(const EstadoSistema& estado) const {
    const auto e = estatisticas(estado);
    std::ostringstream s;
    s << "RESUMO DO ALMOXARIFADO\n";
    s << "Materiais cadastrados: " << e.materiais << '\n';
    s << "Total de unidades: " << e.unidadesEmEstoque << '\n';
    s << "Materiais no minimo: " << e.materiaisBaixos << '\n';
    s << "Ferramentas: " << e.ferramentas << '\n';
    s << "Disponiveis: " << e.ferramentasDisponiveis << '\n';
    s << "Emprestadas: " << e.ferramentasEmprestadas << '\n';
    s << "Usuarios: " << e.usuarios << '\n';
    s << "Movimentacoes registradas: " << e.movimentacoes << '\n';
    return s.str();
}

std::string RelatoriosAvancados::materiaisBaixos(const EstadoSistema& estado) const {
    std::ostringstream s;
    for (const auto& material : estado.estoque().listarMateriais()) {
        if (!material.estoqueBaixo()) continue;
        s << material.getCodigo() << " - " << material.getNome()
          << " (quantidade: " << material.getQuantidade() << ")\n";
    }
    const auto texto = s.str();
    return texto.empty() ? "Nenhum material com estoque baixo.\n" : texto;
}

std::string RelatoriosAvancados::materiaisPorNome(const EstadoSistema& estado,
                                                  const std::string& trecho) const {
    std::ostringstream s;
    for (const auto& material : estado.estoque().listarMateriais()) {
        if (!contem(material.getNome(), trecho)) continue;
        s << material.getCodigo() << " - " << material.getNome()
          << ": " << material.getQuantidade() << " unidades\n";
    }
    return s.str().empty() ? "Nenhum material encontrado.\n" : s.str();
}

std::string RelatoriosAvancados::ferramentasPorNome(const EstadoSistema& estado,
                                                    const std::string& trecho) const {
    std::ostringstream s;
    for (const auto& ferramenta : estado.estoque().listarFerramentas()) {
        if (!contem(ferramenta.getNome(), trecho)) continue;
        s << ferramenta.getCodigo() << " - " << ferramenta.getNome() << " ("
          << (ferramenta.estaDisponivel() ? "disponivel" : "emprestada") << ")\n";
    }
    return s.str().empty() ? "Nenhuma ferramenta encontrada.\n" : s.str();
}

std::string RelatoriosAvancados::emprestimosAtivos(const EstadoSistema& estado) const {
    std::ostringstream s;
    for (const auto& e : estado.emprestimos().ativos()) {
        s << "Ferramenta " << e.getFerramentaCodigo()
          << " - usuario " << e.getUsuarioId()
          << " desde " << e.getDataRetirada() << '\n';
    }
    return s.str().empty() ? "Nenhum emprestimo ativo.\n" : s.str();
}

std::string RelatoriosAvancados::historicoEmprestimos(const EstadoSistema& estado) const {
    std::ostringstream s;
    for (const auto& e : estado.emprestimos().listar()) {
        s << "Ferramenta " << e.getFerramentaCodigo()
          << " | usuario " << e.getUsuarioId()
          << " | retirada " << e.getDataRetirada();
        if (e.estaAberto()) s << " | aberto\n";
        else s << " | devolvido em " << e.getDataDevolucao() << '\n';
    }
    return s.str().empty() ? "Nenhum emprestimo registrado.\n" : s.str();
}

std::string RelatoriosAvancados::movimentacoesNoPeriodo(
        const EstadoSistema& estado, const std::string& inicio,
        const std::string& fim) const {
    std::ostringstream s;
    const auto resultados = estado.historico().porPeriodo(inicio, fim);
    for (const auto& movimento : resultados) s << movimento.resumo() << '\n';
    return s.str().empty() ? "Nenhuma movimentacao no periodo.\n" : s.str();
}

std::vector<ItemRanking> RelatoriosAvancados::maisMovimentados(
        const EstadoSistema& estado, std::size_t limite) const {
    std::map<int, int> total;
    for (const auto& registro : estado.historico().listar()) {
        total[registro.getCodigoMaterial()] += registro.getQuantidade();
    }
    std::vector<ItemRanking> ranking;
    for (const auto& material : estado.estoque().listarMateriais()) {
        auto it = total.find(material.getCodigo());
        if (it != total.end()) {
            ranking.push_back({material.getCodigo(), material.getNome(), it->second});
        }
    }
    std::sort(ranking.begin(), ranking.end(), [](const ItemRanking& a, const ItemRanking& b) {
        if (a.total != b.total) return a.total > b.total;
        return a.codigo < b.codigo;
    });
    if (ranking.size() > limite) ranking.resize(limite);
    return ranking;
}

std::string RelatoriosAvancados::rankingEmTexto(const EstadoSistema& estado,
                                                std::size_t limite) const {
    std::ostringstream s;
    const auto ranking = maisMovimentados(estado, limite);
    for (std::size_t i = 0; i < ranking.size(); ++i) {
        s << i + 1 << ". " << ranking[i].nome
          << " - " << ranking[i].total << " unidades movimentadas\n";
    }
    return s.str().empty() ? "Ainda nao ha movimentacoes.\n" : s.str();
}

std::string RelatoriosAvancados::resumoPorUsuario(const EstadoSistema& estado, int id) const {
    estado.usuarios().exigirCadastrado(id);
    std::ostringstream s;
    const Usuario* usuario = estado.usuarios().buscar(id);
    s << "Usuario: " << usuario->getNome() << " (#" << id << ")\n";
    for (const auto& movimento : estado.historico().porUsuario(id)) {
        s << movimento.resumo() << '\n';
    }
    for (const auto& emprestimo : estado.emprestimos().porUsuario(id)) {
        s << "Ferramenta " << emprestimo.getFerramentaCodigo()
          << ": " << (emprestimo.estaAberto() ? "em aberto" : "devolvida") << '\n';
    }
    return s.str();
}
