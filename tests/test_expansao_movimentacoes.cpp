#include <doctest/doctest.h>
#include "EstadoSistema.hpp"
#include "Data.hpp"
#include <stdexcept>

TEST_CASE("Movimentacoes registram entradas e saidas com responsavel") {
    EstadoSistema estado;
    estado.usuarios().cadastrar(1, "Ana", TipoUsuario::ALMOXARIFE);
    estado.estoque().cadastrarMaterial(Material(10, "Parafuso", 10, 2));
    auto& h = estado.historico();
    h.registrarEntrada(estado.estoque(), estado.usuarios(), 10, 5, 1, "2026-11-24", "Recebido");
    h.registrarSaida(estado.estoque(), estado.usuarios(), 10, 3, 1, "2026-11-25", "Manutencao");
    CHECK(estado.estoque().buscarMaterial(10)->getQuantidade() == 12);
    CHECK(h.quantidade() == 2);
    CHECK(h.totalEntradas(10) == 5);
    CHECK(h.totalSaidas(10) == 3);
    CHECK(h.porPeriodo("2026-11-24", "2026-11-24").size() == 1);
    CHECK(h.porUsuario(1).size() == 2);
    CHECK(h.porMaterial(99).empty());
    CHECK(h.listar()[1].resumo().find("SAIDA") != std::string::npos);
}

TEST_CASE("Movimentacoes rejeitam operacoes invalidas sem alterar estoque") {
    EstadoSistema e;
    e.usuarios().cadastrar(1, "Ana", TipoUsuario::ALMOXARIFE);
    e.usuarios().cadastrar(2, "Bruno", TipoUsuario::FUNCIONARIO);
    e.estoque().cadastrarMaterial(Material(10, "Parafuso", 4, 2));
    auto& h = e.historico();
    CHECK_THROWS_AS(h.registrarSaida(e.estoque(), e.usuarios(), 10, 8, 1,
                                    "2026-11-24", ""), std::invalid_argument);
    CHECK_THROWS_AS(h.registrarEntrada(e.estoque(), e.usuarios(), 10, 1, 2,
                                      "2026-11-24", ""), std::logic_error);
    CHECK_THROWS_AS(h.registrarEntrada(e.estoque(), e.usuarios(), 10, -1, 1,
                                      "2026-11-24", ""), std::invalid_argument);
    CHECK_THROWS_AS(h.registrarEntrada(e.estoque(), e.usuarios(), 99, 1, 1,
                                      "2026-11-24", ""), std::invalid_argument);
    CHECK_THROWS_AS(h.registrarEntrada(e.estoque(), e.usuarios(), 10, 1, 1,
                                      "24/11/26", ""), std::invalid_argument);
    CHECK(e.estoque().buscarMaterial(10)->getQuantidade() == 4);
    CHECK(h.quantidade() == 0);
}

TEST_CASE("Data reconhece ano bissexto e periodo") {
    CHECK(Data::valida("2024-02-29"));
    CHECK_FALSE(Data::valida("2026-02-29"));
    CHECK(Data::anteriorOuIgual("2026-01-01", "2026-01-02"));
    CHECK(Data::dentroDoPeriodo("2026-11-24", "2026-01-01", "2026-12-31"));
    CHECK_THROWS_AS(Data::dentroDoPeriodo("2026-01-01", "2026-12-31", "2026-01-01"), std::invalid_argument);
    CHECK(Data::hoje().size() == 10);
}
