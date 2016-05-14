include_directories(
    ${CMAKE_SOURCE_DIR}/libGUI
    ${CMAKE_SOURCE_DIR}/libSGEntities
)

if(POLICY CMP0020)
    CMAKE_POLICY(SET CMP0020 NEW)
endif()

set(DureuApplication_SRCS
    main.cpp
    Settings.h
    Data.h
    Data.cpp
    Utilities.h
    Utilities.cpp
)

qt5_add_resources(IMG_RSC_ADDED
    Data/Actions.qrc
    Data/Logo.qrc
    Styles/Stylesheets.qrc
)

file(GLOB_RECURSE SHADER_RSC_ADDED
    Shaders/*.frag
    Shaders/*.vert
    Shaders/*.glsl
)

add_executable(${PROJECT_NAME}
    ${DureuApplication_SRCS}
    ${IMG_RSC_ADDED}
    ${SHADER_RSC_ADDED}
)

target_link_libraries( ${PROJECT_NAME}
    # dynamic
    ${OPENSCENEGRAPH_LIBRARIES}
    ${QT_LIBRARIES}

    # defined within cherish
    libGUI
    libSGEntities
    libSGControls
    libSGUtils
)

if( CMAKE_BUILD_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])" )
        set( CMAKE_INSTALL_DEBUG_LIBRARIES_ONLY TRUE )
endif()