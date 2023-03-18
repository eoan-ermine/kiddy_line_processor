#include "pull_daemon.hpp"

#include <userver/yaml_config/merge_schemas.hpp>

namespace kiddy_line_processor {

userver::yaml_config::Schema PullDaemon::GetStaticConfigSchema() {
  return userver::yaml_config::MergeSchemas<
      userver::components::LoggableComponentBase>(R"(
type: object
description: >
  daemon for pulling data from lines provider to database
additionalProperties: false
properties:
    endpoint:
        type: string
        description: >
            Endpoint of lines provider
    interval_baseball:
        type: integer
        description: data fetching interval for baseball
    interval_american_football:
        type: integer
        description: data fetching interval for american football
    interval_football:
        type: integer
        description: data fetching interval for football
)");
}

void AppendPullDaemon(userver::components::ComponentList& component_list) {
  component_list.Append<PullDaemon>();
}

}  // namespace kiddy_line_processor
