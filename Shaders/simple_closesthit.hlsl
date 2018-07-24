
#ifndef RAYTRACING_HIT_HLSL
#define RAYTRACING_HIT_HLSL

struct RayPayload {
    float4 color;
};

[shader("closesthit")]
void closesthit(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attr) {
    float3 barycentrics = float3(1 - attr.barycentrics.x - attr.barycentrics.y, attr.barycentrics.x, attr.barycentrics.y);
    payload.color = float4(barycentrics, 1);
}

#endif // RAYTRACING_HIT_HLSL
