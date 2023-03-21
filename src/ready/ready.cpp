#include "ready.hpp"

#include <userver/clients/dns/component.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

namespace kiddy_line_processor {

namespace {

class Ready final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-ready";

  Ready(const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context)
      : HttpHandlerBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override {
    auto has_baseball =
        pg_cluster_
            ->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                      "SELECT COUNT(*) FROM processor_schema.baseball")
            .AsSingleRow<int>();
    auto has_american_football =
        pg_cluster_
            ->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                      "SELECT COUNT(*) FROM processor_schema.american_football")
            .AsSingleRow<int>();
    auto has_football =
        pg_cluster_
            ->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                      "SELECT COUNT(*) FROM processor_schema.football")
            .AsSingleRow<int>();

    bool is_ready = has_baseball && has_american_football && has_football;
    if (!is_ready)
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);

    return "";
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendReady(userver::components::ComponentList& component_list) {
  component_list.Append<Ready>();
}

}  // namespace kiddy_line_processor