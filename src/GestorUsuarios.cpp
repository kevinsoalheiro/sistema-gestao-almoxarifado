#include "GestorUsuarios.hpp"
#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace {
std::string minusculo(std::string texto) {
    std::transform(texto.begin(), texto.end(), texto.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return texto;
}
}

void GestorUsuarios::cadastrar(const Usuario& usuario) {
    if (existe(usuario.getId())) {
        throw std::invalid_argument("Ja existe usuario com esse ID");
    }
    usuarios_.push_back(usuario);
}

void GestorUsuarios::cadastrar(int id, const std::string& nome, TipoUsuario tipo) {
    cadastrar(Usuario(id, nome, tipo));
}

const Usuario* GestorUsuarios::buscar(int id) const {
    for (const auto& usuario : usuarios_) {
        if (usuario.getId() == id) return &usuario;
    }
    return nullptr;
}

bool GestorUsuarios::existe(int id) const { return buscar(id) != nullptr; }

std::size_t GestorUsuarios::quantidade() const { return usuarios_.size(); }

const std::vector<Usuario>& GestorUsuarios::listar() const { return usuarios_; }

void GestorUsuarios::alterarPerfil(int id, TipoUsuario tipo) {
    for (auto& usuario : usuarios_) {
        if (usuario.getId() == id) {
            // Usuario valida o perfil no construtor.
            Usuario atualizado(id, usuario.getNome(), tipo);
            usuario = atualizado;
            return;
        }
    }
    throw std::invalid_argument("Usuario nao encontrado");
}

void GestorUsuarios::alterarNome(int id, const std::string& nome) {
    for (auto& usuario : usuarios_) {
        if (usuario.getId() == id) {
            Usuario atualizado(id, nome, usuario.getTipo());
            usuario = atualizado;
            return;
        }
    }
    throw std::invalid_argument("Usuario nao encontrado");
}

void GestorUsuarios::excluir(int id) {
    auto it = std::find_if(usuarios_.begin(), usuarios_.end(), [id](const Usuario& u) {
        return u.getId() == id;
    });
    if (it == usuarios_.end()) throw std::invalid_argument("Usuario nao encontrado");
    usuarios_.erase(it);
}

void GestorUsuarios::exigirCadastrado(int id) const {
    if (!existe(id)) throw std::invalid_argument("Usuario nao cadastrado");
}

void GestorUsuarios::exigirPermissaoEstoque(int id) const {
    const Usuario* usuario = buscar(id);
    if (!usuario) throw std::invalid_argument("Usuario nao cadastrado");
    if (!usuario->podeGerenciarEstoque()) {
        throw std::logic_error("Este perfil nao pode modificar o estoque");
    }
}

void GestorUsuarios::exigirPermissaoRelatorio(int id) const {
    const Usuario* usuario = buscar(id);
    if (!usuario) throw std::invalid_argument("Usuario nao cadastrado");
    if (usuario->getTipo() != TipoUsuario::GERENTE &&
        usuario->getTipo() != TipoUsuario::ALMOXARIFE) {
        throw std::logic_error("Este perfil nao pode emitir relatorios gerenciais");
    }
}

std::vector<Usuario> GestorUsuarios::buscarPorNome(const std::string& trecho) const {
    std::vector<Usuario> encontrados;
    const auto termo = minusculo(trecho);
    for (const auto& usuario : usuarios_) {
        if (minusculo(usuario.getNome()).find(termo) != std::string::npos) {
            encontrados.push_back(usuario);
        }
    }
    return encontrados;
}

int GestorUsuarios::quantidadePorPerfil(TipoUsuario tipo) const {
    return static_cast<int>(std::count_if(usuarios_.begin(), usuarios_.end(),
                 [tipo](const Usuario& u) { return u.getTipo() == tipo; }));
}
