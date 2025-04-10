add_library(Data STATIC data.cpp data.hpp)

target_link_libraries(Data PUBLIC Student)
target_link_libraries(Data PUBLIC Course)
target_link_libraries(Data PUBLIC Srequest)
target_link_libraries(Data PUBLIC Subject)
target_link_libraries(Data PUBLIC School)
