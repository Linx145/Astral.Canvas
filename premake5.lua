VULKAN_SDK = os.getenv("VULKAN_SDK")

workspace "AstralCanvas"
    configurations { "Debug", "Release" }

    filter "system:windows"
        defines { "WINDOWS", "GLFW_EXPOSE_NATIVE_WIN32" }
        system "windows"
        architecture "x86_64"

    filter "system:linux"
        defines { "LINUX", "POSIX", "GLFW_EXPOSE_NATIVE_X11" }
        system "linux"

    filter "system:macosx"
        defines { "MACOS", "POSIX", "GLFW_EXPOSE_NATIVE_COCOA" }
        system "macosx"

    include("dependencies/glfw")
    
    project "Astral.Canvas.Dynamic"
        kind "SharedLib"
        language "C++"
        rtti "Off"
        cppdialect "C++14"
        exceptionhandling "Off"
        staticruntime "Off"
        targetdir "bin/%{cfg.buildcfg}"
        includedirs {
            "include", 
            "dependencies/glfw/include", 
            "c-interface/include",
            "Astral.Core",
            "Astral.Json"
        }
        links {"GLFW", "Astral.Canvas"}

        files { 
            "c-interface/src/*.cpp",
            "Astral.Json/Json.cpp"
        }

        defines "EXPORT_DYNAMIC_LIBRARY"

        filter "system:windows"
            systemversion "latest"
            includedirs "%{VULKAN_SDK}/Include"
            links { "%{VULKAN_SDK}/Lib/vulkan-1.lib" }
            defines { "ASTRALCANVAS_VULKAN" }
            ignoredefaultlibraries {"libcmt", "libcmtd"}

        filter "system:macosx"
            links {"Cocoa.framework", "IOKit.framework", "Metal.framework", "MetalKit.framework", "QuartzCore.framework"}
            defines { "ASTRALCANVAS_METAL", "_GLFW_COCOA" }

        filter "system:linux"
            links { "%{VULKAN_SDK}/Lib/vulkan-1.lib" }
            includedirs "%{VULKAN_SDK}/Include"
            defines { "ASTRALCANVAS_VULKAN" }
            
        filter "configurations:Debug"
            defines { "DEBUG" }
            symbols "On"

        filter "configurations:Release"
            defines { "NDEBUG" }
            optimize "On"

    project "Astral.Canvas"
        kind "StaticLib"
        language "C++"
        rtti "Off"
        cppdialect "C++14"
        exceptionhandling "Off"
        staticruntime "Off"
        targetdir "bin/%{cfg.buildcfg}"
        includedirs {
            "include", 
            "dependencies/glfw/include", 
            "Astral.Core",
            "Astral.Json"
        }
        links {"GLFW"}

        files { 
            "src/**.c", 
            "src/**.cpp", 
            "include/**.hpp",
            "include/**.h",
            "Astral.Json/Json.cpp",
            "dependencies/include/GLFW/glfw3.h",
            "dependencies/include/GLFW/glfw3native.h",
        }

        filter "system:windows"
            systemversion "latest"
            includedirs "%{VULKAN_SDK}/Include"
            links { "%{VULKAN_SDK}/Lib/vulkan-1.lib" }
            defines { "ASTRALCANVAS_VULKAN" }

        filter "system:macosx"
            defines { "ASTRALCANVAS_METAL", "_GLFW_COCOA", "GLFW_EXPOSE_NATIVE_COCOA" }
            files "src/**.mm"

        filter "system:linux"
            links { "%{VULKAN_SDK}/Lib/vulkan-1.lib" }
            includedirs "%{VULKAN_SDK}/Include"
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

    include("c-examples/Triangle")
    include("c-examples/DepthTesting")