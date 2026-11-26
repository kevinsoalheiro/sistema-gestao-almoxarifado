#pragma once
#include <filesystem>
#include <vector>

// Backup manual de snapshots validados. Restaurar nunca apaga o backup.
class Backup {
public:
    static std::filesystem::path criar(const std::filesystem::path& arquivoDados,
                                      const std::filesystem::path& pastaBackups);
    static std::vector<std::filesystem::path> listar(
                                  const std::filesystem::path& pastaBackups);
    static void restaurar(const std::filesystem::path& arquivoBackup,
                          const std::filesystem::path& arquivoDados);
};
