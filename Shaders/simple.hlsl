
struct RayGenConstantBuffer {
	float4 viewport;
};

struct RayPayload {
	float4 color;
};

RaytracingAccelerationStructure Scene : register(t0, space0);
RWTexture2D<float4> RenderTarget : register(u0);
ConstantBuffer<RayGenConstantBuffer> g_rayGenCB : register(b0);

[shader("raygeneration")]
void raygeneration() {
	float2 lerpValues = (float2)DispatchRaysIndex() / (float2)DispatchRaysDimensions();
	float3 rayDir = float3(0, 0, 1);
	float3 origin = float3( // x = left, y = top, z = right, w = bottom
		lerp(g_rayGenCB.viewport.x, g_rayGenCB.viewport.z, lerpValues.x),
		lerp(g_rayGenCB.viewport.y, g_rayGenCB.viewport.w, lerpValues.y),
		0.0f);

	RayDesc ray;
	ray.Origin = origin;
	ray.Direction = rayDir;
	ray.TMin = 0.001;
	ray.TMax = 100.0;
	RayPayload payload = { float4(0, 0, 0, 0) };
	TraceRay(Scene, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, ~0, 0, 1, 0, ray, payload);
	RenderTarget[DispatchRaysIndex().xy] = payload.color;
}

[shader("closesthit")]
void closesthit(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attr) {
    float3 barycentrics = float3(1 - attr.barycentrics.x - attr.barycentrics.y, attr.barycentrics.x, attr.barycentrics.y);
    payload.color = float4(barycentrics, 1);
}

[shader("miss")]
void miss(inout RayPayload payload) {
    payload.color = float4(0, 0, 0, 1);
}
