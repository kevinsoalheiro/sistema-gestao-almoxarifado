#pragma once
#include "EstadoSistema.hpp"
#include <filesystem>
#include <string>
#include <vector>

// Importa tabelas de materiais/ferramentas geradas pelo ExportadorCsv.
// O carregamento e validado antes de modificar o estoque.
class ImportadorCsv {
public:
    static std::size_t importarMateriais(EstadoSistema& estado,
                                        const std::filesystem::path& arquivo);
    static std::size_t importarFerramentas(EstadoSistema& estado,
                                          const std::filesystem::path& arquivo);
    static std::vector<std::string> separarCampos(const std::string& linha);
};
