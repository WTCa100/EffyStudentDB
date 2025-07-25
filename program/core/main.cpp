#include "../utilities/workspaceManager/wsManager.hpp"
#include "session/session.hpp"

#include <iostream>
#include <memory>

using Utilities::WsManager;

int main()
{
    std::shared_ptr<WsManager> coreManager = std::make_shared<WsManager>();
    Session coreSession(coreManager);
    coreSession.run();
}
