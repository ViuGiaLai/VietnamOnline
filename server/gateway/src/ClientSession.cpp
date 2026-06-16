#include "ClientSession.hpp"
#include "SessionManager.hpp"

namespace vno::server {

ClientSession::ClientSession(int socket, std::shared_ptr<SessionManager> manager)
    : socket_(socket)
    , manager_(manager)
    , lastPing_(std::chrono::steady_clock::now())
{
}

ClientSession::~ClientSession() {
    // Cleanup if still in manager
    if (manager_ && !playerId_.empty()) {
        manager_->removeSession(playerId_);
    }
}

} // namespace vno::server