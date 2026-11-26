#include "Backup.hpp"
#include "Persistencia.hpp"
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace fs = std::filesystem;

namespace {
std::string instante() {
    const auto relogio = std::chrono::system_clock::now();
    const std::time_t agora = std::chrono::system_clock::to_time_t(relogio);
    std::tm hora{};
#ifdef _WIN32
    localtime_s(&hora, &agora);
#else
    localtime_r(&agora, &hora);
#endif
    std::ostringstream resultado;
    resultado << std::put_time(&hora, "%Y%m%d-%H%M%S");
    return resultado.str();
}

bool backupValido(const fs::path& arquivo) {
    return arquivo.extension() == ".snapshot" &&
           arquivo.filename().string().find("backup-") == 0;
}
}

fs::path Backup::criar(const fs::path& arquivoDados, const fs::path& pastaBackups) {
    if (!Persistencia::existeArquivo(arquivoDados)) {
        throw std::runtime_error("Nao ha dados salvos para fazer backup");
    }
    (void)Persistencia::carregar(arquivoDados);
    fs::create_directories(pastaBackups);
    const std::string base = "backup-" + instante();
    fs::path destino = pastaBackups / (base + ".snapshot");
    for (int i = 1; fs::exists(destino); ++i) {
        destino = pastaBackups / (base + "-" + std::to_string(i) + ".snapshot");
    }
    fs::copy_file(arquivoDados, destino, fs::copy_options::none);
    (void)Persistencia::carregar(destino);
    return destino;
}

std::vector<fs::path> Backup::listar(const fs::path& pastaBackups) {
    std::vector<fs::path> resultado;
    if (!fs::exists(pastaBackups)) return resultado;
    if (!fs::is_directory(pastaBackups)) {
        throw std::invalid_argument("Local de backups nao e uma pasta");
    }
    for (const auto& entrada : fs::directory_iterator(pastaBackups)) {
        if (entrada.is_regular_file() && backupValido(entrada.path())) {
            resultado.push_back(entrada.path());
        }
    }
    std::sort(resultado.begin(), resultado.end(), std::greater<fs::path>());
    return resultado;
}

void Backup::restaurar(const fs::path& arquivoBackup, const fs::path& arquivoDados) {
    if (!Persistencia::existeArquivo(arquivoBackup) || !backupValido(arquivoBackup)) {
        throw std::invalid_argument("Arquivo de backup invalido");
    }
    // Primeiro validar completamente e somente depois substituir o estado.
    EstadoSistema restaurado = Persistencia::carregar(arquivoBackup);
    Persistencia::salvar(restaurado, arquivoDados);
}
