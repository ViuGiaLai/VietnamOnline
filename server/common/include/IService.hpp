#pragma once
#include <string>

namespace vno::server {

class IService {
public:
    virtual ~IService() = default;
    virtual std::string name() const = 0;
    virtual void startup() = 0;
    virtual void shutdown() = 0;
};

} // namespace vno::server
