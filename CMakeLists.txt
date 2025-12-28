cmake_minimum_required (VERSION 3.16 FATAL_ERROR)

# KDE Application Version, managed by release script
set(RELEASE_SERVICE_VERSION_MAJOR "26")
set(RELEASE_SERVICE_VERSION_MINOR "03")
set(RELEASE_SERVICE_VERSION_MICRO "70")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(bovo VERSION ${RELEASE_SERVICE_VERSION})

set(QT_MIN_VERSION "6.5.0")
set(KF_MIN_VERSION "6.0.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(FeatureSummary)
include(ECMInstallIcons)
include(ECMSetupVersion)
include(ECMAddAppIcon)
include(KDEGitCommitHooks)
include(KDEClangFormat)
include(ECMDeprecationSettings)

include(InternalMacros)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Concurrent
    Svg
    SvgWidgets
    Widgets
)

find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS
    CoreAddons
    Crash
    DBusAddons
    DocTools
    I18n
    WidgetsAddons
    XmlGui
    IconThemes
)

find_package(KDEGames6 6.0.0 REQUIRED)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h *.c)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

# re-enabling exceptions (turned off in KDE)
kde_enable_exceptions()

ecm_set_disabled_deprecation_versions(
    QT 6.10.0
    KF 6.21.0
    KDEGAMES 6.0
)

add_subdirectory (src)
add_subdirectory (themes)
add_subdirectory (icons)
add_subdirectory (doc)

ki18n_install(po)
kdoctools_install(po)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
