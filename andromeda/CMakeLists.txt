find_package(Vulkan REQUIRED)

add_library(VulkanCppModule)
add_library(Vulkan::cppm ALIAS VulkanCppModule)

target_compile_definitions(VulkanCppModule PUBLIC
    VULKAN_HPP_DISPATCH_LOADER_DYNAMIC
    VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
    VULKAN_HPP_NO_EXCEPTIONS
    VULKAN_HPP_RAII_NO_EXCEPTIONS
    VULKAN_HPP_STD_USE_EXPECTED)
target_include_directories(VulkanCppModule PRIVATE "${Vulkan_INCLUDE_DIR}")
target_compile_features(VulkanCppModule PUBLIC cxx_std_23)

target_sources(VulkanCppModule
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS "${Vulkan_INCLUDE_DIR}"
    FILES "${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm"
)
target_link_libraries(VulkanCppModule PUBLIC Vulkan::Vulkan)

set(ANDROMEDA andromeda)
add_library(${ANDROMEDA})
target_compile_features(${ANDROMEDA} PUBLIC cxx_std_23)
target_sources(${ANDROMEDA}
    PUBLIC
    FILE_SET andromeda_module TYPE CXX_MODULES
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES andromeda.cppm utility.cppm application.cppm resources.cppm window.cppm 
    resource_management/shader-resource.cppm
)
if (NOT MSVC)
target_compile_options(${ANDROMEDA} PUBLIC -fno-rtti -fno-exceptions -Werror -Wall -Wextra -Wpedantic -Wnarrowing -Wconversion)
endif()
target_sources(${ANDROMEDA} PRIVATE vulkan.cpp device.cpp swapchain.cpp)
target_link_libraries(${ANDROMEDA} PRIVATE glfw VulkanCppModule)
