# Copyright (c) 2022 LG Electronics, Inc.

if (TARGET PkgConfig::CRIU)
    set(CRIU_FOUND 1)
    return()
endif()

find_package(PkgConfig QUIET)

pkg_check_modules(CRIU criu IMPORTED_TARGET)

if (NOT TARGET PkgConfig::CRIU)
    set(CRIU_FOUND 0)
endif()
