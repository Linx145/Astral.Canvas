VULKAN_SDK = os.getenv("VULKAN_SDK")

project "Astral.Canvas"
    kind "StaticLib"
    language "C++"
    rtti "Off"
    cppdialect "C++14"
    exceptionhandling "Off"
    staticruntime "off"
    targetdir "bin/%{cfg.buildcfg}"
    includedirs {
        "include", 
        "../dependencies/glfw/include", 
        "../Somnial.Core/include",
        "%{VULKAN_SDK}/Include",
        -- "../dependencies/wgpu/include",
        "../Somnial.Json"
    }
    links {"GLFW", "%{VULKAN_SDK}/Lib/vulkan-1.lib"}

    files { 
        --Canvas stuff
        "src/**.c", 
        "src/**.cpp", 
        "include/**.h", 
        "include/**.hpp",
        "%{VULKAN_SDK}/Include/*.h",

        "../Somnial.Json/Json.cpp",
        "../Somnial.Json/Json.hpp"
    }

    filter "platforms:Windows"
		systemversion "latest"
        defines { "ASTRALCANVAS_VULKAN" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"