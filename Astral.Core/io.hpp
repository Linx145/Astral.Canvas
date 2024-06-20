#pragma once

#include "Linxc.h"
#include "string.hpp"
#include "array.hpp"
#include <stdio.h>
#include "vector.hpp"
#include "ArenaAllocator.hpp"

#include <sys/stat.h>   // For stat().

#if WINDOWS
#include <io.h>
#include <Windows.h>
#define access _access
#endif
#if POSIX
#include <unistd.h>
#include <dirent.h>
#endif

namespace io
{
    inline string BinaryReadString(IAllocator allocator, FILE *fs)
    {
        long currentPos = ftell(fs);

        usize size = 0;
        while (true)
        {
            i32 result = fgetc(fs);
            if (result != 0 && result != -1)
            {
                size++;
            }
            else
                break;
        }
        string str = string(allocator, size);
        fseek(fs, 0, currentPos);
        fread(str.buffer, 1, size, fs);
        return str;
    }
    inline i32 BinaryReadInt32(FILE *fs)
    {
        i32 result;
        fscanf(fs, "%i", &result);
        return result;
    }
    inline i64 BinaryReadInt64(FILE *fs)
    {
        i64 result;
        fscanf(fs, "%lli", &result);
        return result;
    }
    inline u32 BinaryReadUint32(FILE *fs)
    {
        u32 result;
        fscanf(fs, "%u", &result);
        return result;
    }
    inline u64 BinaryReadUint64(FILE *fs)
    {
        u64 result;
        fscanf(fs, "%llu", &result);
        return result;
    }
    inline float BinaryReadFloat(FILE *fs)
    {
        float result;
        fscanf(fs, "%f", &result);
        return result;
    }
    inline double BinaryReadDouble(FILE *fs)
    {
        double result;
        fscanf(fs, "%d", &result);
        return result;
    }
    inline string ReadFile(IAllocator allocator, const char* path, bool isBinary)
    {
        string result = string(allocator);

        FILE *fs = fopen(path, isBinary ? "rb" : "r");
        if (fs != NULL)
        {
            usize size = 0;
            while (fgetc(fs) != EOF)
            {
                size += 1;
            }
            fseek(fs, 0, SEEK_SET);

            char* buffer = (char*)allocator.Allocate(size + 1);
            if (buffer != NULL)
            {
                fread(buffer, sizeof(char), size, fs);
                
                buffer[size] = '\0';
                result.buffer = buffer;
                result.length = size + 1;
            }

            fclose(fs);
        }
        return result;
    }

    inline bool FileExists(const char *path)
    {
        return access(path, 0) == 0;
    }

    inline bool DirectoryExists(const char* path)
    {
        if (access(path, 0) == 0) 
        {
            struct stat status;

            stat(path, &status);

            return (status.st_mode & S_IFDIR) != 0;
        }
        return false;
    }

    inline bool NewDirectory(const char* path)
    {
        if (!io::DirectoryExists(path))
        {
        #if WINDOWS
            return CreateDirectoryA(path, NULL);
        #else
            return mkdir(path, 0755) == 0;
        #endif

        }
        return false;
    }

    inline void RecursiveCreateDirectories(const char* finalDirPath)
    {
        ArenaAllocator arena = ArenaAllocator(GetCAllocator());
        IAllocator alloc = arena.AsAllocator();

        collections::Array<string> paths = SplitString(alloc, finalDirPath, '/');
        if (paths.length <= 1) //C:/ is not a valid file
        {
            return;
        }
        string currentPath = paths.data[0].Clone(alloc);

        for (usize i = 0; i < paths.length; i++)
        {
            if (i > 0)
            {
                currentPath.Append("/");
                currentPath.Append(paths.data[i].buffer);
            }
            if (!io::DirectoryExists(currentPath.buffer))
            {
                io::NewDirectory(currentPath.buffer);
            }
        }

        arena.deinit();
    }

    inline FILE* CreateDirectoriesAndFile(const char* path)
    {
        ArenaAllocator arena = ArenaAllocator(GetCAllocator());
        IAllocator alloc = arena.AsAllocator();
        collections::Array<string> paths = SplitString(alloc, path, '/');
        if (paths.length <= 1) //C:/ is not a valid file
        {
            return NULL;
        }
        FILE* file = NULL;
        string currentPath = paths.data[0].Clone(alloc);
        for (usize i = 0; i < paths.length; i++)
        {
            if (i > 0)
            {
                currentPath.Append("/");
                currentPath.Append(paths.data[i].buffer);
            }
            if (i < paths.length - 1)
            {
                if (!io::DirectoryExists(currentPath.buffer))
                {
                    io::NewDirectory(currentPath.buffer);
                }
            }
            else
            {
                //create file
                file = fopen(currentPath.buffer, "w");
                break;
            }
        }

        arena.deinit();
        return file;
    }

    inline collections::Array<string> GetFilesInDirectory(IAllocator allocator, const char *dirPath)
    {
        IAllocator defaultAllocator = GetCAllocator();

#if WINDOWS
        WIN32_FIND_DATAA findFileResult;
        char sPath[1024];
        sprintf(sPath, "%s/*.*", dirPath);

        HANDLE handle = FindFirstFileA(sPath, &findFileResult);
        if (handle == INVALID_HANDLE_VALUE)
        {
            return collections::Array<string>();
        }

        collections::vector<string> results = collections::vector<string>(defaultAllocator);
        while (true)
        {
            if (strcmp(findFileResult.cFileName, ".") != 0 && strcmp(findFileResult.cFileName, "..") != 0)
            {
                //printf("%s\n", &findFileResult.cFileName[0]);
                string replaced = ReplaceChar(defaultAllocator, &findFileResult.cFileName[0], '\\', '/');

                string fullPath = string(allocator, dirPath);
                fullPath.Append("/");
                fullPath.Append(replaced.buffer);
                if (!io::DirectoryExists(fullPath.buffer))
                {
                    results.Add(fullPath);
                }
                replaced.deinit();
            }
            if (!FindNextFileA(handle, &findFileResult))
            {
                break;
            }
        }

        FindClose(handle);

        return results.ToOwnedArrayWith(allocator);
#else
        collections::vector<string> results = collections::vector<string>(GetCAllocator());

        struct dirent *dent;
        DIR *srcdir = opendir(dirPath);
        while((dent = readdir(srcdir)) != NULL)
        {
            struct stat st;

            if(strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0)
            {
                continue;
            }
            if (fstatat(dirfd(srcdir), dent->d_name, &st, 0) < 0)
            {
                continue;
            }

            if (S_ISDIR(st.st_mode))
            {
                //dircount++
                string fullPath = string(allocator, dirPath);
                fullPath.Append("/");
                fullPath.Append(dent->d_name);
                results.Add(fullPath);
            }
        }

        return results.ToOwnedArrayWith(allocator);
#endif
    }

    inline collections::Array<string> GetFoldersInDirectory(IAllocator allocator, const char *dirPath)
    {
        IAllocator defaultAllocator = GetCAllocator();

#if WINDOWS
        WIN32_FIND_DATAA findFileResult;
        char sPath[1024];
        sprintf(sPath, "%s/*.*", dirPath);

        HANDLE handle = FindFirstFileA(sPath, &findFileResult);
        if (handle == INVALID_HANDLE_VALUE)
        {
            return collections::Array<string>();
        }

        collections::vector<string> results = collections::vector<string>(defaultAllocator);
        while (true)
        {
            if (strcmp(findFileResult.cFileName, ".") != 0 && strcmp(findFileResult.cFileName, "..") != 0)
            {
                //printf("%s\n", &findFileResult.cFileName[0]);
                string replaced = ReplaceChar(allocator, &findFileResult.cFileName[0], '\\', '/');

                string fullPath = string(defaultAllocator, dirPath);
                fullPath.Append("/");
                fullPath.Append(replaced.buffer);
                if (io::DirectoryExists(fullPath.buffer))
                {
                    results.Add(fullPath);
                }
                replaced.deinit();
            }
            if (!FindNextFileA(handle, &findFileResult))
            {
                break;
            }
        }

        FindClose(handle);

        return results.ToOwnedArrayWith(allocator);
#else
        collections::vector<string> results = collections::vector<string>(defaultAllocator);
        struct dirent *dir;
        DIR *d = opendir(dirPath);
        if (d != NULL) 
        {
            while ((dir = readdir(d)) != NULL) 
            {
                struct stat st;

                if(strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0)
                {
                    continue;
                }
                if (fstatat(dirfd(srcdir), dent->d_name, &st, 0) < 0)
                {
                    continue;
                }

                if (!S_ISDIR(st.st_mode))
                {
                    string fullPath = string(allocator, dirPath);
                    fullPath.Append("/");
                    fullPath.Append(dir->d_name);
                    results.Add(fullPath);
                }
            }
            closedir(d);
        }

        return results.ToOwnedArrayWith(allocator);
#endif
    }
}