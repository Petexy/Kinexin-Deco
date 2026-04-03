cmake_minimum_required(VERSION 3.19)
project(kinexin-deco-kwin
    DESCRIPTION "Left-side window decoration bar for KDE Plasma"
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(QT_DEFAULT_MAJOR_VERSION 6)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
# Get Qt6's actual plugin install path
include(/usr/lib/cmake/Qt6/QtInstallPaths.cmake)
find_package(KDecoration3 REQUIRED)
find_package(KF6 REQUIRED COMPONENTS CoreAddons Config ConfigWidgets KCMUtils WidgetsAddons)

add_library(kinexin-deco-kwin MODULE
    src/kinexindecoration.cpp
    src/kinexinbutton.cpp
    src/plugin.cpp
)

target_link_libraries(kinexin-deco-kwin PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    KDecoration3::KDecoration
    KF6::CoreAddons
    KF6::ConfigCore
)

# KCM configuration module
add_library(kcm_kinexindecoration MODULE
    src/kcm/configwidget.cpp
)

target_link_libraries(kcm_kinexindecoration PRIVATE
    Qt6::Core
    Qt6::DBus
    Qt6::Gui
    Qt6::Widgets
    KF6::CoreAddons
    KF6::ConfigCore
    KF6::KCMUtils
    KF6::WidgetsAddons
)

install(TARGETS kinexin-deco-kwin
    DESTINATION ${QT6_INSTALL_PLUGINS}/org.kde.kdecoration3
)

install(FILES metadata.json
    DESTINATION ${QT6_INSTALL_PLUGINS}/org.kde.kdecoration3
    RENAME kinexin-deco-kwin.json
)

install(TARGETS kcm_kinexindecoration
    DESTINATION ${QT6_INSTALL_PLUGINS}/org.kde.kdecoration3.kcm
)
