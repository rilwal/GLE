
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

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

const char* glsl_version = "#version 440";


int main() {

	int display_w, display_h;

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1920, 1080, "GLE", nullptr, nullptr);

	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);

	glEnable(GL_DEPTH_TEST);

	Shader s, t, blue;
	s.load_from_file("test.frag", Shader::Type::FRAGMENT);
	t.load_from_file("test.vert", Shader::Type::VERTEX);

	blue.load_from_file("blue.frag", Shader::Type::FRAGMENT);
	blue.name = "Blue";

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


	// Temporary test code to try and create and render a model
	Model m;
	m.load_from_obj("train_subd.obj");

	// TODO: move this into model.cpp
	uint32_t vao, vbo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * m.vertices.size() + sizeof(glm::vec3) * m.normals.size(), nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4) * m.vertices.size(), &m.vertices[0][0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * m.vertices.size(), sizeof(glm::vec3) * m.normals.size(), &m.normals[0].x);


	glVertexAttribPointer(0, 4, GL_FLOAT, 0, 0, 0); // vertex positions tightly packed
	glVertexAttribPointer(1, 3, GL_FLOAT, 0, 0, (void*)(sizeof(glm::vec4) * m.vertices.size())); // and normals

	// Set up a camera. For now this needs to be done manually
	glm::mat4 projection = glm::perspective(45.f, 16.f / 9.f, 1.f, 200.f); // TODO: don't assume so much
	glm::mat4 view = glm::lookAt(glm::vec3{ 10.f, 10.f, 10.f }, glm::vec3{ 0.f, 0.f, 0.f }, glm::vec3{ 0.f, 1.f, 0.f });

	glm::mat4 vp = projection * view;
	
	Camera c {vp};

	Object train("Train");

	train.model = &m;
	train.program = &p;

	for (auto& [name, uniform] : p.uniforms) {
		if (name == "mvp")
			uniform.value.m4 = vp;
	}


	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

		ImGui::ShowDemoWindow();

		static bool show_shader_window = true;
		static bool show_program_window = true;

		Shader::show_shaders_gui(show_shader_window);
		Program::show_programs_gui(show_program_window);
		train.render_gui();


		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		train.render(c);
		ImGui::EndFrame();
		//ImGui::UpdatePlatformWindows();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
}
