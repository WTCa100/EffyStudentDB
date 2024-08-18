#pragma once

#include "directoryManager/dirManager.hpp"
#include "fileManager/fManager.hpp"
#include "commandHandler/commandHandler.hpp"

#include <filesystem>
#include <memory>
#include <optional>

namespace Utilities
{

    class WsManager
    {
    private:
        std::filesystem::path workingDir_ {std::filesystem::current_path()};
        std::unique_ptr<Workspace::DirectoryManager> dManager_;
        std::unique_ptr<Workspace::FileManager>      fManager_;
        bool initializeDatabase();

    public:
        WsManager(std::string workingDir_);
        WsManager() : WsManager(std::filesystem::current_path().string()) {}
        ~WsManager() = default;


        bool createFile(std::string fileName, std::optional<std::filesystem::path> subPath = std::nullopt);
        bool creatFolder(std::string catalogName);
    };
} // namespace Utilities