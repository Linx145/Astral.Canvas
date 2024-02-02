#ifdef MACOS
#import "Graphics/Metal/MetalImplementations.h"
#import "Metal/Metal.h"
#import "Graphics/Metal/MetalInstanceData.h"
#import "Foundation/Foundation.h"

NSString *NSString_FromCharPtr(const char *buffer)
{
    NSString *result = [NSString stringWithUTF8String:buffer];
    return result;
}

NSString *NSString_FromString(string str)
{
    NSString *result = [NSString stringWithUTF8String:str.buffer];
    return result;
}

void AstralCanvasMetal_DestroyShaderProgram(void* vertex, void* fragment)
{
    if (vertex != NULL)
    {
        [(id<MTLFunction>)vertex release];
    }
    if (fragment != NULL)
    {
        [(id<MTLFunction>)fragment release];
    }
}
bool AstralCanvasMetal_CreateShaderProgram(string vertexSource, string fragmentSource, void** vertexOut, void** fragmentOut)
{
    IAllocator tempAllocator = GetCAllocator();
    id<MTLDevice> gpu = AstralCanvasMetal_GetCurrentGPU();
    
    NSString *vertexNSString = [NSString stringWithUTF8String:vertexSource.buffer];
    NSString *fragmentNSString = [NSString stringWithUTF8String:fragmentSource.buffer];
    
    id<MTLFunction> vertexFunction = NULL;
    id<MTLFunction> fragmentFunction = NULL;
    
    id<MTLLibrary> vertexShader = [gpu newLibraryWithSource:vertexNSString options:NULL error:NULL];
    if (vertexShader != NULL)
    {
        vertexFunction = [vertexShader newFunctionWithName:@"main"];
        [vertexShader release];
    }
    else 
    {
        [vertexNSString release];
        return false;
    }
    
    id<MTLLibrary> fragmentShader = [gpu newLibraryWithSource:fragmentNSString options:NULL error:NULL];
    if (fragmentShader != NULL)
    {
        fragmentFunction = [fragmentShader newFunctionWithName:@"main"];
        [fragmentShader release];
    }
    else
    {
        [vertexNSString release];
        [fragmentNSString release];
        return false;
    }
    
    [vertexNSString release];
    [fragmentNSString release];
    
    *vertexOut = (void*)vertexShader;
    *fragmentOut = (void*)fragmentShader;
    return true;
}
#endif
