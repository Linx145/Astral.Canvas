#vertex
#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 UV;

layout(location = 3) in int textureID;
layout(location = 4) in mat4 instanceData;

layout(binding = 0) uniform Matrices
{
    mat4 world;
    mat4 view;
    mat4 proj;
} matrices;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) flat out int fragTexID;

void main() {
    vec4 worldPosition = instanceData * matrices.world * vec4(position, 1.0);
    gl_Position = matrices.proj * matrices.view * worldPosition;

    fragColor = color;
    fragTexCoord = UV;
    fragTexID = textureID;
}

#fragment
#version 450

layout(binding = 1) uniform sampler samplerState;
layout(binding = 2) uniform texture2D inputTexture[4];

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) flat in int fragTexID;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 col = texture(sampler2D(inputTexture[fragTexID], samplerState), fragTexCoord) * fragColor;

    if (col.a < 0.01)
    {
        discard;
    }
    outColor = col;
}