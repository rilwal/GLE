
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include "ImGuizmo.h"

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

#include "shaders.hpp"
#include "model.hpp"
#include "object.hpp"
#include "camera.hpp"
#include "texture.hpp"

#include "VertexBuffer.hpp"
#include "property_list.hpp"

const char* glsl_version = "#version 440";



void EditTransform(const Camera& camera, glm::mat4& matrix)
{
	ImGuiIO& io = ImGui::GetIO();


	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
	if (ImGui::IsKeyPressed('Q'))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed('W'))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed('E')) // r Key
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	

	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(&camera.v[0][0], &camera.p[0][0], mCurrentGizmoOperation, mCurrentGizmoMode, &matrix[0][0], NULL, NULL);
}




int main() {
	AssetManager asset_manager;

	int display_w, display_h;

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	GLFWwindow* window = glfwCreateWindow(1920, 1080, "GLE", nullptr, nullptr);

	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	Shader s, t, blue;
	s.load_from_file("test.frag", Shader::Type::FRAGMENT);
	t.load_from_file("test.vert", Shader::Type::VERTEX);

	blue.load_from_file("blue.frag", Shader::Type::FRAGMENT);
	blue.name = "Blue";

	Shader pbr_frag, pbr_vert;
	pbr_frag.load_from_file("pbr.frag", Shader::Type::FRAGMENT);
	pbr_vert.load_from_file("pbr.vert", Shader::Type::VERTEX);

	Program pbr;
	pbr.vertex_shader = &pbr_vert;
	pbr.fragment_shader = &pbr_frag;

	pbr.link();


	Shader pbr2_frag, pbr2_vert;
	pbr2_frag.load_from_file("pbr2.frag", Shader::Type::FRAGMENT);
	pbr2_vert.load_from_file("pbr2.vert", Shader::Type::VERTEX);

	Program pbr2;
	pbr2.vertex_shader = &pbr2_vert;
	pbr2.fragment_shader = &pbr2_frag;

	pbr2.link();



	Program p;
	p.vertex_shader = &t;
	p.fragment_shader = &s;
	p.link();




	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);

	// Temporary test code to try and create and render a model
	Model& m = *asset_manager.LoadAsset<Model>("train.obj");

	Texture& tex = *asset_manager.LoadAsset<Texture2D>("test.png");


	VertexBuffer vb;
	vb.set_layout(m.layout);
	vb.set_data(m.gl_data);

	// set up a framebuffer!!!
	uint32_t framebuffer = 0;
	glCreateFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	uint32_t fb_texture;
	glGenTextures(1, &fb_texture);
	glBindTexture(GL_TEXTURE_2D, fb_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1920, 1080, 0, GL_RGBA, GL_FLOAT, nullptr);

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1080);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb_texture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	
	// A shader for post-processing
	Shader post_frag, post_vert;
	post_frag.load_from_file("post.frag", Shader::Type::FRAGMENT);
	post_vert.load_from_file("post.vert", Shader::Type::VERTEX);

	Program post;
	post.vertex_shader = &post_frag;
	post.fragment_shader = &post_vert;
	post.link();

	// Set up a camera. For now this needs to be done manually
	glm::mat4 projection = glm::perspective(45.f, 16.f / 9.f, 1.f, 100.f); // TODO: don't assume so much
	glm::mat4 view = glm::lookAt(glm::vec3{ 10.f, 10.f, 10.f }, glm::vec3{ 0.f, 0.f, 0.f }, glm::vec3{ 0.f, 1.f, 0.f });

	glm::mat4 vp = projection * view;
	
	Camera c {vp, view, projection};

	Object train("Train", {0, 0, -4});
	Object train2("Another Train", {0, 0, -8});

	train.model = &m;
	train.program = &p;
	train.material_id = p.create_material();
	train.vertex_buffer = &vb;

	train2.model = &m;
	train2.program = &pbr2;
	train2.material_id = pbr2.create_material();
	train2.vertex_buffer = &vb;

	pbr2.materials[train2.material_id].uniforms["albedo_map"].value.i = tex.get_tex_id();

	// Set up and use boombox model!
	Model& boombox_model = *asset_manager.LoadAsset<Model>("boombox.obj");
	VertexBuffer boombox_vbo;
	boombox_vbo.set_layout(boombox_model.layout);
	boombox_vbo.set_data(boombox_model.gl_data);

	Object boombox("Boombox", glm::vec3(0), glm::vec3(500));
	boombox.model = &boombox_model;
	boombox.program = &pbr2;
	boombox.material_id = pbr2.create_material();
	boombox.vertex_buffer = &boombox_vbo;

	Texture* boombox_albedo    = asset_manager.LoadAsset<Texture2D>("T_BoomBox_A.png");
	Texture* boombox_normal    = asset_manager.LoadAsset<Texture2D>("T_BoomBox_N.png");
	Texture* boombox_emissive  = asset_manager.LoadAsset<Texture2D>("T_BoomBox_e.png");
	Texture* boombox_roughness = asset_manager.LoadAsset<Texture2D>("T_BoomBox_mr.png");
	Texture* boombox_ao        = asset_manager.LoadAsset<Texture2D>("T_BoomBox_ao.png");

	pbr2.materials[boombox.material_id].uniforms["albedo_map"].value.i    = boombox_albedo->get_tex_id();
	pbr2.materials[boombox.material_id].uniforms["normal_map"].value.i    = boombox_normal->get_tex_id();
	pbr2.materials[boombox.material_id].uniforms["emissive_map"].value.i  = boombox_emissive->get_tex_id();
	pbr2.materials[boombox.material_id].uniforms["roughness_map"].value.i = boombox_roughness->get_tex_id();
	pbr2.materials[boombox.material_id].uniforms["ao_map"].value.i        = boombox_ao->get_tex_id();

	//Texture* skybox = asset_manager.LoadAsset<TextureCube>("skybox");

	//pbr2.materials[boombox.material_id].uniforms["env"].value.i = skybox->get_tex_id();


	///
	/// Set up a quaddy boi
	/// 
	
	std::vector<float> quad = {
		-1, 1,
		-1, -1,
		1, 1,
		1,1,
		-1, -1,
		1, -1
	};

	std::vector<uint8_t> raw;
	raw.resize(quad.size() * sizeof(float));
	memcpy(raw.data(), quad.data(), quad.size() * sizeof(float));

	VertexBuffer quad_vbo;
	quad_vbo.set_layout({ {"position", ShaderDataType::Vec2} });
	quad_vbo.set_data(raw);
	size_t post_mat_id = post.create_material();
	post.materials[post_mat_id].uniforms["input_fb"].value.i = fb_texture;


	// Load a compute shader to do bloomies
	Shader bloom_prefilter_s;
	bloom_prefilter_s.load_from_file("bloom_pf.glsl", Shader::Type::COMPUTE);

	Program bloom_prefilter;
	bloom_prefilter.compute_shader = &bloom_prefilter_s;
	bloom_prefilter.link();


	Shader bloom_downsample_s;
	bloom_downsample_s.load_from_file("bloom_ds.glsl", Shader::Type::COMPUTE);
	
	Program bloom_downsample;
	bloom_downsample.compute_shader = &bloom_downsample_s;
	bloom_downsample.link();


	Shader bloom_upsample_s;
	bloom_upsample_s.load_from_file("bloom_us.glsl", Shader::Type::COMPUTE);

	Program bloom_upsample;
	bloom_upsample.compute_shader = &bloom_upsample_s;
	bloom_upsample.link();


	Object* selected_object = &boombox;


	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();
		ImGuizmo::BeginFrame();

		ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

		ImGui::ShowDemoWindow();

		ImGui::PushID("Gizmo");
		glm::mat4 juke = {};
		ImGuizmo::RecomposeMatrixFromComponents(&selected_object->position[0], &selected_object->rotation[0], &selected_object->scale[0], &juke[0][0]);

		EditTransform(c, selected_object->model_matrix);
		
		ImGui::PopID();

		ImGuizmo::DecomposeMatrixToComponents(&juke[0][0], &selected_object->position[0], &selected_object->rotation[0], &selected_object->scale[0]);



		ImGui::Begin("Debug");

		if (ImGui::Button("Select Train"))
			selected_object = &train;

		if (ImGui::Button("Select BB"))
			selected_object = &boombox;

		if (ImGui::Button("Reload shaders")) {
			pbr_frag.load_from_file("pbr.frag", Shader::Type::FRAGMENT);
			pbr_vert.load_from_file("pbr.vert", Shader::Type::VERTEX);

			pbr.link();

			pbr2_frag.load_from_file("pbr2.frag", Shader::Type::FRAGMENT);
			pbr2_vert.load_from_file("pbr2.vert", Shader::Type::VERTEX);

			pbr2.link();


			post_frag.load_from_file("post.frag", Shader::Type::FRAGMENT);
			post_vert.load_from_file("post.vert", Shader::Type::VERTEX);

			post.link();

		}

		ImGui::End();

		static bool show_shader_window = true;
		static bool show_program_window = true;

		Shader::show_shaders_gui(show_shader_window);
		Program::show_programs_gui(show_program_window);
		asset_manager.render_asset_list();
		train.render_gui();
		train2.render_gui();
		boombox.render_gui();

		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//vb.bind();

		train.render(c);
		train2.render(c);

		boombox.render(c);
		

		ImGui::Begin("Camera");

		ImGui::DragFloat("Gamma", &c.gamma, 0.01, 0.0, 5.0);
		ImGui::DragFloat("Exposure", &c.exposure, 0.01, 0.0, 5.0);

		static float bloom_threshold = 1.0;
		ImGui::DragFloat("Bloom threshold", &bloom_threshold);

		ImGui::End();

		
		ImGui::EndFrame();
		//ImGui::UpdatePlatformWindows();


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


		// Try to apply our post-processing effects
		int32_t width, height;

		bloom_prefilter.use();
		glUniform1i(glGetUniformLocation(bloom_prefilter.program_id, "mip_in"), 1);
		glUniform1i(glGetUniformLocation(bloom_prefilter.program_id, "mip_out"), 2);
		//glUniform1f(glGetUniformLocation(bloom_prefilter.program_id, "threshold"), bloom_threshold);


		glBindImageTexture(1, fb_texture, 0, false, 0, GL_READ_ONLY, GL_RGBA16F);
		glBindImageTexture(2, fb_texture, 1, false, 0, GL_WRITE_ONLY, GL_RGBA16F);
		glGetTextureLevelParameteriv(fb_texture, 1, GL_TEXTURE_WIDTH, &width);
		glGetTextureLevelParameteriv(fb_texture, 1, GL_TEXTURE_HEIGHT, &height);

		glDispatchCompute(width, height, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		bloom_downsample.use();
		glUniform1i(glGetUniformLocation(bloom_downsample.program_id, "mip_in"), 1);
		glUniform1i(glGetUniformLocation(bloom_downsample.program_id, "mip_out"), 2);

		int32_t level = 1;


		while (width > 1 && height > 1) {
			glBindImageTexture(1, fb_texture, level, false, 0, GL_READ_WRITE, GL_RGBA16F);
			glBindImageTexture(2, fb_texture, level + 1, false, 0, GL_READ_WRITE, GL_RGBA16F);

			glGetTextureLevelParameteriv(fb_texture, level + 1, GL_TEXTURE_WIDTH, &width);
			glGetTextureLevelParameteriv(fb_texture, level + 1, GL_TEXTURE_HEIGHT, &height);

			glDispatchCompute(width, height, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

			level++;
		}

		level--;

		bloom_upsample.use();
		glUniform1i(glGetUniformLocation(bloom_upsample.program_id, "mip_in"), 1);
		glUniform1i(glGetUniformLocation(bloom_upsample.program_id, "mip_out"), 2);
		//glUniform1f(glGetUniformLocation(bloom_upsample.program_id, "radius"), 1.0);

		while (level > 0) {
			glBindImageTexture(1, fb_texture, level, false, 0, GL_READ_WRITE, GL_RGBA16F);
			glBindImageTexture(2, fb_texture, level - 1, false, 0, GL_READ_WRITE, GL_RGBA16F);

			glGetTextureLevelParameteriv(fb_texture, level - 1, GL_TEXTURE_WIDTH, &width);
			glGetTextureLevelParameteriv(fb_texture, level - 1, GL_TEXTURE_HEIGHT, &height);

			glDispatchCompute(width, height, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

			level--;
		}

		//glBindImageTexture(1, fb_texture, 1, false, 0, GL_READ_WRITE, GL_RGBA16F);
		//glBindImageTexture(2, fb_texture, 2, false, 0, GL_READ_WRITE, GL_RGBA16F);

		//glDispatchCompute(1920 / 32, 1080 / 32, 1);
		//glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);

		// Render our framebuffer to the screen


		quad_vbo.bind();
		post.materials[post_mat_id].uniforms["gamma"].value.f = c.gamma;
		post.materials[post_mat_id].uniforms["exposure"].value.f = c.exposure;
		post.materials[post_mat_id].use();

		glDisable(GL_DEPTH_TEST);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glEnable(GL_DEPTH_TEST);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
}

