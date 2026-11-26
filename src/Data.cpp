#include "Data.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace {
int numero(const std::string& s, std::size_t inicio, std::size_t quantidade) {
    int n = 0;
    for (std::size_t i = inicio; i < inicio + quantidade; ++i) {
        if (s[i] < '0' || s[i] > '9') return -1;
        n = n * 10 + (s[i] - '0');
    }
    return n;
}

bool bissexto(int ano) {
    return ano % 4 == 0 && (ano % 100 != 0 || ano % 400 == 0);
}
} // namespace

namespace Data {
bool valida(const std::string& texto) {
    if (texto.size() != 10 || texto[4] != '-' || texto[7] != '-') return false;
    const int ano = numero(texto, 0, 4);
    const int mes = numero(texto, 5, 2);
    const int dia = numero(texto, 8, 2);
    if (ano < 1 || mes < 1 || mes > 12 || dia < 1) return false;
    const int dias[12] = {31, bissexto(ano) ? 29 : 28, 31, 30, 31, 30,
                          31, 31, 30, 31, 30, 31};
    return dia <= dias[mes - 1];
}

void exigirValida(const std::string& texto) {
    if (!valida(texto)) {
        throw std::invalid_argument("Data invalida. Formato esperado: AAAA-MM-DD");
    }
}

bool anteriorOuIgual(const std::string& primeira, const std::string& segunda) {
    exigirValida(primeira);
    exigirValida(segunda);
    return primeira <= segunda;
}

bool dentroDoPeriodo(const std::string& data,
                     const std::string& inicio, const std::string& fim) {
    exigirValida(data);
    exigirValida(inicio);
    exigirValida(fim);
    if (inicio > fim) throw std::invalid_argument("Periodo invertido");
    return inicio <= data && data <= fim;
}

std::string hoje() {
    const auto agora = std::chrono::system_clock::now();
    const std::time_t tempo = std::chrono::system_clock::to_time_t(agora);
    std::tm calendario{};
#ifdef _WIN32
    localtime_s(&calendario, &tempo);
#else
    localtime_r(&tempo, &calendario);
#endif
    std::ostringstream saida;
    saida << std::put_time(&calendario, "%Y-%m-%d");
    return saida.str();
}
} // namespace Data
