# SPDX-FileCopyrightText: 2024 Friedrich W. H. Kossebau <kossebau@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

set(BOVO_INSTALL_THEMESDIR "${KDE_INSTALL_DATADIR}/bovo/themes")

function(install_theme id)
    cmake_parse_arguments(ARG "" "" "FILES" ${ARGN})
    foreach(_file ${ARG_FILES})
        get_filename_component(_dir "${_file}" DIRECTORY)
        get_filename_component(_extension "${_file}" EXT)

        if (_extension STREQUAL ".svg")
            set(svgz "${CMAKE_CURRENT_BINARY_DIR}/${_file}z")
            generate_svgz("${_file}" "${svgz}" "theme-${_dir}-")
            set(_file "${svgz}")
        endif()
        install( FILES ${_file} DESTINATION ${BOVO_INSTALL_THEMESDIR}/${_dir} )
    endforeach()
endfunction()

install_theme(scribble
    FILES
        scribble/theme.svg
        scribble/themerc
)

install_theme(highcontrast
    FILES
        highcontrast/theme.svg
        highcontrast/themerc
)

# Spacy and Gomoku doesn't have white background color
# causing trouble on theme switching. Disable them for 4.0
# and solve POST-4.0
install_theme(spacy
    FILES
        spacy/theme.svg
        spacy/themerc
)

install_theme(gomoku
    FILES
        gomoku/theme.svg
        gomoku/themerc
)
