add_library(DisplayHelper STATIC displayHelper.cpp displayHelper.hpp)

target_link_libraries(DisplayHelper PUBLIC 
    Logger
    SqlAdapter
    Data
)