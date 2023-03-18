#include "subscribe.hpp"

#include <fmt/format.h>
#include <chrono>

#include <userver/clients/dns/component.hpp>

namespace kiddy_line_processor {

void Subscribe::SubscribeOnSportsLine(
    handlers::api::ProcessorServiceBase::SubscribeOnSportsLineCall& call) {
  handlers::api::SubscribeInfo request;

  while (call.Read(request)) {
    std::vector<SportInfo> sports;
    for (int i = 0, size = request.sports_size(); i != size; ++i) {
      sports.push_back(SportInfo{request.sports(i), 0});
    }
    int interval = request.interval();

    std::cout << request.sports_size() << ' ' << request.interval() << '\n';

    while (true) {
      handlers::api::LinesInfo response;
      RetrieveChanges(sports, *response.mutable_changes());
      call.Write(response);

      userver::engine::SleepFor(std::chrono::seconds(interval));
    }
  }

  call.Finish();
}

void AppendSubscribe(userver::components::ComponentList& component_list) {
  component_list.Append<Subscribe>();
}

}  // namespace kiddy_line_processor
