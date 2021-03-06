cmake_minimum_required(VERSION 3.15)
project(MyFileSystem)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    # set(BUILD_SHARED_LIBS TRUE)
    set(Vulkan_BIN "C:\\VulkanSDK\\1.2.198.1\\Bin")
    set(Vulkan_SDK "C:\\VulkanSDK\\1.2.198.1")
    set(Vulkan_LIBRARY "C:\\VulkanSDK\\1.2.198.1\\Lib\\vulkan-1.lib")
    set(Vulkan_INCLUDE_PATH "C:\\VulkanSDK\\1.2.198.1\\Include")
    # set(Vulkan_THIRD_INCLUDE_PATH "C:\\VulkanSDK\\1.2.198.1\\Third-Party\\Include")
    find_package(Vulkan REQUIRED)
    

    # find_library(ws2_32_LIBRARY_PATH ws2_32)
    # add_executable(Executive ${exec_src})
    # target_link_libraries(Executive ${ws2_32_LIBRARY_PATH})

    include_directories(C:\\VulkanSDK\\1.2.198.1\\Include)

    include_directories("glfw-3.3.6\\include")
    add_subdirectory(glfw-3.3.6)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    
    set(IMGUI_DIR D:/approot/imgui)

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(STB_INCLUDE_PATH /home/number/codes/learnvulkan/include)
    set(TINYOBJ_INCLUDE_PATH /home/number/libraries/tinyobjloader)
    set(GLFW_DIR /home/number/tools/glfw)
    set(IMGUI_DIR /home/number/approot/ImGuiApps/imgui/)
# else()
endif()

message(STATUS "Configuring on/for ${CMAKE_SYSTEM_NAME}")
# include_directories(${VULKAN_INCLUDE_PATH})

include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends ..)
add_executable(fstest MyFileSystem.cpp)

add_executable(bookmanager bookmanager.cpp ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_demo.cpp ${IMGUI_DIR}/imgui_tables.cpp ${IMGUI_DIR}/imgui_widgets.cpp)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(bookmanager
    -lglfw3
    -lvulkan
    -ldl
    -lGL
    -lrt
    -lm
    -lXrandr
    -lXi
    -lX11
    -lpthread
    -lXinerama
    -lXcursor
    )
else()
    message("windows")
    target_link_libraries(fstest glfw)
    target_link_libraries(bookmanager glfw Vulkan::Vulkan)
endif()
