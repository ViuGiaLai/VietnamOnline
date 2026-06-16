#include "ServiceRegistry.hpp"

namespace vno::server {

void ServiceRegistry::registerService(std::shared_ptr<IService> service) {
    services_.push_back(std::move(service));
}

void ServiceRegistry::startupAll() {
    for (auto& s : services_) s->startup();
}

void ServiceRegistry::shutdownAll() {
    for (auto it = services_.rbegin(); it != services_.rend(); ++it)
        (*it)->shutdown();
}

IService* ServiceRegistry::find(const std::string& name) const {
    for (const auto& s : services_)
        if (s->name() == name) return s.get();
    return nullptr;
}

} // namespace vno::server
