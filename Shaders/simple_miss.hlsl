
#ifndef RAYTRACING_MISS_HLSL
#define RAYTRACING_MISS_HLSL

struct RayPayload {
    float4 color;
};

[shader("miss")]
void miss(inout RayPayload payload) {
    payload.color = float4(0, 0, 0, 1);
}

#endif // RAYTRACING_MISS_HLSL
