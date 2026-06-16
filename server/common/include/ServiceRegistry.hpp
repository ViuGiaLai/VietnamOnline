#pragma once
#include "IService.hpp"
#include <memory>
#include <vector>

namespace vno::server {

class ServiceRegistry {
public:
    void registerService(std::shared_ptr<IService> service);
    void startupAll();
    void shutdownAll();
    IService* find(const std::string& name) const;

private:
    std::vector<std::shared_ptr<IService>> services_;
};

} // namespace vno::server
