add_library(cubesphere STATIC
	cubebuilder.cpp
	cubebuilder.h
	earthellipsoid.cpp
	mapzenimagerytileprovider.cpp
	geodetic.h
	quadnode.h
	maths/qdoublematrix4x4.cpp
	maths/qdoublevector2d.cpp
	maths/qdoublevector3d.cpp
)
find_package(Qt6 REQUIRED COMPONENTS
	Gui
	Core
	Concurrent
)

target_link_libraries(cubesphere PUBLIC
	Qt6::Gui
	Qt6::Core
	Qt6::Concurrent
)

target_include_directories(cubesphere PUBLIC
	maths
)
