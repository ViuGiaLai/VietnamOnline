#pragma once
#include "IService.hpp"
#include <iostream>

namespace vno::server {

class QuestService : public IService {
public:
    std::string name() const override { return "quest-service"; }
    void startup() override { std::cout << "[quest-service] 12 quests, max 3 active (FR-08)\n"; }
    void shutdown() override {}
};

} // namespace vno::server
