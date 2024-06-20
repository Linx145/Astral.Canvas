#include "Linxc.h"
#include "ShaderCompiler.hpp"
#include "ArenaAllocator.hpp"
#include "io.hpp"
#include "path.hpp"

i32 main(int argc, char **argv)
{
    if (argc < 3)
    {
        fprintf(stderr, "Invalid usage, Astral.Shaderc requires at least 2 arguments (input & output files/folders), provided %i instead\n", argc);
        return 0;
    }

    ArenaAllocator arena = ArenaAllocator(GetCAllocator());
    IAllocator alloc = arena.AsAllocator();
    collections::vector<string> includedDirectories = collections::vector<string>(alloc);

    for (usize i = 3; i < argc; i++)
    {
        string str = string(GetCAllocator(), argv[i]);
        if (str.StartsWith("-I"))
        {
            string dirPath;
            if (str.buffer[2] == '"' && str.buffer[str.length - 2] == '"')
            {
                dirPath = string(alloc, str.buffer + 3, str.length - 4);
            }
            else
            {
                dirPath = string(alloc, str.buffer + 2, str.length - 3);
            }
            if (io::DirectoryExists(dirPath.buffer))
            {
                includedDirectories.Add(dirPath);
            }
        }
        str.deinit();
    }
    string argv1 = string(alloc, argv[1]);
    string str = io::ReadFile(alloc, argv[1], false);

    if (str.buffer == NULL)
    {
        //is a directory, compile everything inside the directory instead and output
        //to argv[2]
        if (io::DirectoryExists(argv[1]) && io::DirectoryExists(argv[2]))
        {
            includedDirectories.Add(argv1);
            collections::Array<string> filesInDir = io::GetFilesInDirectory(alloc, argv[1]);

            for (usize i = 0; i < filesInDir.length; i++)
            {
                if (path::GetExtension(alloc, filesInDir.data[i]) != ".shader")
                {
                    continue;
                }
                //string fullPath = string(alloc, argv[1]);
                //fullPath.Append("/");
                //fullPath.Append(filesInDir.data[i].buffer);
                str = io::ReadFile(alloc, filesInDir.data[i].buffer, false);

                AstralShadercCompileResult result = AstralShaderc_CompileShader(alloc, str, filesInDir.data[i], &includedDirectories);
                if (result.errorMessage.buffer != NULL)
                {
                    fprintf(stderr, "%s: %s\n\n", filesInDir.data[i].buffer, result.errorMessage.buffer);
                    continue;
                }
                else
                {
                    usize argvLen = strlen(argv[2]);
                    string newPath = string(alloc, argv[2]);
                    if (argv[2][argvLen - 2] != '\\' && argv[2][argvLen - 2] != '/')
                    {
                        newPath.Append("/");
                    }
                    newPath.AppendDeinit(path::GetFileName(GetCAllocator(), filesInDir.data[i]));
                    newPath.Append("obj");
                    if (!AstralShaderc_WriteToFile(alloc, newPath, &result))
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

    string dirPath = path::GetDirectory(GetCAllocator(), argv1);
    includedDirectories.Add(dirPath);
    AstralShadercCompileResult result = AstralShaderc_CompileShader(alloc, str, argv1, &includedDirectories);

    dirPath.deinit();
    if (result.errorMessage.buffer != NULL)
    {
        fprintf(stderr, "%s\n", result.errorMessage.buffer);
    }
    else
    {
        //string originalPath = string(alloc, argv[i]);
        string newPath = string(alloc, argv[2]); // path::SwapExtension(alloc, originalPath, ".shaderobj");
        if (!AstralShaderc_WriteToFile(alloc, newPath, &result))
        {
            fprintf(stderr, "Failed to output shader file\n");
        }
    }

    arena.deinit();

    return 0;
}