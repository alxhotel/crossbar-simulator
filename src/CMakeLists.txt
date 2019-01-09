# Find the QtWidgets library
find_package(Threads)
find_package(Qt5Widgets CONFIG REQUIRED)

set(SOURCES
	# Main
	main.cpp

	# GUI: Main
	gui/MainWindow.h gui/MainWindow.cpp
	# GUI: CodeEditor
	gui/editor/CodeEditor.h gui/editor/CodeEditor.cpp
	# GUI: CrossbarGrid
	gui/crossbar-grid/CrossbarGrid.h gui/crossbar-grid/CrossbarGrid.cpp
	gui/crossbar-grid/LineTogglerCircle.h gui/crossbar-grid/LineTogglerCircle.cpp
	gui/crossbar-grid/QubitCircle.h gui/crossbar-grid/QubitCircle.cpp
	gui/crossbar-grid/TextValueChanger.h gui/crossbar-grid/TextValueChanger.cpp
	# GUI: Modals
	gui/modals/QubitInfo.h gui/modals/QubitInfo.cpp
	# GUI: Settings
	gui/modals/Settings.h gui/modals/Settings.cpp

	# Crossbar components
	crossbar/CrossbarModel.h crossbar/CrossbarModel.cpp
	crossbar/Qubit.h crossbar/Qubit.cpp
	crossbar/QubitState.h crossbar/QubitState.cpp
	crossbar/QubitPosition.h crossbar/QubitPosition.cpp
	# Crossbar: control-lines
	crossbar/control-lines/QubitLine.h crossbar/control-lines/QubitLine.cpp
	crossbar/control-lines/BarrierLine.h crossbar/control-lines/BarrierLine.cpp
	# Crossbar: operations
	crossbar/operations/Operation.h
	crossbar/operations/Shuttling.h crossbar/operations/Shuttling.cpp
	crossbar/operations/ZGate.h crossbar/operations/ZGate.cpp
	crossbar/operations/SingleGate.h crossbar/operations/SingleGate.cpp
	crossbar/operations/SqSwap.h crossbar/operations/SqSwap.cpp
	crossbar/operations/Measurement.h crossbar/operations/Measurement.cpp
	# Crossbat: Routing
	crossbar/CrossbarRouter.h crossbar/CrossbarRouter.cpp
	# Crossbar: constraint checker
	crossbar/ConstraintChecker.h crossbar/ConstraintChecker.cpp
	# Crossbar: utils
	crossbar/Subscriber.h

	# Parser
	parser/CQASMParser.h parser/CQASMParser.cpp
)

set (UIS
	gui/MainWindow.ui
	gui/modals/QubitInfo.ui
	gui/modals/Settings.ui
)

set (RESOURCES
	resources.qrc
)

# Generate code from ui files
qt5_wrap_ui(MOC_UIS ${UIS})

# Generate rules for building source files from the resources
qt5_add_resources(MOC_RESOURCES ${RESOURCES})

# Tell CMake to create the helloworld executable
add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${MOC_UIS} ${MOC_RESOURCES})

# Libraries
# Use the Widgets module from Qt 5
target_link_libraries(${PROJECT_NAME} Qt5::Widgets lexgram naxos ${CMAKE_THREAD_LIBS_INIT})
