
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION

#include <GLFW/glfw3.h>

#include "shaders.hpp"
#include "model.hpp"

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

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);


	// Temporary test code to try and create and render a model
	Model m;
	m.vertices = { {0, .5, 0, 1}, {.5, -.5, 0, 1}, {-.5, -.5, 0, 1} };
	m.indices = { 0, 1, 2 };

	uint32_t vao, vbo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * m.vertices.size(), &m.vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, 0, 0, 0); // vertex positions tightly packed


	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		ImGui::ShowDemoWindow();

		static bool show_shader_window = true;
		static bool show_program_window = true;

		Shader::show_shaders_gui(show_shader_window);
		Program::show_programs_gui(show_program_window);



		ImGui::Render();
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);


		// More rendering test code
		if (p.linked) {
			p.use();
			glDrawElements(GL_TRIANGLES, m.indices.size(), GL_UNSIGNED_INT, &m.indices[0]);
		}

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
}
