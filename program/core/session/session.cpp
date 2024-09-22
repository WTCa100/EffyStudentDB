#include "session.hpp"

Session::Session(std::shared_ptr<WsManager> wsMgr, std::shared_ptr<Logger> logger) : wsMgr_(wsMgr), logger_(logger)
{
    LOG((*logger_), "New session instanace created!");
}

Session::~Session()
{
    LOG((*logger_), "Session teardown!");
}