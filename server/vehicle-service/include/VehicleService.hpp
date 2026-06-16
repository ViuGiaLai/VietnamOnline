#pragma once
#include "IService.hpp"
#include <iostream>

namespace vno::server {

class VehicleService : public IService {
public:
    std::string name() const override { return "vehicle-service"; }
    void startup() override { std::cout << "[vehicle-service] Rental/fuel/return (FR-09)\n"; }
    void shutdown() override {}
};

} // namespace vno::server
