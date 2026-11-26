#pragma once
#include "EstadoSistema.hpp"
#include <filesystem>

// Gera tabelas que podem ser abertas em planilhas, sem alterar o estoque.
class ExportadorCsv {
public:
    static void materiais(const EstadoSistema& estado,
                          const std::filesystem::path& arquivo);
    static void ferramentas(const EstadoSistema& estado,
                            const std::filesystem::path& arquivo);
    static void usuarios(const EstadoSistema& estado,
                         const std::filesystem::path& arquivo);
    static void emprestimos(const EstadoSistema& estado,
                            const std::filesystem::path& arquivo);
    static void movimentacoes(const EstadoSistema& estado,
                              const std::filesystem::path& arquivo);
};
