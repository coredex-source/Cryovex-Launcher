# Auth module - Microsoft login handling
add_library(CryovexAuth STATIC
    AuthManager.cpp
    AuthManager.h
    MicrosoftAuth.cpp
    MicrosoftAuth.h
)

target_link_libraries(CryovexAuth
    Qt6::Core
    Qt6::Network
    Qt6::WebEngineWidgets
)

target_link_libraries(CryovexLauncher CryovexAuth)