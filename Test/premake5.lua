project "AstralCanvasTest"
    kind "ConsoleApp"
    language "C++"
    rtti "Off"
    cppdialect "C++14"
    exceptionhandling "Off"
    staticruntime "off"
    targetdir "bin/%{cfg.buildcfg}"
    includedirs {
        "include", 
        "../dependencies/glfw/include", 
        "../Astral.Core",
        "../include",
        "../Astral.Json"
    }
    links {"Astral.Canvas"}

    files { 
        --Canvas stuff
        "Program.cpp", 
        "../Astral.Json/Json.cpp",
        "TestContent/*.shaderobj"
    }

    postbuildcommands { 
        "{COPYFILE}	 \"TestContent/Triangle.shaderobj\" \"bin/Debug/Triangle.shaderobj\"",
        "{COPYFILE}	 \"TestContent/Texture.shaderobj\" \"bin/Debug/Texture.shaderobj\"",
        "{COPYFILE}	 \"TestContent/Instancing.shaderobj\" \"bin/Debug/Instancing.shaderobj\"",
        "{COPYFILE}	 \"TestContent/BindlessTexturing.shaderobj\" \"bin/Debug/BindlessTexturing.shaderobj\"",
        "{COPYFILE}	 \"TestContent/tbh.png\" \"bin/Debug/tbh.png\"",
        "{COPYFILE}	 \"TestContent/RedSlime.png\" \"bin/Debug/RedSlime.png\"",
        "{COPYFILE}	 \"TestContent/GreenSlime.png\" \"bin/Debug/GreenSlime.png\"",
        "{COPYFILE}	 \"TestContent/BlueSlime.png\" \"bin/Debug/BlueSlime.png\""
    }

    filter "system:macosx"
        links { "GLFW", "Cocoa.framework", "IOKit.framework", "Metal.framework", "MetalKit.framework", "QuartzCore.framework" }

    filter "system:windows"
        includedirs "%{VULKAN_SDK}/Include"
        defines "ASTRALCANVAS_VULKAN"

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"