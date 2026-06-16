#pragma once
#include "IService.hpp"
#include <iostream>

namespace vno::server {

class PoiService : public IService {
public:
    std::string name() const override { return "poi-service"; }
    void startup() override { std::cout << "[poi-service] 24 POI Hoi An loaded (FR-07)\n"; }
    void shutdown() override {}
};

} // namespace vno::server
