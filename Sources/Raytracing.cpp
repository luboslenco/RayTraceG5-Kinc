#include "pch.h"

#include <Kore/IO/FileReader.h>
#include <Kore/Graphics5/CommandList.h>
#include <Kore/Graphics5/ConstantBuffer.h>
#include <Kore/Graphics5/Graphics.h>
#include <Kore/Graphics5/RayTrace.h>
#include <Kore/System.h>

using namespace Kore;

namespace {
	const int bufferCount = 2;
	int currentBuffer = -1;
	Graphics5::RenderTarget* framebuffers[bufferCount];
	Graphics5::CommandList* commandList;
	Graphics5::ConstantBuffer* constantBuffer;
	Graphics5::RayTraceTarget* target;
	Graphics5::RayTracePipeline* pipeline;
	Graphics5::AccelerationStructure* accel;

	void update() {
		currentBuffer = (currentBuffer + 1) % bufferCount;
		Graphics5::begin(framebuffers[currentBuffer]);

		commandList->begin();
		Graphics5::setAccelerationStructure(accel);
		Graphics5::setRayTracePipeline(pipeline);
		Graphics5::setRayTraceTarget(target);
		Graphics5::dispatchRays(commandList);
		Graphics5::copyRayTraceTarget(commandList, framebuffers[currentBuffer], target);
		commandList->end();
		
		Graphics5::end();
		Graphics5::swapBuffers();
	}
}

int kore(int argc, char** argv) {
	Kore::WindowOptions options;
	options.title = "Shader";
	options.width = 1280;
	options.height = 720;
	options.mode = WindowMode::WindowModeWindow;
	Kore::System::init(options.title, options.width, options.height, &options);
	Kore::System::setCallback(update);

	// Command list
	commandList = new Graphics5::CommandList;
	for (int i = 0; i < bufferCount; ++i) {
		framebuffers[i] = new Graphics5::RenderTarget(System::windowWidth(0), System::windowHeight(0), 16, false, Graphics5::Target32Bit,
		                                              -1, -i - 1 /* hack in an index for backbuffer render targets */);
	}
	
	commandList->end(); // TODO: Otherwise "Reset fails because the command list was not closed"

	// Pipeline
	FileReader rayTraceShader("simple.cso");

	constantBuffer = new Graphics5::ConstantBuffer(4 * 4);
	constantBuffer->lock();
	constantBuffer->setFloat(0, -1);
	constantBuffer->setFloat(4, -1);
	constantBuffer->setFloat(8, 1);
	constantBuffer->setFloat(12, 1);
	constantBuffer->unlock();

	pipeline = new Graphics5::RayTracePipeline(commandList, rayTraceShader.readAll(), rayTraceShader.size(), constantBuffer);

	// Acceleration structure
	Graphics4::VertexStructure structure;
	structure.add("pos", Graphics4::Float3VertexData);
	Graphics5::VertexBuffer* vb = new Graphics5::VertexBuffer(3, structure, false);
	float* v = vb->lock();
	v[0] = 0.0f; v[1] =-0.7f; v[2] = 1.0f;
	v[3] =-0.7f; v[4] = 0.7f; v[5] = 1.0f;
	v[6] = 0.7f; v[7] = 0.7f; v[8] = 1.0f;
	vb->unlock();

	Graphics5::IndexBuffer* ib = new Graphics5::IndexBuffer(3, true);
	int* i = ib->lock();
	i[0] = 0; i[1] = 1; i[2] = 2;
	ib->unlock();

	accel = new Graphics5::AccelerationStructure(commandList, vb, ib);

	// Output
	target = new Graphics5::RayTraceTarget(1280, 720);

	Kore::System::start();

	return 0;
}
