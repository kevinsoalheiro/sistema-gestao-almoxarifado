#include "Emprestimo.hpp"
#include <stdexcept>

Emprestimo::Emprestimo(int usuarioId, int ferramentaCodigo,
                       const std::string& dataRetirada)
    : usuarioId_(usuarioId), ferramentaCodigo_(ferramentaCodigo),
      dataRetirada_(dataRetirada), dataDevolucao_(), aberto_(true) {}

int Emprestimo::getUsuarioId() const { return usuarioId_; }
int Emprestimo::getFerramentaCodigo() const { return ferramentaCodigo_; }
const std::string& Emprestimo::getDataRetirada() const { return dataRetirada_; }
bool Emprestimo::estaAberto() const { return aberto_; }

void Emprestimo::registrarDevolucao(const std::string& dataDevolucao) {
    if (!aberto_) {
        throw std::logic_error("Emprestimo ja encerrado");
    }
    dataDevolucao_ = dataDevolucao;
    aberto_ = false;
}
