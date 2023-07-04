cmake_minimum_required(VERSION 3.0)

set(CMAKE_COLOR_MAKEFILE OFF)

# 指定工程名称
project(PodDemoLabelAdmissionController)

# 指定源文件路径，将所有源文件汇总到DIR_SRCS中
aux_source_directory(src DIR_SRCS)
aux_source_directory(pkg/zev/src DIR_SRCS)
aux_source_directory(pkg/ejson/src DIR_SRCS)
aux_source_directory(pkg/zutil DIR_SRCS)
aux_source_directory(pkg/zutil/adapter DIR_SRCS)
aux_source_directory(pkg/zutil/base DIR_SRCS)
aux_source_directory(pkg/zutil/pkg/mbedtls DIR_SRCS)
file(GLOB_RECURSE RES_SRCS pkg/ResourceType/src/*.cpp)

# 指定头文件路径
include_directories(include)
include_directories(pkg)
include_directories(pkg/ejson/include)
include_directories(pkg/zutil)
include_directories(pkg/zev/include)
include_directories(pkg/ejson/include)
include_directories(pkg/ResourceType/include)
include_directories(pkg/sonic-json)

# 检索所有头文件
file(GLOB_RECURSE HEADER_FILES RELATIVE ${CMAKE_SOURCE_DIR} *.h *.hpp *.hh)

# 配置链接库路径
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	if(MSVC) # Visual Studio
	else() # MinGW
		link_directories(${PROJECT_SOURCE_DIR}/pkg/openssl/lib)
	endif()
endif()

# 添加目标执行文件
add_executable(PodDemoLabelAdmissionController ${DIR_SRCS} ${RES_SRCS} src/Main.cpp ${HEADER_FILES})

# 根据平台独立设定
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	# 根据编译器独立设定
	if(MSVC) # Visual Studio
		# 设定链接库
        target_link_libraries(PodDemoLabelAdmissionController ws2_32)
		# 启用VS工程文件夹，并修改默认的CMakePredefinedTargets名称
		set_property(GLOBAL PROPERTY USE_FOLDERS ON)
		set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")
		# 设置默认启动工程
		set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

		# 配置编译选项
        target_compile_options(PodDemoLabelAdmissionController PRIVATE /W3)
		# 添加宏定义
		add_definitions(-D_CRT_SECURE_NO_WARNINGS -DWIN32)

		# 生成VS Filter(与文件目录一致)
		# 此处添加的所有文件必须同时在add_library/add_executable中使用，否则不会生成VS Filter
        source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${DIR_SRCS} ${RES_SRCS} ${HEADER_FILES})
	else() # MinGW等
		# 设定链接库
		# target_link_libraries(${PROJECT_NAME} ssl crypto ws2_32 z)
		# 配置编译选项
        target_compile_options(PodDemoLabelAdmissionController PRIVATE -W -Wall)
		# 添加宏定义
		add_definitions(-D_CRT_SECURE_NO_WARNINGS -DENABLE_SSL)
	endif()
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	# add_library(SSL SHARED IMPORTED)
	# set_target_properties(SSL PROPERTIES IMPORTED_LOCATION /home/zymin/Import/20210908/RPM/usr/lib64/libssl.so)
	# add_library(CRYPTO SHARED IMPORTED)
	# set_target_properties(CRYPTO PROPERTIES IMPORTED_LOCATION /home/zymin/Import/20210908/RPM/usr/lib64/libcrypto.so)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
    target_compile_options(PodDemoLabelAdmissionController PRIVATE -g $<$<COMPILE_LANGUAGE:CXX>:-Wno-placement-new> $<$<COMPILE_LANGUAGE:CXX>:-Wno-format>)
    target_link_libraries(PodDemoLabelAdmissionController pthread dl)
endif()
