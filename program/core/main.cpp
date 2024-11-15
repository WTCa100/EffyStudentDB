#include <iostream>
#include <memory>

#include "../utilities/workspaceManager/wsManager.hpp"
#include "session/session.hpp"

using Utilities::WsManager;
int main(int argc, char const *argv[])
{
    std::shared_ptr<WsManager> coreManager = std::make_shared<WsManager>();
    Session coreSession(coreManager, coreManager->getLogger());
    coreSession.run();
}
