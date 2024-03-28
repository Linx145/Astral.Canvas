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
    SetDefaultAllocator(allocator);

    ArenaAllocator arena = ArenaAllocator(&allocator);
    string str = io::ReadFile(&arena.asAllocator, argv[1]);

    if (str.buffer == NULL)
    {
        //is a directory, compile everything inside the directory instead and output
        //to argv[2]
        if (io::DirectoryExists(argv[1]) && io::DirectoryExists(argv[2]))
        {
            collections::Array<string> filesInDir = io::GetFilesInDirectory(&arena.asAllocator, argv[1]);

            for (usize i = 0; i < filesInDir.length; i++)
            {
                if (path::GetExtension(&arena.asAllocator, filesInDir.data[i]) != ".shader")
                {
                    continue;
                }
                string fullPath = string(&arena.asAllocator, argv[1]);
                fullPath.Append("/");
                fullPath.Append(filesInDir.data[i].buffer);
                str = io::ReadFile(&arena.asAllocator, fullPath.buffer);

                AstralShadercCompileResult result = AstralShaderc_CompileShader(&arena.asAllocator, str);
                if (result.errorMessage.buffer != NULL)
                {
                    fprintf(stderr, "%s: %s\n\n", filesInDir.data[i].buffer, result.errorMessage.buffer);
                    continue;
                }
                else
                {
                    usize argvLen = strlen(argv[2]);
                    string newPath = string(&arena.asAllocator, argv[2]);
                    if (argv[2][argvLen - 2] != '\\' && argv[2][argvLen - 2] != '/')
                    {
                        newPath.Append("/");
                    }
                    newPath.AppendDeinit(path::GetFileName(GetCAllocator(), filesInDir.data[i]));
                    newPath.Append("obj");
                    if (!AstralShaderc_WriteToFile(&arena.asAllocator, newPath, &result))
                    {
                        fprintf(stderr, "Failed to write shader to %s\n", newPath.buffer);
                    }
                }
            }
            printf("Finished\n");
            return 0;
        }
        fprintf(stderr, "Could not open file %s\n", argv[1]);
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