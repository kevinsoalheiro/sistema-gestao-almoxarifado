#pragma once
#include "EstadoSistema.hpp"
#include <filesystem>

// Salva o estado inteiro em arquivo texto versionado, com gravacao temporaria.
class Persistencia {
public:
    static void salvar(const EstadoSistema& estado,
                       const std::filesystem::path& arquivo);
    static EstadoSistema carregar(const std::filesystem::path& arquivo);
    static bool existeArquivo(const std::filesystem::path& arquivo);
};
