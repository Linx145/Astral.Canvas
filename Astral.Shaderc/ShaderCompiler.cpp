#include "ShaderCompiler.hpp"
#include "ArenaAllocator.hpp"
#include "hashset.hpp"
#include "path.hpp"
#include "scope.hpp"

collections::hashset<string> currentlyLoadingFiles = collections::hashset<string>(GetCAllocator(), &stringHash, &stringEql);

string AstralShaderc_CompileSpirvBinary(IAllocator allocator, shaderc::Compiler *compiler, const char* entryPoint, shaderc_shader_kind kind, string source, bool optimize, collections::vector<u32> *result)
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

bool AstralShaderc_CompileMSL(IAllocator allocator, AstralShadercCompileResult *compileResult, spvc_context context, bool reflectData1)
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
        fprintf(stderr, "Error creating MSL compiler\n");
        return false;
    }

    spvc_compiler_options options;
    if (spvc_compiler_create_compiler_options(compiler, &options) != SPVC_SUCCESS)
    {
        return false;
    }
    spvc_compiler_options_set_uint(options, SPVC_COMPILER_OPTION_MSL_VERSION, SPVC_MAKE_MSL_VERSION(2, 0, 0));
    if (spvc_compiler_install_compiler_options(compiler, options) != SPVC_SUCCESS)
    {
        fprintf(stderr, "Error installing MSL compiler options\n");
        return false;
    }
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
        u32 binding = spvc_compiler_get_decoration(compiler, allResources[i].id, SpvDecorationBinding);
        u32 descSet = spvc_compiler_get_decoration(compiler, allResources[i].id, SpvDecorationDescriptorSet);

        spvc_msl_resource_binding newBindings;
        spvc_msl_resource_binding_init(&newBindings);
        newBindings.binding = binding;
        newBindings.desc_set = descSet;
        newBindings.stage = reflectData1 ? SpvExecutionModelVertex : SpvExecutionModelFragment;
        newBindings.msl_buffer = binding + MSL_UNIFORM_BINDING_START;
        spvc_compiler_msl_add_resource_binding(compiler, &newBindings);
    }

    usize mslBinding = 0;
    //textures
    spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_SEPARATE_IMAGE, &allResources, &uniformCount);
    for (usize i = 0; i < uniformCount; i++)
    {
        u32 binding = spvc_compiler_get_decoration(compiler, allResources[i].id, SpvDecorationBinding);
        u32 descSet = spvc_compiler_get_decoration(compiler, allResources[i].id, SpvDecorationDescriptorSet);
        
        spvc_msl_resource_binding newBindings;
        spvc_msl_resource_binding_init(&newBindings);
        newBindings.binding = binding;
        newBindings.desc_set = descSet;
        newBindings.stage = reflectData1 ? SpvExecutionModelVertex : SpvExecutionModelFragment;
        newBindings.msl_texture = mslBinding;
        spvc_compiler_msl_add_resource_binding(compiler, &newBindings);

        spvc_type type = spvc_compiler_get_type_handle(compiler, allResources[i].type_id);
        u32 arrayDimensions = spvc_type_get_num_array_dimensions(type);
        u32 arrayLength;
        if (arrayDimensions > 0)
        {
            arrayLength = spvc_type_get_array_dimension(type, 0);
        }
        else arrayLength = 1;
        mslBinding += arrayLength;
    }

    //subpasses
    spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_SUBPASS_INPUT, &allResources, &uniformCount);
    for (usize i = 0; i < uniformCount; i++)
    {
        u32 binding = spvc_compiler_get_decoration(compiler, allResources[i].id, SpvDecorationBinding);
        u32 descSet = spvc_compiler_get_decoration(compiler, allResources[i].id, SpvDecorationDescriptorSet);
        
        spvc_msl_resource_binding newBindings;
        spvc_msl_resource_binding_init(&newBindings);
        newBindings.binding = binding;
        newBindings.desc_set = descSet;
        newBindings.stage = reflectData1 ? SpvExecutionModelVertex : SpvExecutionModelFragment;
        newBindings.msl_texture = mslBinding;
        spvc_compiler_msl_add_resource_binding(compiler, &newBindings);

        mslBinding += 1;
    }

    mslBinding = 0;

    //sampler
    spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_SEPARATE_SAMPLERS, &allResources, &uniformCount);
    for (usize i = 0; i < uniformCount; i++)
    {
        u32 binding = spvc_compiler_get_decoration(compiler, allResources[i].id, SpvDecorationBinding);
        u32 descSet = spvc_compiler_get_decoration(compiler, allResources[i].id, SpvDecorationDescriptorSet);
        
        spvc_msl_resource_binding newBindings;
        spvc_msl_resource_binding_init(&newBindings);
        newBindings.binding = binding;
        newBindings.desc_set = descSet;
        newBindings.stage = reflectData1 ? SpvExecutionModelVertex : SpvExecutionModelFragment;
        newBindings.msl_sampler = mslBinding;
        spvc_compiler_msl_add_resource_binding(compiler, &newBindings);

        spvc_type type = spvc_compiler_get_type_handle(compiler, allResources[i].type_id);
        u32 arrayDimensions = spvc_type_get_num_array_dimensions(type);
        u32 arrayLength;
        if (arrayDimensions > 0)
        {
            arrayLength = spvc_type_get_array_dimension(type, 0);
        }
        else arrayLength = 1;
        mslBinding += arrayLength;
    }

    const char *result;
    
    if (spvc_compiler_compile(compiler, &result) != SPVC_SUCCESS)
    {
        fprintf(stderr, "Error compiling MSL: %s\n", spvc_context_get_last_error_string(context));
        return false;
    }
    *resultString = string(allocator, result);
    return true;
}

bool AstralShaderc_GenerateReflectionData(IAllocator allocator, AstralShadercCompileResult *compileResult, spvc_context context, bool reflectData1)
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
        fprintf(stderr, "Failed to parse spirv\n");
        return false;
    }

    spvc_compiler compiler;
    if (spvc_context_create_compiler(context, SPVC_BACKEND_NONE, parsedIR, SPVC_CAPTURE_MODE_COPY, &compiler) != SPVC_SUCCESS)
    {
        fprintf(stderr, "Failed to create compiler\n");
        return false;
    }

    spvc_compiler_options options;
    if (spvc_compiler_create_compiler_options(compiler, &options) != SPVC_SUCCESS)
    {
        fprintf(stderr, "Failed to create compiler options\n");
        return false;
    }
    if (spvc_compiler_install_compiler_options(compiler, options) != SPVC_SUCCESS)
    {
        fprintf(stderr, "Failed to install compiler options\n");
        return false;
    }

    spvc_resources resources;
    if (spvc_compiler_create_shader_resources(compiler, &resources) != SPVC_SUCCESS)
    {
        fprintf(stderr, "Failed to create shader resources\n");
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
            fprintf(stderr, "Failed to find struct size\n");
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
    usize mslBinding = 0;
    spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_SEPARATE_IMAGE, &allResources, &resourcesCount);

    shaderVariables->textures = collections::Array<AstralShadercResource>(allocator, resourcesCount);
    for (usize i = 0; i < resourcesCount; i++)
    {
        AstralShadercResource resourceData{};
        resourceData.binding = spvc_compiler_get_decoration(compiler, allResources[i].id, SpvDecorationBinding);
        resourceData.set = spvc_compiler_get_decoration(compiler, allResources[i].id, SpvDecorationDescriptorSet);
        resourceData.variableName = string(allocator, allResources[i].name);
        resourceData.mslBinding = mslBinding;

        spvc_type type = spvc_compiler_get_type_handle(compiler, allResources[i].type_id);
        u32 arrayDimensions = spvc_type_get_num_array_dimensions(type);
        if (arrayDimensions > 0)
        {
            resourceData.arrayLength = spvc_type_get_array_dimension(type, 0);
        }
        else resourceData.arrayLength = 0;

        shaderVariables->textures.data[i] = resourceData;
        mslBinding += (i32)fmax(resourceData.arrayLength, 1);
    }

    //get input attachments
    spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_SUBPASS_INPUT, &allResources, &resourcesCount);

    //msl bindings dont reset as for metal, both input attachments and images count as textures
    shaderVariables->inputAttachments = collections::Array<AstralShadercResource>(allocator, resourcesCount);
    for (usize i = 0; i < resourcesCount; i++)
    {
        AstralShadercResource resourceData{};
        resourceData.binding = spvc_compiler_get_decoration(compiler, allResources[i].id, SpvDecorationBinding);
        resourceData.set = spvc_compiler_get_decoration(compiler, allResources[i].id, SpvDecorationDescriptorSet);
        resourceData.inputAttachmentIndex = spvc_compiler_get_decoration(compiler, allResources[i].id, SpvDecorationInputAttachmentIndex);
        resourceData.variableName = string(allocator, allResources[i].name);
        resourceData.mslBinding = mslBinding;

        shaderVariables->inputAttachments.data[i] = resourceData;
        mslBinding += 1;
    }
    
    //get samplers
    spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_SEPARATE_SAMPLERS, &allResources, &resourcesCount);

    shaderVariables->samplers = collections::Array<AstralShadercResource>(allocator, resourcesCount);
    mslBinding = 0;
    for (usize i = 0; i < resourcesCount; i++)
    {
        AstralShadercResource resourceData{};
        resourceData.binding = spvc_compiler_get_decoration(compiler, allResources[i].id, SpvDecorationBinding);
        resourceData.set = spvc_compiler_get_decoration(compiler, allResources[i].id, SpvDecorationDescriptorSet);
        resourceData.variableName = string(allocator, allResources[i].name);
        resourceData.mslBinding = mslBinding;

        spvc_type type = spvc_compiler_get_type_handle(compiler, allResources[i].type_id);
        if (spvc_type_get_num_array_dimensions(type) > 0)
        {
            resourceData.arrayLength = spvc_type_get_array_dimension(type, 0);
        }
        else resourceData.arrayLength = 0;

        shaderVariables->samplers.data[i] = resourceData;
        mslBinding += (i32)fmax(resourceData.arrayLength, 1);
    }

    spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_STORAGE_BUFFER, &allResources, &resourcesCount);

    //get SSBOs
    shaderVariables->computeBuffers = collections::Array<AstralShadercResource>(allocator, resourcesCount);
    mslBinding = 0;
    for (usize i = 0; i < resourcesCount; i++)
    {
        AstralShadercResource resourceData{};
        resourceData.binding = spvc_compiler_get_decoration(compiler, allResources[i].id, SpvDecorationBinding);
        resourceData.set = spvc_compiler_get_decoration(compiler, allResources[i].id, SpvDecorationDescriptorSet);
        resourceData.variableName = string(allocator, allResources[i].name);
        resourceData.mslBinding = mslBinding;

        shaderVariables->computeBuffers.data[i] = resourceData;
        mslBinding += 1;

        //should get material type
        spvc_type ssboType = spvc_compiler_get_type_handle(compiler, allResources[i].base_type_id);
        //u32 numMemberTypes = spvc_type_get_num_member_types(ssboType);
        //for (u32 j = 0; j < numMemberTypes; j++)
        //{
        spvc_type_id memberTypeID = spvc_type_get_member_type(ssboType, 0);
        spvc_type memberType = spvc_compiler_get_type_handle(compiler, memberTypeID);

        spvc_type_id materialTypeID = spvc_type_get_base_type_id(memberType);
        spvc_type materialType = spvc_compiler_get_type_handle(compiler, materialTypeID);
        string materialName = string(GetCAllocator(), spvc_compiler_get_name(compiler, materialTypeID));
        AstralShadercMaterialData *materialData = compileResult->definedMaterials.Get(materialName);
        if (materialData != NULL && materialData->parameters.data == NULL)
        {
            u32 materialParamsCount = spvc_type_get_num_member_types(memberType);
            materialData->parameters = collections::Array<AstralShadercMaterialParameter>(allocator, materialParamsCount);

            for (usize j = 0; j < materialParamsCount; j++)
            {
                string paramName = string(allocator, spvc_compiler_get_member_name(compiler, materialTypeID, j));

                materialData->parameters.data[j].parameterName = paramName;
                if (spvc_compiler_get_declared_struct_member_size(compiler, materialType, j, &materialData->parameters.data[j].size) != SPVC_SUCCESS)
                {
                    materialData->parameters.data[j].size = 0;
                    fprintf(stderr, "Failed to find struct size of material member variable %s\n", materialData->parameters.data[j].parameterName.buffer);
                }
            }
        }
        materialName.deinit();
    }

    return true;
}

bool AstralShaderc_TokenizeAll(IAllocator allocator, LinxcTokenizer *tokenizer, string fileName, collections::vector<string> *includedDirectories)
{
    currentlyLoadingFiles.Add(fileName);
    while (true)
    {
        LinxcToken token = tokenizer->TokenizeAdvance();
        if (token.ID == Linxc_Invalid)
        {
            currentlyLoadingFiles.Remove(fileName);
            return false;
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
                tokenizer->tokenStream.RemoveAt_Swap(tokenizer->tokenStream.count - 1);
                token = tokenizer->TokenizeAdvance();
                if (token.ID == Linxc_MacroString)
                {
                    for (usize i = 0; i < includedDirectories->count; i++)
                    {
                        string incFileFullName = string(GetCAllocator(), token.tokenizer->buffer + token.start + 1, token.end - token.start - 2); // token.ToString(GetCAllocator());
                        Scope(string, incFileFullName);
                        incFileFullName.Prepend("/");
                        incFileFullName.Prepend(includedDirectories->ptr[i].buffer);

                        if (io::FileExists(incFileFullName.buffer) && !currentlyLoadingFiles.Contains(incFileFullName))
                        {
                            string fileContents = io::ReadFile(allocator, incFileFullName.buffer, false);

                            LinxcTokenizer *includedFileTokenizer = (LinxcTokenizer *)allocator.Allocate(sizeof(LinxcTokenizer)); // LinxcTokenizer(fileContents.buffer, fileContents.length, tokenizer->nameToToken);
                            *includedFileTokenizer = LinxcTokenizer(fileContents.buffer, fileContents.length, tokenizer->nameToToken);
                            includedFileTokenizer->tokenStream = collections::vector<LinxcToken>(GetCAllocator());
                            
                            if (AstralShaderc_TokenizeAll(allocator, includedFileTokenizer, incFileFullName, includedDirectories))
                            {
                                for (usize c = 0; c < includedFileTokenizer->tokenStream.count - 1; c++)
                                {
                                    LinxcToken includedToken = includedFileTokenizer->tokenStream.ptr[c];
                                    //printf("%s", includedToken.ToString(allocator).buffer);
                                    tokenizer->tokenStream.Add(includedToken);
                                }
                            }

                            includedFileTokenizer->tokenStream.deinit();


                            incFileFullName.deinit();
                            break;
                        }
                        incFileFullName.deinit();
                    }

                    continue;
                }
                else
                {
                    currentlyLoadingFiles.Remove(fileName);
                    return false;
                }
            }
            else
            {
                tokenizer->tokenStream.Add(token);
            }
        }
    }
    currentlyLoadingFiles.Remove(fileName);
    return true;
}

AstralShadercCompileResult AstralShaderc_CompileShader(IAllocator allocator, string fileContents, string fileName, collections::vector<string> *includedDirectories)
{
    IAllocator defaultAllocator = GetCAllocator();
    AstralCanvasShaderCompiler compiler = AstralCanvasShaderCompiler(allocator, fileContents);
    AstralShadercParsingState parsingState = AstralShaderc_ParsingNone;
    collections::hashmap<string, AstralShadercMaterialData> definedMaterials = collections::hashmap<string, AstralShadercMaterialData>(allocator, &stringHash, &stringEql);

    i32 vertexBufferInputIndex = -1;

    collections::vector<CharSlice> tokenStrings = collections::vector<CharSlice>(GetCAllocator());
    LinxcTokenizer* tokenizer = &compiler.tokenizer;
    tokenizer->tokenStream = collections::vector<LinxcToken>(allocator);

    string vertexShaderData = string();
    string fragmentShaderData = string();
    string computeShaderData = string();

    AstralShaderc_TokenizeAll(allocator, tokenizer, fileName, includedDirectories);
    tokenizer->currentToken = 0;
    while (true)
    {
        LinxcToken token = tokenizer->Next();
        if (token.ID == Linxc_Hash)
        {
            LinxcToken next = tokenizer->Next();

            if (tokenStrings.count > 0 && (next.ID == Acsl_Keyword_Vertex || next.ID == Acsl_Keyword_Fragment))
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
                    SHADER_COMPILE_ERR("Multiple #vertex declarations found!");
                }
                parsingState = AstralShaderc_ParsingVertex;
                continue;
            }
            else if (next.ID == Acsl_Keyword_Fragment)
            {
                if (fragmentShaderData.buffer != NULL)
                {
                    SHADER_COMPILE_ERR("Multiple #fragment declarations found!");
                }
                parsingState = AstralShaderc_ParsingFragment;
                continue;
            }
            else if (next.ID == Acsl_Keyword_Compute)
            {
                if (computeShaderData.buffer != NULL)
                {
                    SHADER_COMPILE_ERR("Multiple #compute declarations found!");
                }
                parsingState = AstralShaderc_ParsingCompute;
                continue;
            }
            else if (next.ID == Acsl_Keyword_End)
            {
                if (vertexBufferInputIndex > -1)
                {
                    vertexBufferInputIndex = -1;
                    continue;
                }
                else
                {
                    SHADER_COMPILE_ERR("#end declaration without any form of begin");
                }
            }
            else if (next.ID == Acsl_Keyword_Material)
            {
                //next identifier is a material
                next = tokenizer->Next();
                if (next.ID != Linxc_Identifier)
                {
                    SHADER_COMPILE_ERR("Expected material name after #material declaration");
                }
                //register the thing first
                //reflection of material data params is gotten later
                string materialName = next.ToString(allocator);
                if (definedMaterials.Contains(materialName))
                {
                    materialName.deinit();
                    SHADER_COMPILE_ERR("Declaring the same material twice");
                }
                definedMaterials.Add(materialName, AstralShadercMaterialData(allocator, materialName));
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
            OUTPUT_TOKEN(token);
        }
    }

    tokenStrings.deinit();

    shaderc::Compiler shadercCompiler = shaderc::Compiler();
    AstralShadercCompileResult result = AstralShadercCompileResult(allocator);
    result.definedMaterials = definedMaterials;

    if (fragmentShaderData.buffer != NULL && vertexShaderData.buffer != NULL)
    {
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
        fragmentShaderData.deinit();
        vertexShaderData.deinit();
        compiler.deinit();
        SHADER_COMPILE_ERR("Shader file not recognised as a complete vertex-fragment or compute shader");
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
                        result.errorMessage = string(allocator, "Failed to compile MSL");
                    }
                    else
                        result.errorMessage.Append("\nFailed to compile MSL");
                }
            }

            spvc_context_destroy(context);
        }
    }

    return result;
}

void AstralShaderc_WriteShaderData(Json::JsonWriter *writer, collections::vector<u32> spirv, AstralShadercShaderVariables *variables, string msl)
{
    if (variables->uniforms.length > 0)
    {
        if (!writer->WritePropertyName("uniforms"))
        {
            fprintf(stderr, "Failed to write 'uniforms'\n");
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

            writer->WritePropertyName("mslBinding");
            writer->WriteUintValue(variables->textures.data[i].mslBinding);

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

            writer->WritePropertyName("mslBinding");
            writer->WriteUintValue(variables->samplers.data[i].mslBinding);

            writer->WriteEndObject();
        }
        writer->WriteEndArray();
    }

    if (variables->inputAttachments.length > 0)
    {
        writer->WritePropertyName("inputAttachments");
        writer->WriteStartArray();
        for (usize i = 0; i < variables->inputAttachments.length; i++)
        {
            writer->WriteStartObject();

            writer->WritePropertyName("name");
            writer->WriteString(variables->inputAttachments.data[i].variableName.buffer);

            writer->WritePropertyName("index");
            writer->WriteUintValue(variables->inputAttachments.data[i].inputAttachmentIndex);

            writer->WritePropertyName("set");
            writer->WriteUintValue(variables->inputAttachments.data[i].set);

            writer->WritePropertyName("binding");
            writer->WriteUintValue(variables->inputAttachments.data[i].binding);

            writer->WritePropertyName("mslBinding");
            writer->WriteUintValue(variables->inputAttachments.data[i].mslBinding);

            writer->WriteEndObject();
        }
        writer->WriteEndArray();
    }

    if (variables->computeBuffers.length > 0)
    {
        writer->WritePropertyName("storageBuffers");
        writer->WriteStartArray();
        for (usize i = 0; i < variables->computeBuffers.length; i++)
        {
            writer->WriteStartObject();

            writer->WritePropertyName("name");
            writer->WriteString(variables->computeBuffers.data[i].variableName.buffer);

            writer->WritePropertyName("set");
            writer->WriteUintValue(variables->computeBuffers.data[i].set);

            writer->WritePropertyName("binding");
            writer->WriteUintValue(variables->computeBuffers.data[i].binding);

            writer->WritePropertyName("mslBinding");
            writer->WriteUintValue(variables->computeBuffers.data[i].mslBinding);

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
        //need to replace all " with \"
        string formatted1 = ReplaceCharWithString(GetCAllocator(), msl.buffer, '"', "\\\"");
        string formatted2 = ReplaceCharWithString(GetCAllocator(), formatted1.buffer, '\n', "\\n");
        formatted1.deinit();
        writer->WriteString(formatted2.buffer);
        formatted2.deinit();
    }
}

bool AstralShaderc_WriteToFile(IAllocator allocator, string filePath, AstralShadercCompileResult *compiledShader)
{
    FILE *fs = NULL;
    if (fopen_s(&fs, filePath.buffer, "w") != 0)
    {
        fprintf(stderr, "Failed to create file\n");
        return false;
    }

    Json::JsonWriter writer = Json::JsonWriter(allocator, fs, true);

    writer.WriteStartObject();
    
    if (compiledShader->definedMaterials.Count > 0)
    {
        writer.WritePropertyName("materials");
        writer.WriteStartObject();
        for (usize i = 0; i < compiledShader->definedMaterials.bucketsCount; i++)
        {
            if (compiledShader->definedMaterials.buckets[i].initialized)
            {
                for (usize j = 0; j < compiledShader->definedMaterials.buckets[i].entries.count; j++)
                {
                    AstralShadercMaterialData *material = &compiledShader->definedMaterials.buckets[i].entries.ptr[j].value;
                    writer.WritePropertyName(material->materialName.buffer);
                    writer.WriteStartObject();
                    for (usize c = 0; c < material->parameters.length; c++)
                    {
                        writer.WritePropertyName(material->parameters.data[c].parameterName.buffer);
                        writer.WriteUintValue(material->parameters.data[c].size);
                    }
                    writer.WriteEndObject();
                }
            }
        }
        writer.WriteEndObject();
    }
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