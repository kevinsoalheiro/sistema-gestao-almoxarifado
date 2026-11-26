#include <doctest/doctest.h>
#include "Auditoria.hpp"
#include "RelatoriosAvancados.hpp"

TEST_CASE("Relatorios fornecem estatisticas e filtros") {
    EstadoSistema e;
    e.usuarios().cadastrar(1, "Admin", TipoUsuario::ALMOXARIFE);
    e.estoque().cadastrarMaterial(Material(10, "Parafuso", 5, 5));
    e.estoque().cadastrarMaterial(Material(11, "Prego", 10, 2));
    e.estoque().cadastrarFerramenta(Ferramenta(20, "Martelo"));
    e.historico().registrarSaida(e.estoque(), e.usuarios(), 11, 4, 1,
                                 "2026-11-24", "Uso");
    RelatoriosAvancados r;
    auto s = r.estatisticas(e);
    CHECK(s.materiais == 2);
    CHECK(s.ferramentas == 1);
    CHECK(s.materiaisBaixos == 1);
    CHECK(s.unidadesEmEstoque == 11);
    CHECK(r.materiaisPorNome(e, "PAR").find("Parafuso") != std::string::npos);
    CHECK(r.ferramentasPorNome(e, "mar").find("Martelo") != std::string::npos);
    CHECK(r.rankingEmTexto(e).find("Prego") != std::string::npos);
    CHECK(r.movimentacoesNoPeriodo(e, "2026-11-24", "2026-11-25").find("SAIDA") != std::string::npos);
    CHECK(r.resumoPorUsuario(e, 1).find("Admin") != std::string::npos);
    CHECK(Auditoria::contagemDeMateriaisBaixos(e) == 1);
    CHECK(Auditoria::semInconsistencias(e));
    CHECK(Auditoria::relatorio(e).find("AVISO") != std::string::npos);
}
