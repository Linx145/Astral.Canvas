#pragma once

#include "vector.hpp"
#include "string.hpp"
#include "shaderc/shaderc.hpp"
#include "allocators.hpp"
#include "lexer.hpp"
#include "Json.hpp"
#include "spirv_cross/spirv_cross_c.h"

#define MSL_UNIFORM_BINDING_START 8
#define OUTPUT_TOKEN(token) AstralShaderc_AddTokenString(&tokenStrings, tokenizer, token)
#define SHADER_COMPILE_ERR(errorMessage) tokenStrings.deinit(); return AstralShadercCompileResult(string(allocator, errorMessage))

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
    u32 mslBinding;
    union
    {
        u32 arrayLength;
        u32 inputAttachmentIndex;
    };
};

struct AstralShadercShaderVariables
{
    collections::Array<AstralShadercUniform> uniforms;
    collections::Array<AstralShadercResource> textures;
    collections::Array<AstralShadercResource> samplers;
    collections::Array<AstralShadercResource> inputAttachments;
    collections::Array<AstralShadercResource> computeBuffers;

    inline AstralShadercShaderVariables()
    {
        uniforms = collections::Array<AstralShadercUniform>();
        textures = collections::Array<AstralShadercResource>();
        samplers = collections::Array<AstralShadercResource>();
        inputAttachments = collections::Array<AstralShadercResource>();
        computeBuffers = collections::Array<AstralShadercResource>();
    }
    inline AstralShadercShaderVariables(IAllocator allocator)
    {
        uniforms = collections::Array<AstralShadercUniform>(allocator);
        textures = collections::Array<AstralShadercResource>(allocator);
        samplers = collections::Array<AstralShadercResource>(allocator);
        inputAttachments = collections::Array<AstralShadercResource>(allocator);
        computeBuffers = collections::Array<AstralShadercResource>(allocator);
    }
    inline void deinit()
    {
        uniforms.deinit();
        textures.deinit();
        samplers.deinit();
        inputAttachments.deinit();
        computeBuffers.deinit();
    }
};

struct AstralShadercMaterialParameter
{
    string parameterName;
    usize size;

    AstralShadercMaterialParameter()
    {
        parameterName = string();
        size = 0;
    }
    AstralShadercMaterialParameter(string name, usize size)
    {
        parameterName = name;
        this->size = size;
    }
    void deinit()
    {
        parameterName.deinit();
    }
};
struct AstralShadercMaterialData
{
    IAllocator allocator;
    string materialName;
    collections::Array<AstralShadercMaterialParameter> parameters;

    inline AstralShadercMaterialData()
    {
        this->allocator = IAllocator{};
        materialName = string();
        parameters = collections::Array<AstralShadercMaterialParameter>();
    }
    inline AstralShadercMaterialData(IAllocator allocator, string name)
    {
        this->allocator = allocator;
        materialName = name;
        parameters = collections::Array<AstralShadercMaterialParameter>();
    }
    inline void deinit()
    {
        materialName.deinit();
        for (usize i = 0; i < parameters.length; i++)
        {
            parameters.data[i].deinit();
        }
        parameters.deinit();
    }
};

struct AstralShadercCompileResult
{
    IAllocator allocator;
    bool isCompute;
    collections::vector<u32> shaderData1;
    AstralShadercShaderVariables shaderVariables1;
    collections::vector<u32> shaderData2;
    AstralShadercShaderVariables shaderVariables2;
    string shaderData1MSL;
    string shaderData2MSL;
    string errorMessage;
    collections::hashmap<string, AstralShadercMaterialData> definedMaterials;

    inline AstralShadercCompileResult(IAllocator allocator)
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
string AstralShaderc_CompileSpirvBinary(IAllocator allocator, shaderc::Compiler *compiler, const char *entryPoint, shaderc_shader_kind kind, string source, bool optimize, collections::vector<u32> *result);

struct AstralCanvasShaderCompiler
{
    collections::hashmap<string, LinxcTokenID> nameToToken;
    LinxcTokenizer tokenizer;
    inline AstralCanvasShaderCompiler(IAllocator allocator, string fileContents)
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
        nameToToken.Add(string(nameToToken.allocator, "buffer"), Acsl_Keyword_Buffer);
        nameToToken.Add(string(nameToToken.allocator, "end"), Acsl_Keyword_End);
        nameToToken.Add(string(nameToToken.allocator, "in"), Acsl_Keyword_In);
        nameToToken.Add(string(nameToToken.allocator, "out"), Acsl_Keyword_Out);
        nameToToken.Add(string(nameToToken.allocator, "location"), Acsl_Keyword_Location);
        nameToToken.Add(string(nameToToken.allocator, "material"), Acsl_Keyword_Material);

        tokenizer = LinxcTokenizer(fileContents.buffer, fileContents.length, &nameToToken);
    }
    inline void deinit()
    {
        nameToToken.deinit();
        tokenizer.deinit();
    }
};

bool AstralShaderc_CompileMSL(IAllocator allocator, AstralShadercCompileResult *compileResult, spvc_context context, bool reflectData1);

bool AstralShaderc_GenerateReflectionData(IAllocator allocator, AstralShadercCompileResult *compileResult, spvc_context context, bool reflectData1);

inline void AstralShaderc_AddTokenString(collections::vector<CharSlice> *tokenStrings, LinxcTokenizer *tokenizer, LinxcToken token)
{
    tokenStrings->Add(token.ToCharSlice());
    LinxcToken peekNext = tokenizer->PeekNext();
    if (token.ID != Linxc_Nl && token.ID != Linxc_Period && peekNext.ID != Linxc_Period && peekNext.ID != Linxc_Comma && peekNext.ID != Linxc_RParen && peekNext.ID != Linxc_LParen && peekNext.ID != Linxc_Semicolon)
    {
        tokenStrings->Add(CharSlice(" "));
    }
}

bool AstralShaderc_TokenizeAll(IAllocator allocator, LinxcTokenizer *tokenizer, string fileName, collections::vector<string> *includedDirectories);

AstralShadercCompileResult AstralShaderc_CompileShader(IAllocator allocator, string fileContents, string fileName, collections::vector<string> *includedDirectories);

void AstralShaderc_WriteShaderData(Json::JsonWriter *writer, collections::vector<u32> spirv, AstralShadercShaderVariables *variables, string msl);

bool AstralShaderc_WriteToFile(IAllocator allocator, string filePath, AstralShadercCompileResult *compiledShader);