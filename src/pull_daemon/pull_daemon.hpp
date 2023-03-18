#pragma once

#include <fmt/format.h>
#include <array>
#include <chrono>
#include <iostream>

#include <userver/clients/http/client.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/components/component_list.hpp>
#include <userver/components/impl/component_base.hpp>
#include <userver/components/loggable_component_base.hpp>
#include <userver/engine/sleep.hpp>
#include <userver/formats/json.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/async.hpp>

namespace kiddy_line_processor {

#define AddTask(sport, tableName)                                              \
  tasks_.push_back(                                                            \
      userver::utils::CriticalAsync("retrieve_" #tableName, [&]() {            \
        while (true) {                                                         \
          this->RetrieveChanges(#sport, #tableName);                           \
          userver::engine::SleepFor(                                           \
              std::chrono::seconds(config["interval_" #tableName].As<int>())); \
        }                                                                      \
      }));

class PullDaemon final : public userver::components::LoggableComponentBase {
 public:
  static constexpr std::string_view kName = "pull-daemon";
  PullDaemon(const userver::components::ComponentConfig& config,
             const userver::components::ComponentContext& component_context)
      : userver::components::LoggableComponentBase(config, component_context),
        http_client_(
            component_context.FindComponent<userver::components::HttpClient>()
                .GetHttpClient()),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()),

        endpoint(config["endpoint"].As<std::string>()) {
    AddTask(baseball, baseball) AddTask(football, american_football)
        AddTask(soccer, football)
  }

  void RetrieveChanges(std::string sport, std::string tableName) {
    auto response = http_client_.CreateRequest()
                        ->get()
                        ->retry(1)
                        ->timeout(1000)
                        ->url(fmt::format("{}/{}", endpoint, sport))
                        ->perform();
    auto response_json = userver::formats::json::FromString(response->body());
    double value =
        std::stod((*response_json["lines"].begin()).As<std::string>());

    pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        fmt::format("INSERT INTO processor_schema.{} (coefficient) VALUES ($1)",
                    tableName),
        value);
  }
  static userver::yaml_config::Schema GetStaticConfigSchema();

 private:
  userver::clients::http::Client& http_client_;
  userver::storages::postgres::ClusterPtr pg_cluster_;
  std::string endpoint;
  std::vector<userver::engine::TaskWithResult<void>> tasks_;
};

void AppendPullDaemon(userver::components::ComponentList& component_list);

}  // namespace kiddy_line_processor

template <>
inline constexpr bool
    userver::components::kHasValidate<kiddy_line_processor::PullDaemon> = true;
