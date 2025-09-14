# Utils module - Logging & helpers
add_library(CryovexUtils STATIC
    Logger.cpp
    Logger.h
    FileUtils.cpp
    FileUtils.h
    NetworkUtils.cpp
    NetworkUtils.h
)

target_link_libraries(CryovexUtils
    Qt6::Core
    Qt6::Network
)

target_link_libraries(CryovexLauncher CryovexUtils)