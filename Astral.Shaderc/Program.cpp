#include "Linxc.h"
#include "ShaderCompiler.hpp"
#include "ArenaAllocator.hpp"
#include "io.hpp"
#include "path.hpp"

i32 main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Invalid arguments: %i\n", argc);
        return 0;
    }
    IAllocator allocator = GetCAllocator();

    ArenaAllocator arena = ArenaAllocator(&allocator);
    string str = io::ReadFile(&arena.asAllocator, argv[1]);

    if (str.buffer == NULL)
    {
        fprintf(stderr, "Failed to read file\n");
        return 1;
    }

    AstralShadercCompileResult result = AstralShaderc_CompileShader(&arena.asAllocator, str);
    if (result.errorMessage.buffer != NULL)
    {
        fprintf(stderr, "%s\n", result.errorMessage.buffer);
    }
    else
    {
        //string originalPath = string(&arena.asAllocator, argv[i]);
        string newPath = string(&arena.asAllocator, argv[2]); // path::SwapExtension(&arena.asAllocator, originalPath, ".shaderobj");
        if (!AstralShaderc_WriteToFile(&arena.asAllocator, newPath, &result))
        {
            fprintf(stderr, "Failed to output shader file\n");
        }
    }

    arena.deinit();

    return 0;
}