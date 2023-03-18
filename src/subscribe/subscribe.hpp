#pragma once

#include <string>
#include <string_view>

#include <handlers/subscribe_client.usrv.pb.hpp>
#include <handlers/subscribe_service.usrv.pb.hpp>
#include <userver/components/component_list.hpp>
#include <userver/engine/sleep.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

namespace kiddy_line_processor {

struct SportInfo {
  handlers::api::Sport sport;
  int last_id;
};

class Subscribe final : public handlers::api::ProcessorServiceBase::Component {
 public:
  static constexpr std::string_view kName = "handler-subscribe";

  Subscribe(const userver::components::ComponentConfig& config,
            const userver::components::ComponentContext& component_context)
      : handlers::api::ProcessorServiceBase::Component(config,
                                                       component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  void SubscribeOnSportsLine(
      handlers::api::ProcessorServiceBase::SubscribeOnSportsLineCall& call);

  template <typename T>
  void RetrieveChanges(std::vector<SportInfo>& sports, T& response) {
    for (auto& sport_info : sports) {
      auto sport = sport_info.sport;
      auto& last_id = sport_info.last_id;

      using TupleType = std::tuple<int, double>;

#define RetrieveChange(enumValue, tableName, fieldName)                        \
  if (sport == enumValue) {                                                    \
    auto [id, coefficient_sum] =                                               \
        pg_cluster_                                                            \
            ->Execute(                                                         \
                userver::storages::postgres::ClusterHostType::kMaster,         \
                "SELECT COALESCE(MAX(id), -1), COALESCE(SUM(coefficient), 0) " \
                "FROM processor_schema." #tableName " WHERE id > $1",          \
                last_id)                                                       \
            .AsSingleRow<TupleType>(userver::storages::postgres::kRowTag);     \
    if (id != -1) last_id = id;                                                \
    response[#fieldName] = coefficient_sum;                                    \
  }

      RetrieveChange(
          handlers::api::Sport::BASEBALL, baseball,
          baseball) else RetrieveChange(handlers::api::AMERICAN_FOOTBALL,
                                        american_football,
                                        football) else RetrieveChange(handlers::api::
                                                                          FOOTBALL,
                                                                      football,
                                                                      soccer)
    }
  }

 private:
  userver::storages::postgres::ClusterPtr pg_cluster_;
};

void AppendSubscribe(userver::components::ComponentList& component_list);

}  // namespace kiddy_line_processor
