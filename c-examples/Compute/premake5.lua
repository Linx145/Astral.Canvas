project "Compute"
    kind "ConsoleApp"
    language "C"
    cdialect "C11"
    staticruntime "Off"
    targetdir "bin/%{cfg.buildcfg}"
    includedirs {
        "../../c-interface/include",
        "../../Astral.Core"
    }
    links {"Astral.Canvas.Dynamic"}

    files {
        "Program.c"
    }

    postbuildcommands { 
        "{COPYFILE}	 \"Particles.shaderobj\" \"bin/%{cfg.buildcfg}/Particles.shaderobj\""
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"