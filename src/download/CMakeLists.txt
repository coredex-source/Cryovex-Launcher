# Download module - Assets + libraries downloader
add_library(CryovexDownload STATIC
    DownloadManager.cpp
    DownloadManager.h
    DownloadTask.cpp
    DownloadTask.h
)

target_link_libraries(CryovexDownload
    Qt6::Core
    Qt6::Network
)

target_link_libraries(CryovexLauncher CryovexDownload)