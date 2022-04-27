#version 450

layout(location=0) in vec3 inPosition;

layout(set=0 binding=0) uniform FrameUBO
{
   mat4 u_projectionMatrix;
   mat4 u_viewMatrix;
}

void main()
{
    gl_Position = u_projectionMatrix * u_viewMatrix * vec4(inPosition, 1.0f);
}