#include "Auditoria.hpp"
#include <sstream>
#include <stdexcept>

int Auditoria::contagemDeMateriaisBaixos(const EstadoSistema& estado) {
    int total = 0;
    for (const auto& material : estado.estoque().listarMateriais()) {
        if (material.estoqueBaixo()) ++total;
    }
    return total;
}

int Auditoria::contagemDeFerramentasEmprestadas(const EstadoSistema& estado) {
    int total = 0;
    for (const auto& ferramenta : estado.estoque().listarFerramentas()) {
        if (!ferramenta.estaDisponivel()) ++total;
    }
    return total;
}

bool Auditoria::semInconsistencias(const EstadoSistema& estado) {
    try {
        estado.validarIntegridade();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::vector<std::string> Auditoria::avisos(const EstadoSistema& estado) {
    std::vector<std::string> mensagens;
    try {
        estado.validarIntegridade();
    } catch (const std::exception& erro) {
        mensagens.push_back(std::string("ERRO: ") + erro.what());
    }

    for (const auto& material : estado.estoque().listarMateriais()) {
        if (material.estoqueBaixo()) {
            std::ostringstream ss;
            ss << "AVISO: estoque baixo em " << material.getNome()
               << " (" << material.getQuantidade() << " unidades)";
            mensagens.push_back(ss.str());
        }
    }

    for (const auto& ferramenta : estado.estoque().listarFerramentas()) {
        if (!ferramenta.estaDisponivel()) {
            mensagens.push_back("INFO: ferramenta emprestada: " + ferramenta.getNome());
        }
    }

    if (estado.usuarios().quantidadePorPerfil(TipoUsuario::ALMOXARIFE) == 0 &&
        estado.usuarios().quantidadePorPerfil(TipoUsuario::GERENTE) == 0) {
        mensagens.push_back("AVISO: nenhum usuario pode gerenciar o estoque");
    }

    if (estado.usuarios().quantidade() == 0) {
        mensagens.push_back("AVISO: nenhum usuario cadastrado");
    }

    if (mensagens.empty()) mensagens.push_back("Nenhum problema ou alerta encontrado");
    return mensagens;
}

std::string Auditoria::relatorio(const EstadoSistema& estado) {
    std::ostringstream saida;
    saida << "AUDITORIA DO ALMOXARIFADO\n";
    saida << "Integridade dos relacionamentos: "
          << (semInconsistencias(estado) ? "OK" : "ERRO") << '\n';
    saida << "Materiais em estoque baixo: " << contagemDeMateriaisBaixos(estado) << '\n';
    saida << "Ferramentas emprestadas: " << contagemDeFerramentasEmprestadas(estado) << '\n';
    saida << "Registros de movimentacao: " << estado.historico().quantidade() << '\n';
    saida << "Registros de emprestimo: " << estado.emprestimos().quantidade() << '\n';
    for (const auto& aviso : avisos(estado)) saida << "- " << aviso << '\n';
    return saida.str();
}
