project "Astral.Canvas"
    kind "ConsoleApp"
    language "C++"
    rtti "Off"
    exceptionhandling "Off"
    staticruntime "off"
    targetdir "bin/%{cfg.buildcfg}"
    includedirs {
        "include", 
        "../dependencies/glfw/include", 
        "../Somnial.Core/include",
        "../dependencies/wgpu/include"
    }
    links {"GLFW", "../dependencies/wgpu/Windows/wgpu_native.dll.lib"}

    files { 
        --Canvas stuff
        "src/**.c", 
        "src/**.cpp", 
        "include/**.h", 
        "include/**.hpp",
    }

    filter "platforms:Windows"
		systemversion "latest"

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"