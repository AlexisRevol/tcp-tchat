set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(client_qt
    src/main_qt.cpp
    src/MainWindow.cpp
    src/ChatMessageWidget.cpp
    "${CMAKE_CURRENT_SOURCE_DIR}/include/MainWindow.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/ChatMessageWidget.hpp"
    resources.qrc 
)

target_include_directories(tchat_client_lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(client_qt PRIVATE
    Qt6::Widgets
    tchat_client_lib
    tchat_common_lib
)