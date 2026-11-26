#pragma once
#include "EstadoSistema.hpp"
#include <string>
#include <vector>

// Gera avisos e verifica a coerencia dos dados antes de salvar ou apresentar.
class Auditoria {
public:
    static std::vector<std::string> avisos(const EstadoSistema& estado);
    static bool semInconsistencias(const EstadoSistema& estado);
    static std::string relatorio(const EstadoSistema& estado);
    static int contagemDeMateriaisBaixos(const EstadoSistema& estado);
    static int contagemDeFerramentasEmprestadas(const EstadoSistema& estado);
};
