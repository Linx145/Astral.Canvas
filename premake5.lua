VULKAN_SDK = os.getenv("VULKAN_SDK")

workspace "AstralCanvas"
    configurations { "Debug", "Release" }
    platforms { "Windows", "MacOS", "Linux" }

    filter "platforms:Windows"
        defines { "WINDOWS", "GLFW_EXPOSE_NATIVE_WIN32" }
        system "windows"
        architecture "x86_64"

    filter "platforms:MacOS"
        defines { "MACOS", "POSIX", "GLFW_EXPOSE_NATIVE_COCOA" }
        system "macosx"

    filter "platforms:Linux"
        defines { "LINUX", "POSIX", "GLFW_EXPOSE_NATIVE_X11" }
        system "linux"

    include("../dependencies/glfw")

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
            "Astral.Core",
            "%{VULKAN_SDK}/Include",
            "Astral.Json"
        }
        links {"GLFW", "%{VULKAN_SDK}/Lib/vulkan-1.lib"}

        files { 
            --Canvas stuff
            "src/**.c", 
            "src/**.cpp", 
            -- "include/**.h", 
            -- "include/**.hpp",
            -- "%{VULKAN_SDK}/Include/*.h",

            "Astral.Json/Json.cpp",
            -- "Astral.Json/Json.hpp"
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

    include("Astral.Reflect")

    include("Astral.Shaderc")

    include("Test")