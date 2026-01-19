cmake_minimum_required(VERSION 4.2.0)
project(DearAlarm LANGUAGES CXX)

if(WIN32)

	set(CMAKE_CXX_STANDARD 17)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)

	set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/thirdparty/imgui")

	# 1. 占坑
	add_executable(DearAlarm "")

	# 2. 对已有目标设置头文件搜索路径
	target_include_directories(DearAlarm PRIVATE
		${CMAKE_SOURCE_DIR}/thirdparty
		${IMGUI_DIR}
		${IMGUI_DIR}/backends
	)

	# 3. 收集源文件
	set(SOURCES
		main.cpp
		${IMGUI_DIR}/imgui.cpp
		${IMGUI_DIR}/imgui_draw.cpp
		${IMGUI_DIR}/imgui_tables.cpp
		${IMGUI_DIR}/imgui_widgets.cpp
		${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
		${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
	)

	if(WIN32)
		list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/resources.rc")
	endif()

	# 4. 把源文件挂到目标上
	target_sources(DearAlarm PRIVATE ${SOURCES})

	# 5. 链接库
	target_link_libraries(DearAlarm PRIVATE glfw3 opengl32 gdi32)
	
endif()