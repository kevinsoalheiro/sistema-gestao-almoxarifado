#include "Emprestimo.hpp"
#include "Validacao.hpp"
#include <stdexcept>

namespace {

bool dataValida(const std::string& data) {
    // Formato AAAA-MM-DD. Conferimos mes, dia e ano bissexto.
    if (data.size() != 10 || data[4] != '-' || data[7] != '-') {
        return false;
    }
    for (std::size_t i = 0; i < data.size(); ++i) {
        if (i == 4 || i == 7) continue;
        if (data[i] < '0' || data[i] > '9') return false;
    }

    const int ano = std::stoi(data.substr(0, 4));
    const int mes = std::stoi(data.substr(5, 2));
    const int dia = std::stoi(data.substr(8, 2));
    if (ano == 0 || mes < 1 || mes > 12 || dia < 1) return false;

    const bool bissexto = (ano % 4 == 0 && (ano % 100 != 0 || ano % 400 == 0));
    const int diasPorMes[] = {31, bissexto ? 29 : 28, 31, 30, 31, 30,
                             31, 31, 30, 31, 30, 31};
    return dia <= diasPorMes[mes - 1];
}

} // namespace

Emprestimo::Emprestimo(int usuarioId, int ferramentaCodigo,
                       const std::string& dataRetirada)
    : usuarioId_(usuarioId), ferramentaCodigo_(ferramentaCodigo),
      dataRetirada_(dataRetirada), dataDevolucao_(), aberto_(true) {
    Validacao::codigoPositivo(usuarioId);
    Validacao::codigoPositivo(ferramentaCodigo);
    if (!dataValida(dataRetirada)) {
        throw std::invalid_argument("Data de retirada invalida (use AAAA-MM-DD)");
    }
}

int Emprestimo::getUsuarioId() const { return usuarioId_; }
int Emprestimo::getFerramentaCodigo() const { return ferramentaCodigo_; }
const std::string& Emprestimo::getDataRetirada() const { return dataRetirada_; }
bool Emprestimo::estaAberto() const { return aberto_; }

void Emprestimo::registrarDevolucao(const std::string& dataDevolucao) {
    if (!aberto_) {
        throw std::logic_error("O emprestimo ja foi encerrado");
    }
    if (!dataValida(dataDevolucao) || dataDevolucao < dataRetirada_) {
        throw std::invalid_argument("Data de devolucao invalida");
    }
    dataDevolucao_ = dataDevolucao;
    aberto_ = false;
}
