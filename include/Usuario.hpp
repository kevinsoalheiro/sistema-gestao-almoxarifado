#pragma once
#include <string>

/** @brief Tipos de usuario do almoxarifado. */
enum class TipoUsuario { ALMOXARIFE, FUNCIONARIO, GERENTE };

/** @brief Representa uma pessoa que utiliza o sistema. */
class Usuario {
private:
    int id_;
    std::string nome_;
    TipoUsuario tipo_;

public:
    /** @brief Cria um usuario. @param id Identificador unico. @param nome Nome. @param tipo Perfil de acesso. */
    Usuario(int id, const std::string& nome, TipoUsuario tipo);

    /** @return Identificador do usuario. */
    int getId() const;

    /** @return Nome do usuario. */
    const std::string& getNome() const;

    /** @return Tipo de usuario. */
    TipoUsuario getTipo() const;

    /** @brief Verifica se o usuario pode gerenciar o estoque. @return Verdadeiro se tiver permissao. */
    bool podeGerenciarEstoque() const;
};
