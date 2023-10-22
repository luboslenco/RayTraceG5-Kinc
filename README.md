# RayTraceG5-Kinc

#### Direct3D12

Build:
`Kinc/make -g direct3d12`

Compile shader (Windows):
`.\dxc.exe -Zpr -Fo .\simple.cso -T lib_6_3 .\simple.hlsl`

Reference:
https://github.com/Microsoft/DirectX-Graphics-Samples/tree/master/Samples/Desktop/D3D12Raytracing

#### Vulkan

Build:
`Kinc/make -g vulkan`

Compile shader (Linux):
`./dxc -Zpr -Fo ./simple.spv -T lib_6_4 ./simple.hlsl -spirv -fvk-use-scalar-layout -fspv-target-env="vulkan1.2" -fvk-u-shift 1 all -fvk-b-shift 2 all`

Reference:
https://github.com/KhronosGroup/Vulkan-Samples/blob/main/samples/extensions/ray_tracing_basic/ray_tracing_basic.cpp

#### Metal

Build:
`Kinc/make -g metal`

Reference:
https://developer.apple.com/documentation/metal/metal_sample_code_library/accelerating_ray_tracing_using_metal

![](img.jpg)
