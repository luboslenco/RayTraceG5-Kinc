#include <kinc/io/filereader.h>
#include <kinc/graphics5/commandlist.h>
#include <kinc/graphics5/graphics.h>
#include <kinc/graphics5/pipeline.h>
#include <kinc/graphics5/shader.h>
#include <kinc/graphics5/texture.h>
#include <kinc/graphics5/vertexbuffer.h>
#include <kinc/graphics5/indexbuffer.h>
#include <kinc/graphics5/constantbuffer.h>
#include <kinc/graphics5/raytrace.h>
#include <kinc/system.h>
#include <kinc/window.h>

#include <assert.h>
#include <stdlib.h>

#define BUFFER_COUNT 2
static int current_buffer = -1;
static kinc_g5_render_target_t framebuffers[BUFFER_COUNT];
static kinc_g5_command_list_t command_list;
static kinc_g5_constant_buffer_t constant_buffer;
static kinc_g5_texture_t output;
static kinc_raytrace_pipeline_t pipeline;
static kinc_raytrace_acceleration_structure_t accel;

#define HEAP_SIZE 1024 * 1024
static uint8_t *heap = NULL;
static size_t heap_top = 0;

static void *allocate(size_t size) {
	size_t old_top = heap_top;
	heap_top += size;
	assert(heap_top <= HEAP_SIZE);
	return &heap[old_top];
}

static void update() {
	current_buffer = (current_buffer + 1) % BUFFER_COUNT;
	kinc_g5_begin(&framebuffers[current_buffer], 0);

	kinc_g5_command_list_begin(&command_list);
	kinc_raytrace_set_acceleration_structure(&accel);
	kinc_raytrace_set_pipeline(&pipeline);
	kinc_raytrace_set_target(&output);
	kinc_raytrace_dispatch_rays(&command_list);
	kinc_raytrace_copy(&command_list, &framebuffers[current_buffer], &output);
	kinc_g5_command_list_end(&command_list);

	kinc_g5_end(0);
	kinc_g5_swap_buffers();
}

int kickstart(int argc, char** argv) {
	kinc_init("Raytracing", 1280, 720, NULL, NULL);
	kinc_set_update_callback(update);

	heap = (uint8_t*)malloc(HEAP_SIZE);
	assert(heap != NULL);

	kinc_g5_command_list_init(&command_list);
	for (int i = 0; i < BUFFER_COUNT; ++i) {
		kinc_g5_render_target_init(&framebuffers[i], kinc_window_width(0), kinc_window_height(0), 16, false, KINC_G5_RENDER_TARGET_FORMAT_32BIT, -1,
			-i - 1 /* hack in an index for backbuffer render targets */);
	}

	kinc_g5_command_list_end(&command_list); // TODO: Otherwise "This API cannot be called on a closed command list."

	kinc_file_reader_t file;
	kinc_file_reader_open(&file, "simple.cso", KINC_FILE_TYPE_ASSET);
	size_t data_size = kinc_file_reader_size(&file);
	uint8_t *data = allocate(data_size);
	kinc_file_reader_read(&file, data, data_size);
	kinc_file_reader_close(&file);

	kinc_g5_constant_buffer_init(&constant_buffer, 4 * 4);
	kinc_g5_constant_buffer_lock_all(&constant_buffer);
	kinc_g5_constant_buffer_set_float(&constant_buffer, 0, -1);
	kinc_g5_constant_buffer_set_float(&constant_buffer, 4, -1);
	kinc_g5_constant_buffer_set_float(&constant_buffer, 8, 1);
	kinc_g5_constant_buffer_set_float(&constant_buffer, 12, 1);
	kinc_g5_constant_buffer_unlock(&constant_buffer);

	kinc_g5_texture_init_non_sampled_access(&output, kinc_window_width(0), kinc_window_height(0), KINC_IMAGE_FORMAT_RGBA32);

	kinc_raytrace_pipeline_init(&pipeline, &command_list, data, data_size, &constant_buffer);

	kinc_g5_vertex_structure_t structure;
	kinc_g4_vertex_structure_init(&structure);
	kinc_g4_vertex_structure_add(&structure, "pos", KINC_G4_VERTEX_DATA_FLOAT3);

	kinc_g5_vertex_buffer_t vertices;
	kinc_g5_vertex_buffer_init(&vertices, 3, &structure, true, 0);
	float *v = kinc_g5_vertex_buffer_lock_all(&vertices);
	v[0] = 0.0f; v[1] =-0.7f; v[2] = 1.0f;
	v[3] =-0.7f; v[4] = 0.7f; v[5] = 1.0f;
	v[6] = 0.7f; v[7] = 0.7f; v[8] = 1.0f;
	kinc_g5_vertex_buffer_unlock_all(&vertices);
	// kinc_g5_command_list_upload_vertex_buffer(&command_list, &vertices);

	kinc_g5_index_buffer_t indices;
	kinc_g5_index_buffer_init(&indices, 3, true);
	int *i = kinc_g5_index_buffer_lock(&indices);
	i[0] = 0; i[1] = 1; i[2] = 2;
	kinc_g5_index_buffer_unlock(&indices);
	// kinc_g5_command_list_upload_index_buffer(&command_list, &indices);

	kinc_raytrace_acceleration_structure_init(&accel, &command_list, &vertices, &indices);

	kinc_start();

	return 0;
}
