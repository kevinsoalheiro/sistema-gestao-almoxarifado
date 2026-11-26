#pragma once
#include "Usuario.hpp"
#include <string>
#include <vector>

// Gerencia os cadastros. A classe Usuario continua representando apenas uma pessoa.
class GestorUsuarios {
private:
    std::vector<Usuario> usuarios_;

public:
    void cadastrar(const Usuario& usuario);
    void cadastrar(int id, const std::string& nome, TipoUsuario tipo);
    const Usuario* buscar(int id) const;
    bool existe(int id) const;
    std::size_t quantidade() const;
    const std::vector<Usuario>& listar() const;
    void alterarPerfil(int id, TipoUsuario tipo);
    void alterarNome(int id, const std::string& nome);
    void excluir(int id);
    void exigirCadastrado(int id) const;
    void exigirPermissaoEstoque(int id) const;
    void exigirPermissaoRelatorio(int id) const;
    std::vector<Usuario> buscarPorNome(const std::string& trecho) const;
    int quantidadePorPerfil(TipoUsuario tipo) const;
};
