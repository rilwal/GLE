
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION

#include <GLFW/glfw3.h>
#include "shaders.hpp"

const char* glsl_version = "#version 440";

std::vector<Shader> shaders;

int main() {

	int display_w, display_h;

	glfwInit();
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1280, 800, "GLE", nullptr, nullptr);

	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);


	Shader s, t;
	s.load_from_file("test.frag", Shader::Type::FRAGMENT);
	t.load_from_file("test.vert", Shader::Type::VERTEX);

	shaders.push_back(s);
	shaders.push_back(t);

	Program p;
	p.link(shaders);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);


	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		static bool show_shader_windpw = true;
		if (show_shader_windpw) {
			if (ImGui::Begin("Shaders", &show_shader_windpw)) {
				p.render_gui();
				ImGui::End();
			}
		}

		ImGui::Render();
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		glfwSwapBuffers(window);
	}
}
