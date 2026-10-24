find_package(Vulkan REQUIRED)

add_library(VulkanCppModule)
add_library(Vulkan::cppm ALIAS VulkanCppModule)

target_compile_definitions(VulkanCppModule PUBLIC
    VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
    VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1)

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
    FILES andromeda.cppm
)
target_sources(${ANDROMEDA} PRIVATE vulkan.cpp window.cpp)
target_link_libraries(${ANDROMEDA} PRIVATE glfw VulkanCppModule)
