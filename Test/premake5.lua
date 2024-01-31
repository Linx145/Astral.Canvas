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
        "{COPYFILE}	 \"TestContent/tbh.png\" \"bin/Debug/tbh.png\""
    }


    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"