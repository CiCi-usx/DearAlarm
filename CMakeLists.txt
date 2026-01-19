cmake_minimum_required(VERSION 4.2.0)
project(DearAlarm LANGUAGES CXX)

if(WIN32)

	set(CMAKE_CXX_STANDARD 17)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)

	set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/thirdparty/imgui")

	# 1. 定义可执行文件，并包含源代码
	add_executable(DearAlarm main.cpp)

	# 2. 添加拷贝命令
	add_custom_command(TARGET DearAlarm POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
		"${CMAKE_SOURCE_DIR}/alarm.mp3"
		"$<TARGET_FILE_DIR:DearAlarm>/alarm.mp3"
		
		# 添加下面这一行来拷贝图标
		COMMAND ${CMAKE_COMMAND} -E copy
		"${CMAKE_SOURCE_DIR}/app_icon.png"
		"$<TARGET_FILE_DIR:DearAlarm>/app_icon.png"
		
		COMMENT "Copying assets (mp3 and png) to the output directory..."
	)

	# 3. 对已有目标设置头文件搜索路径
	target_include_directories(DearAlarm PRIVATE
		${CMAKE_SOURCE_DIR}/thirdparty
		${IMGUI_DIR}
		${IMGUI_DIR}/backends
	)

	# 4. 收集源文件
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

	# 5. 把源文件挂到目标上
	target_sources(DearAlarm PRIVATE ${SOURCES})

	# 6. 链接库
	target_link_libraries(DearAlarm PRIVATE glfw3 opengl32 gdi32)
	
endif()