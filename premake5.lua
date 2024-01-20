VULKAN_SDK = os.getenv("VULKAN_SDK")

project "Astral.Canvas"
    kind "ConsoleApp"
    language "C++"
    rtti "Off"
    exceptionhandling "Off"
    targetdir "bin/%{cfg.buildcfg}"
    includedirs {
        "include", 
        "../dependencies/glfw/include", 
        "../Somnial.Core/include",
        "$(VULKAN_SDK)/include"
    }
    filter "kind:not StaticLib"
        links {"GLFW", "$(VULKAN_SDK)/lib/vulkan-1.lib"}

    files { 
        --Canvas stuff
        "src/**.c", 
        "src/**.cpp", 
        "include/**.h", 
        "include/**.hpp",
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"