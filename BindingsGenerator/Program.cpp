#include "Linxc.h"
#include "allocators.hpp"
#include "ArenaAllocator.hpp"
#include "bindgen.hpp"

i32 main()
{
    IAllocator cAllocator = GetCAllocator();
    SetDefaultAllocator(cAllocator);

    ArenaAllocator arena = ArenaAllocator(GetDefaultAllocator());

    printf("Program started\n");
    LinxcParser parser = LinxcParser(&arena.asAllocator);
    parser.SetLinxcStdLocation(string(&arena.asAllocator, "C:/Users/Linus/source/repos/SomnialGametech/Astral.Canvas/Astral.Core"));

    parser.includeDirectories.Add(string(&arena.asAllocator, "C:/Users/Linus/source/repos/SomnialGametech/Astral.Canvas/include"));
    
    LinxcIncludedFile sourceFile;
    sourceFile.includeName = string(&arena.asAllocator, "TargetBindings.linxc");
    sourceFile.fullNameAndPath = string(&arena.asAllocator, "C:/Users/Linus/source/repos/SomnialGametech/Astral.Canvas/src/TargetBindings.linxc");
    parser.includedFiles.Add(sourceFile.includeName, sourceFile);

    //parser.AddAllFilesFromDirectory(string(&arena.asAllocator, "C:/Users/Linus/source/repos/Linxc/Tests"));
    if (!parser.Compile("C:/Users/Linus/source/repos/SomnialGametech/Astral.Canvas/Bindings"))
    {
        parser.PrintAllErrors();
        printf("Compilation failed!\n");
    }
    else printf("Compilation success!\n");

    parser.deinit();
    printf("Program finish, freeing remaining %i allocations\n", (i32)arena.ptrs.count);
    arena.deinit();
    getchar();

    return 0;
}