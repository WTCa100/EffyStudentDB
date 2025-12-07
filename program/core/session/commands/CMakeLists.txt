add_subdirectory(commandHelp)
add_subdirectory(commandAdd)
add_subdirectory(commandFind)
add_subdirectory(commandAssign)
add_subdirectory(commandShow)
add_subdirectory(commandDrop)
add_subdirectory(commandOpen)
add_subdirectory(commandClose)
add_subdirectory(commandEdit)
add_subdirectory(commandRemove)
add_subdirectory(commandList)

add_library(CommandFactory STATIC CommandFactory.cpp CommandFactory.hpp)

target_link_libraries(CommandFactory PUBLIC 
                      CommandHelp
                      CommandAdd
                      CommandShow
                      CommandAssign
                      CommandDrop
                      CommandFind
                      CommandOpen
                      CommandClose
                      CommandEdit
                      CommandRemove
                      CommandList)
