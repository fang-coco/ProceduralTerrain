cmake_minimum_required(VERSION 3.16)

project(ProceduralTerrain VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS
	Quick
    Network
)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appProceduralTerrain
    main.cpp
)

qt_add_qml_module(appProceduralTerrain
    URI ProceduralTerrain
    VERSION 1.0
    QML_FILES
        Main.qml
		SOURCES
		SOURCES src/HeightGenerator.h
		SOURCES src/Heightmap.h
		SOURCES src/NoiseGenerator.h
        SOURCES manager.h manager.cpp
		SOURCES camera.h camera.cpp
		QML_FILES CameraController.qml
		SOURCES graphics.h graphics.cpp
		RESOURCES shaders/test.vert
		RESOURCES shaders/test.frag
		SOURCES Plane.h
		RESOURCES shaders/plane.frag
		RESOURCES shaders/plane.vert
		QML_FILES Setting.qml
		RESOURCES shaders/tessellation.vert
		RESOURCES shaders/tessellation.frag
		RESOURCES shaders/tessellation.te
		RESOURCES shaders/tessellation.tc
		RESOURCES shaders/cubesphere.vert
		RESOURCES shaders/cubesphere.frag
		RESOURCES shaders/cubesphere.tc
		RESOURCES shaders/cubesphere.te
        RESOURCES shaders/cubesphere.geom
        SOURCES src/downloadtiles.h
        src/downloadtiles.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appProceduralTerrain PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appProceduralTerrain
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

add_subdirectory(cubesphere/)

target_include_directories(appProceduralTerrain PRIVATE
	thrid-part/FastNoiseLite
)
target_link_libraries(appProceduralTerrain PRIVATE
	Qt6::Quick
    Qt6::Network
	cubesphere
)

include(GNUInstallDirs)
install(TARGETS appProceduralTerrain
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
