#pragma once

#include "vector.hpp"
#include "string.hpp"
#include "shaderc/shaderc.hpp"
#include "allocators.hpp"
#include "lexer.hpp"
#include "Json.hpp"
#include "spirv_cross/spirv_cross_c.h"

enum AstralShadercParsingState
{
    AstralShaderc_ParsingNone,
    AstralShaderc_ParsingVertex,
    AstralShaderc_ParsingFragment,
    AstralShaderc_ParsingCompute
};

struct AstralShadercUniform
{
    string variableName;
    u32 set;
    u32 binding;
    usize size;
};
struct AstralShadercResource
{
    string variableName;
    u32 set;
    u32 binding;
    u32 arrayLength;
};

struct AstralShadercShaderVariables
{
    collections::Array<AstralShadercUniform> uniforms;
    collections::Array<AstralShadercResource> textures;
    collections::Array<AstralShadercResource> samplers;

    inline AstralShadercShaderVariables()
    {
        uniforms = collections::Array<AstralShadercUniform>();
        textures = collections::Array<AstralShadercResource>();
        samplers = collections::Array<AstralShadercResource>();
    }
    inline AstralShadercShaderVariables(IAllocator *allocator)
    {
        uniforms = collections::Array<AstralShadercUniform>(allocator);
        textures = collections::Array<AstralShadercResource>(allocator);
        samplers = collections::Array<AstralShadercResource>(allocator);
    }
    inline void deinit()
    {
        uniforms.deinit();
        textures.deinit();
        samplers.deinit();
    }
};

struct AstralShadercCompileResult
{
    IAllocator *allocator;
    bool isCompute;
    collections::vector<u32> shaderData1;
    AstralShadercShaderVariables shaderVariables1;
    collections::vector<u32> shaderData2;
    AstralShadercShaderVariables shaderVariables2;
    string shaderData1MSL;
    string shaderData2MSL;
    string errorMessage;

    inline AstralShadercCompileResult(IAllocator *allocator)
    {
        this->allocator = allocator;
        isCompute = false;
        shaderData1 = collections::vector<u32>(allocator);
        shaderData2 = collections::vector<u32>(allocator);
        shaderVariables1 = AstralShadercShaderVariables(allocator);
        shaderVariables2 = AstralShadercShaderVariables(allocator);
        shaderData1MSL = string();
        shaderData2MSL = string();
        errorMessage = string(allocator);
    }
    inline AstralShadercCompileResult(string error)
    {
        errorMessage = error;
        this->allocator = error.allocator;
        isCompute = false;
        shaderData1 = collections::vector<u32>(allocator);
        shaderData2 = collections::vector<u32>(allocator);
        shaderVariables1 = AstralShadercShaderVariables();
        shaderVariables2 = AstralShadercShaderVariables();
        shaderData1MSL = string();
        shaderData2MSL = string();
    }
    inline void deinit()
    {
        shaderData1.deinit();
        shaderData2.deinit();
        shaderVariables1.deinit();
        shaderVariables2.deinit();
    }
};

/// @brief Compiles the given glsl string into SPIRV binary.
/// @param allocator Allocator to create the error strings with
/// @param compiler The shaderc compiler to use
/// @param source_name the name of the source code file
/// @param kind The type of shader
/// @param source The input GLSL code
/// @param optimize Whether the SPIRV result should be optimized
/// @param result The vector to dump the integer SPIRV assembly into
/// @return A string with buffer != NULL if there is an error, else empty string if all clear
inline string AstralShaderc_CompileSpirvBinary(IAllocator *allocator, shaderc::Compiler *compiler, const char* entryPoint, shaderc_shader_kind kind, string source, bool optimize, collections::vector<u32> *result)
{
    shaderc::CompileOptions options;

    if (optimize)
    {
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
    }
    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_1);
    
    if (!compiler->IsValid())
    {
        return string(allocator, "Compiler invalid");
    }
    shaderc::SpvCompilationResult module = compiler->CompileGlslToSpv(source.buffer, kind, "", entryPoint, options);

    if (module.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        return string(allocator, module.GetErrorMessage().c_str());
    }

    usize length = module.end() - module.begin();
    for (usize i = 0; i < length; i++)
    {
        result->Add(*(module.begin() + i));
    }
    return string();
}

struct AstralCanvasShaderCompiler
{
    collections::hashmap<string, LinxcTokenID> nameToToken;
    LinxcTokenizer tokenizer;
    inline AstralCanvasShaderCompiler(IAllocator *allocator, string fileContents)
    {
        nameToToken = collections::hashmap<string, LinxcTokenID>(allocator, &stringHash, &stringEql);
        nameToToken.Add(string(nameToToken.allocator, "include"), Linxc_Keyword_include);
        nameToToken.Add(string(nameToToken.allocator, "bool"), Linxc_Keyword_bool);
        nameToToken.Add(string(nameToToken.allocator, "break"), Linxc_Keyword_break);
        nameToToken.Add(string(nameToToken.allocator, "const"), Linxc_Keyword_const);
        nameToToken.Add(string(nameToToken.allocator, "continue"), Linxc_Keyword_continue);
        nameToToken.Add(string(nameToToken.allocator, "define"), Linxc_Keyword_define);
        nameToToken.Add(string(nameToToken.allocator, "double"), Linxc_Keyword_double);
        nameToToken.Add(string(nameToToken.allocator, "else"), Linxc_Keyword_else);
        nameToToken.Add(string(nameToToken.allocator, "false"), Linxc_Keyword_false);
        nameToToken.Add(string(nameToToken.allocator, "float"), Linxc_Keyword_float);
        nameToToken.Add(string(nameToToken.allocator, "for"), Linxc_Keyword_for);
        //nameToToken.Add(string(nameToToken.allocator, "i8"), Linxc_Keyword_i8);
        //nameToToken.Add(string(nameToToken.allocator, "i16"), Linxc_Keyword_i16);
        nameToToken.Add(string(nameToToken.allocator, "int"), Linxc_Keyword_i32);
        //nameToToken.Add(string(nameToToken.allocator, "i64"), Linxc_Keyword_i64);
        nameToToken.Add(string(nameToToken.allocator, "if"), Linxc_Keyword_if);
        //nameToToken.Add(string(nameToToken.allocator, "ifdef"), Linxc_Keyword_ifdef);
        //nameToToken.Add(string(nameToToken.allocator, "ifndef"), Linxc_Keyword_ifndef);
        nameToToken.Add(string(nameToToken.allocator, "include"), Linxc_Keyword_include);
        nameToToken.Add(string(nameToToken.allocator, "return"), Linxc_Keyword_return);
        nameToToken.Add(string(nameToToken.allocator, "struct"), Linxc_Keyword_struct);
        nameToToken.Add(string(nameToToken.allocator, "true"), Linxc_Keyword_true);
        //nameToToken.Add(string(nameToToken.allocator, "u8"), Linxc_Keyword_u8);
        //nameToToken.Add(string(nameToToken.allocator, "u16"), Linxc_Keyword_u16);
        nameToToken.Add(string(nameToToken.allocator, "uint"), Linxc_Keyword_u32);
        //nameToToken.Add(string(nameToToken.allocator, "u64"), Linxc_Keyword_u64);
        nameToToken.Add(string(nameToToken.allocator, "void"), Linxc_Keyword_void);
        nameToToken.Add(string(nameToToken.allocator, "while"), Linxc_Keyword_while);

        nameToToken.Add(string(nameToToken.allocator, "vertex"), Acsl_Keyword_Vertex);
        nameToToken.Add(string(nameToToken.allocator, "fragment"), Acsl_Keyword_Fragment);
        nameToToken.Add(string(nameToToken.allocator, "compute"), Acsl_Keyword_Compute);
        nameToToken.Add(string(nameToToken.allocator, "layout"), Acsl_Keyword_Layout);
        nameToToken.Add(string(nameToToken.allocator, "version"), Acsl_Keyword_Version);

        tokenizer = LinxcTokenizer(fileContents.buffer, fileContents.length, &nameToToken);
    }
    inline void deinit()
    {
        nameToToken.deinit();
        tokenizer.deinit();
    }
};

inline bool AstralShaderc_CompileMSL(IAllocator *allocator, AstralShadercCompileResult *compileResult, spvc_context context, bool reflectData1)
{
    collections::vector<u32> *dataToReflect;
    string *resultString;
    if (reflectData1)
    {
        dataToReflect = &compileResult->shaderData1;
        resultString = &compileResult->shaderData1MSL;
    }
    else
    {
        dataToReflect = &compileResult->shaderData2;
        resultString = &compileResult->shaderData2MSL;
    }

    spvc_parsed_ir parsedIR;
    if (spvc_context_parse_spirv(context, dataToReflect->ptr, dataToReflect->count, &parsedIR) != SPVC_SUCCESS)
    {
        return false;
    }

    spvc_compiler compiler;
    if (spvc_context_create_compiler(context, SPVC_BACKEND_MSL, parsedIR, SPVC_CAPTURE_MODE_COPY, &compiler) != SPVC_SUCCESS)
    {
        return false;
    }

    spvc_compiler_options options;
    if (spvc_compiler_create_compiler_options(compiler, &options) != SPVC_SUCCESS)
    {
        return false;
    }
    if (spvc_compiler_install_compiler_options(compiler, options) != SPVC_SUCCESS)
    {
        printf("Error installing MSL compiler options\n");
        return false;
    }
    spvc_compiler_set_decoration(compiler, 0, SpvDecorationBinding, 1);

    spvc_resources resources;
    if (spvc_compiler_create_shader_resources(compiler, &resources) != SPVC_SUCCESS)
    {
        return false;
    }
    const spvc_reflected_resource *allResources;
    usize uniformCount = 0;
    spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_UNIFORM_BUFFER, &allResources, &uniformCount);
    for (usize i = 0; i < uniformCount; i++)
    {
        
        //need to increase binding by 1 for msl because binding0 is taken up by the vertex buffer/input from vertex stage
        u32 binding = spvc_compiler_get_decoration(compiler, allResources[i].id, SpvDecorationBinding);
        u32 descSet = spvc_compiler_get_decoration(compiler, allResources[i].id, SpvDecorationDescriptorSet);

        spvc_msl_resource_binding newBindings;
        spvc_msl_resource_binding_init(&newBindings);
        newBindings.binding = binding;
        newBindings.desc_set = descSet;
        newBindings.stage = reflectData1 ? SpvExecutionModelVertex : SpvExecutionModelFragment;
        newBindings.msl_buffer = binding + 1;
        spvc_compiler_msl_add_resource_binding(compiler, &newBindings);
        //printf("resource %s, binding %u\n", allResources[i].name, binding + 1);
        //spvc_compiler_unset_decoration(compiler, allResources[i].id, SpvDecorationBinding);
        //spvc_compiler_set_decoration(compiler, allResources[i].id, SpvDecorationBinding, binding + 1);
    }

    const char *result;
    if (spvc_compiler_compile(compiler, &result) != SPVC_SUCCESS)
    {
        printf("Error compiling MSL\n");
        return false;
    }
    *resultString = string(allocator, result);
    return true;
}

inline bool AstralShaderc_GenerateReflectionData(IAllocator *allocator, AstralShadercCompileResult *compileResult, spvc_context context, bool reflectData1)
{
    AstralShadercShaderVariables *shaderVariables;
    collections::vector<u32> *dataToReflect;
    if (reflectData1)
    {
        dataToReflect = &compileResult->shaderData1;
        shaderVariables = &compileResult->shaderVariables1;
    }
    else
    {
        dataToReflect = &compileResult->shaderData2;
        shaderVariables = &compileResult->shaderVariables2;
    }

    spvc_parsed_ir parsedIR;
    if (spvc_context_parse_spirv(context, dataToReflect->ptr, dataToReflect->count, &parsedIR) != SPVC_SUCCESS)
    {
        return false;
    }

    spvc_compiler compiler;
    if (spvc_context_create_compiler(context, SPVC_BACKEND_NONE, parsedIR, SPVC_CAPTURE_MODE_COPY, &compiler) != SPVC_SUCCESS)
    {
        return false;
    }

    spvc_compiler_options options;
    if (spvc_compiler_create_compiler_options(compiler, &options) != SPVC_SUCCESS)
    {
        return false;
    }
    if (spvc_compiler_install_compiler_options(compiler, options) != SPVC_SUCCESS)
    {
        return false;
    }

    spvc_resources resources;
    if (spvc_compiler_create_shader_resources(compiler, &resources) != SPVC_SUCCESS)
    {
        return false;
    }

    const spvc_reflected_resource *allResources;

    //get uniforms
    usize uniformCount = 0;
    spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_UNIFORM_BUFFER, &allResources, &uniformCount);

    shaderVariables->uniforms = collections::Array<AstralShadercUniform>(allocator, uniformCount);
    for (usize i = 0; i < uniformCount; i++)
    {
        usize structSize;
        if (spvc_compiler_get_declared_struct_size(compiler, spvc_compiler_get_type_handle(compiler, allResources[i].type_id), &structSize) != SPVC_SUCCESS)
        {
            return false;
        }
        u32 set = spvc_compiler_get_decoration(compiler, allResources[i].id, SpvDecorationDescriptorSet);
        u32 binding = spvc_compiler_get_decoration(compiler, allResources[i].id, SpvDecorationBinding);

        AstralShadercUniform uniformData;
        uniformData.binding = binding;
        uniformData.set = set;
        uniformData.variableName = string(allocator, allResources[i].name);
        uniformData.size = structSize;
        shaderVariables->uniforms.data[i] = uniformData;
    }

    //get textures
    usize resourcesCount = 0;
    spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_SEPARATE_IMAGE, &allResources, &resourcesCount);

    shaderVariables->textures = collections::Array<AstralShadercResource>(allocator, resourcesCount);
    for (usize i = 0; i < resourcesCount; i++)
    {
        AstralShadercResource resourceData;
        resourceData.binding = spvc_compiler_get_decoration(compiler, allResources[i].id, SpvDecorationBinding);
        resourceData.set = spvc_compiler_get_decoration(compiler, allResources[i].id, SpvDecorationDescriptorSet);
        resourceData.variableName = string(allocator, allResources[i].name);
        resourceData.arrayLength = spvc_type_get_array_dimension(spvc_compiler_get_type_handle(compiler, allResources[i].type_id), 0);

        shaderVariables->textures.data[i] = resourceData;
    }
    
    //get samplers
    spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_SEPARATE_SAMPLERS, &allResources, &resourcesCount);

    shaderVariables->samplers = collections::Array<AstralShadercResource>(allocator, resourcesCount);
    for (usize i = 0; i < resourcesCount; i++)
    {
        AstralShadercResource resourceData;
        resourceData.binding = spvc_compiler_get_decoration(compiler, allResources[i].id, SpvDecorationBinding);
        resourceData.set = spvc_compiler_get_decoration(compiler, allResources[i].id, SpvDecorationDescriptorSet);
        resourceData.variableName = string(allocator, allResources[i].name);
        resourceData.arrayLength = spvc_type_get_array_dimension(spvc_compiler_get_type_handle(compiler, allResources[i].type_id), 0);

        shaderVariables->samplers.data[i] = resourceData;
    }

    return true;
}

inline AstralShadercCompileResult AstralShaderc_CompileShader(IAllocator *allocator, string fileContents)
{
    IAllocator defaultAllocator = GetCAllocator();
    AstralCanvasShaderCompiler compiler = AstralCanvasShaderCompiler(allocator, fileContents);
    AstralShadercParsingState parsingState = AstralShaderc_ParsingNone;

    collections::vector<CharSlice> tokenStrings = collections::vector<CharSlice>(&defaultAllocator);
    LinxcTokenizer* tokenizer = &compiler.tokenizer;
    tokenizer->tokenStream = collections::vector<LinxcToken>(allocator);

    string vertexShaderData = string();
    string fragmentShaderData = string();
    string computeShaderData = string();

    while (true)
    {
        LinxcToken token = tokenizer->TokenizeAdvance();
        if (token.ID == Linxc_Invalid)
        {
            tokenStrings.deinit();
            compiler.deinit();
            return AstralShadercCompileResult(string(allocator, "Invalid character in file"));
        }
        tokenizer->tokenStream.Add(token);
        if (token.ID == Linxc_Eof)
        {
            break;
        }
        else if (token.ID == Linxc_Hash)
        {
            token = tokenizer->TokenizeAdvance();
            if (token.ID == Linxc_Keyword_include)
            {
                token = tokenizer->TokenizeAdvance();
                if (token.ID == Linxc_MacroString)
                {
                    continue;
                }
                else
                {
                    return AstralShadercCompileResult(string(allocator, "Expected valid file path after #include directive"));
                }
            }
            else
            {
                tokenizer->tokenStream.Add(token);
            }
        }
    }
    tokenizer->currentToken = 0;
    while (true)
    {
        LinxcToken token = tokenizer->Next();
        if (token.ID == Linxc_Hash)
        {
            LinxcToken next = tokenizer->Next();

            if (tokenStrings.count > 0 && (next.ID == Acsl_Keyword_Vertex || next.ID == Acsl_Keyword_Fragment || next.ID == Acsl_Keyword_Fragment))
            {
                if (parsingState == AstralShaderc_ParsingVertex)
                {
                    vertexShaderData = ConcatFromCharSlices(allocator, tokenStrings.ptr, tokenStrings.count);
                }
                else if (parsingState == AstralShaderc_ParsingFragment)
                {
                    fragmentShaderData = ConcatFromCharSlices(allocator, tokenStrings.ptr, tokenStrings.count);
                }
                else if (parsingState == AstralShaderc_ParsingCompute)
                {
                    computeShaderData = ConcatFromCharSlices(allocator, tokenStrings.ptr, tokenStrings.count);
                }
                tokenStrings.Clear();
            }

            //don't transpile these as they are not valid glsl
            if (next.ID == Acsl_Keyword_Vertex)
            {
                if (vertexShaderData.buffer != NULL)
                {
                    tokenStrings.deinit();
                    compiler.deinit();
                    return AstralShadercCompileResult(string(allocator, "Multiple #vertex declarations found!"));
                }
                parsingState = AstralShaderc_ParsingVertex;
                continue;
            }
            else if (next.ID == Acsl_Keyword_Fragment)
            {
                if (fragmentShaderData.buffer != NULL)
                {
                    tokenStrings.deinit();
                    compiler.deinit();
                    return AstralShadercCompileResult(string(allocator, "Multiple #fragment declarations found!"));
                }
                parsingState = AstralShaderc_ParsingFragment;
                continue;
            }
            else if (next.ID == Acsl_Keyword_Compute)
            {
                if (computeShaderData.buffer != NULL)
                {
                    tokenStrings.deinit();
                    compiler.deinit();
                    return AstralShadercCompileResult(string(allocator, "Multiple #compute declarations found!"));
                }
                parsingState = AstralShaderc_ParsingCompute;
                continue;
            }
            else
            {
                //copy over as usual
                tokenStrings.Add(token.ToCharSlice());
                tokenStrings.Add(next.ToCharSlice());
                
                //for #version directives, we need a space after the 'version' keyword
                tokenStrings.Add(CharSlice(" "));
            }
        }
        else if (token.ID == Linxc_Eof)
        {
            //stringify the remaining data
            if (tokenStrings.count > 0)
            {
                if (parsingState == AstralShaderc_ParsingVertex)
                {
                    vertexShaderData = ConcatFromCharSlices(allocator, tokenStrings.ptr, tokenStrings.count);
                }
                else if (parsingState == AstralShaderc_ParsingFragment)
                {
                    fragmentShaderData = ConcatFromCharSlices(allocator, tokenStrings.ptr, tokenStrings.count);
                }
                else if (parsingState == AstralShaderc_ParsingCompute)
                {
                    computeShaderData = ConcatFromCharSlices(allocator, tokenStrings.ptr, tokenStrings.count);
                }
                tokenStrings.Clear();
            }
            break;
        }
        else
        {
            tokenStrings.Add(token.ToCharSlice());
            LinxcToken peekNext = tokenizer->PeekNext();
            if (token.ID != Linxc_Nl && token.ID != Linxc_Period && peekNext.ID != Linxc_Period && peekNext.ID != Linxc_Comma && peekNext.ID != Linxc_RParen && peekNext.ID != Linxc_LParen && peekNext.ID != Linxc_Semicolon)
            {
                tokenStrings.Add(CharSlice(" "));
            }
        }
    }

    tokenStrings.deinit();

    shaderc::Compiler shadercCompiler = shaderc::Compiler();
    AstralShadercCompileResult result = AstralShadercCompileResult(allocator);

    if (fragmentShaderData.buffer != NULL && vertexShaderData.buffer != NULL)
    {
        //printf("%s\n___\n%s\n", vertexShaderData.buffer, fragmentShaderData.buffer);

        string vertexError = AstralShaderc_CompileSpirvBinary(allocator, &shadercCompiler, "vertex", shaderc_vertex_shader, vertexShaderData, false, &result.shaderData1);
        if (vertexError.buffer != NULL)
        {
            result.errorMessage = string(allocator);
            result.errorMessage.Append("Vertex Shader Error: ");
            result.errorMessage.AppendDeinit(vertexError);
            result.errorMessage.Append("\n");
        }

        string fragmentError = AstralShaderc_CompileSpirvBinary(allocator, &shadercCompiler, "fragment", shaderc_fragment_shader, fragmentShaderData, false, &result.shaderData2);
        if (fragmentError.buffer != NULL)
        {
            if (vertexError.buffer != NULL)
            {
                result.errorMessage.Append("\n");
            }
            else result.errorMessage = string(allocator);
            result.errorMessage.Append("Fragment Shader Error: ");
            result.errorMessage.AppendDeinit(fragmentError);
        }
    }
    else if (computeShaderData.buffer != NULL)
    {
        //compile compute shader
        
        string computeError = AstralShaderc_CompileSpirvBinary(allocator, &shadercCompiler, "main", shaderc_compute_shader, computeShaderData, false, &result.shaderData1);
        if (computeError.buffer != NULL)
        {
            result.errorMessage = string(allocator);
            result.errorMessage.Append("Compute Shader Error: ");
            result.errorMessage.AppendDeinit(computeError);
        }
        result.isCompute = true;
    }
    else
    {
        tokenStrings.deinit();
        fragmentShaderData.deinit();
        vertexShaderData.deinit();
        compiler.deinit();
        return AstralShadercCompileResult(string(allocator, "Shader file not recognised as a complete vertex-fragment or compute shader"));
    }

    compiler.deinit();

    //perform reflection
    if (result.errorMessage.buffer == NULL)
    {
        spvc_context context;
        if (spvc_context_create(&context) != SPVC_SUCCESS)
        {
            result.errorMessage = string(allocator, "Failed to initialize spirv-cross reflector. Could not generate uniform/texture/sampler data from shader");
        }
        else
        {
            if (!AstralShaderc_GenerateReflectionData(allocator, &result, context, true))
            {
                if (result.isCompute)
                {
                    result.errorMessage = string(allocator, "Failed to perform reflection on compute shader");
                }
                else 
                    result.errorMessage = string(allocator, "Failed to perform reflection on vertex shader");
            }
            if (fragmentShaderData.buffer != NULL)
            {
                if (!AstralShaderc_GenerateReflectionData(allocator, &result, context, false))
                {
                    if (result.errorMessage.buffer == NULL)
                    {
                        result.errorMessage = string(allocator, "Failed to perform reflection on fragment shader");
                    }
                    else
                        result.errorMessage.Append("\nFailed to perform reflection on fragment shader");
                }
            }

            //create MSL
            if (!AstralShaderc_CompileMSL(allocator, &result, context, true))
            {
                if (result.isCompute)
                {
                    result.errorMessage = string(allocator, "Failed to perform reflection on compute shader");
                }
                else 
                    result.errorMessage = string(allocator, "Failed to perform reflection on vertex shader");
            }
            if (fragmentShaderData.buffer != NULL)
            {
                if (!AstralShaderc_CompileMSL(allocator, &result, context, false))
                {
                    if (result.errorMessage.buffer == NULL)
                    {
                        result.errorMessage = string(allocator, "Failed to perform reflection on fragment shader");
                    }
                    else
                        result.errorMessage.Append("\nFailed to perform reflection on fragment shader");
                }
            }

            spvc_context_destroy(context);
        }
    }

    return result;
}

inline void AstralShaderc_WriteShaderData(SomnialJson::JsonWriter *writer, collections::vector<u32> spirv, AstralShadercShaderVariables *variables, string msl)
{
    if (variables->uniforms.length > 0)
    {
        if (!writer->WritePropertyName("uniforms"))
        {
            printf("Failed to write 'uniforms'\n");
        }
        writer->WriteStartArray();
        for (usize i = 0; i < variables->uniforms.length; i++)
        {
            writer->WriteStartObject();

            writer->WritePropertyName("name");
            writer->WriteString(variables->uniforms.data[i].variableName.buffer);

            writer->WritePropertyName("stride");
            writer->WriteUintValue(variables->uniforms.data[i].size);

            writer->WritePropertyName("set");
            writer->WriteUintValue(variables->uniforms.data[i].set);

            writer->WritePropertyName("binding");
            writer->WriteUintValue(variables->uniforms.data[i].binding);

            writer->WriteEndObject();
        }
        writer->WriteEndArray();
    }

    if (variables->textures.length > 0)
    {
        writer->WritePropertyName("images");
        writer->WriteStartArray();
        for (usize i = 0; i < variables->textures.length; i++)
        {
            writer->WriteStartObject();

            writer->WritePropertyName("name");
            writer->WriteString(variables->textures.data[i].variableName.buffer);

            writer->WritePropertyName("arrayLength");
            writer->WriteUintValue(variables->textures.data[i].arrayLength);

            writer->WritePropertyName("set");
            writer->WriteUintValue(variables->textures.data[i].set);

            writer->WritePropertyName("binding");
            writer->WriteUintValue(variables->textures.data[i].binding);

            writer->WriteEndObject();
        }
        writer->WriteEndArray();
    }

    if (variables->samplers.length > 0)
    {
        writer->WritePropertyName("samplers");
        writer->WriteStartArray();
        for (usize i = 0; i < variables->samplers.length; i++)
        {
            writer->WriteStartObject();

            writer->WritePropertyName("name");
            writer->WriteString(variables->samplers.data[i].variableName.buffer);

            writer->WritePropertyName("arrayLength");
            writer->WriteUintValue(variables->samplers.data[i].arrayLength);

            writer->WritePropertyName("set");
            writer->WriteUintValue(variables->samplers.data[i].set);

            writer->WritePropertyName("binding");
            writer->WriteUintValue(variables->samplers.data[i].binding);

            writer->WriteEndObject();
        }
        writer->WriteEndArray();
    }

    writer->WritePropertyName("spirv");
    writer->WriteStartArray();
    for (usize i = 0; i < spirv.count; i++)
    {
        writer->WriteUintValue(spirv.ptr[i]);
    }
    writer->WriteEndArray();

    if (msl.buffer != NULL)
    {
        writer->WritePropertyName("msl");
        writer->WriteString(msl.buffer);        
    }
}
inline bool AstralShaderc_WriteToFile(IAllocator *allocator, string filePath, AstralShadercCompileResult *compiledShader)
{
    FILE *fs = NULL;
    if (fopen_s(&fs, filePath.buffer, "w") != 0)
    {
        printf("Failed to create file\n");
        return false;
    }

    SomnialJson::JsonWriter writer = SomnialJson::JsonWriter(allocator, fs, true);

    writer.WriteStartObject();
    
    if (compiledShader->isCompute)
    {
        writer.WritePropertyName("compute");
        writer.WriteStartObject();

        AstralShaderc_WriteShaderData(&writer, compiledShader->shaderData1, &compiledShader->shaderVariables1, compiledShader->shaderData1MSL);

        writer.WriteEndObject();
    }
    else
    {
        writer.WritePropertyName("vertex");
        writer.WriteStartObject();

        AstralShaderc_WriteShaderData(&writer, compiledShader->shaderData1, &compiledShader->shaderVariables1, compiledShader->shaderData1MSL);

        writer.WriteEndObject();
        

        writer.WritePropertyName("fragment");
        writer.WriteStartObject();

        AstralShaderc_WriteShaderData(&writer, compiledShader->shaderData2, &compiledShader->shaderVariables2, compiledShader->shaderData2MSL);

        writer.WriteEndObject();
    }

    writer.WriteEndObject();

    writer.SaveAndCloseFile();
    writer.deinit();

    return true;
}