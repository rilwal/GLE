
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

#include "VertexBuffer.hpp"

const char* glsl_version = "#version 440";


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
	Model& m = *asset_manager.LoadAsset<Model>("train.obj");




	VertexArray va;
	va.set_layout(m.layout);
	
	va.set_data(m.gl_data);


	// Set up a camera. For now this needs to be done manually
	glm::mat4 projection = glm::perspective(45.f, 16.f / 9.f, 1.f, 100.f); // TODO: don't assume so much
	glm::mat4 view = glm::lookAt(glm::vec3{ 10.f, 10.f, 10.f }, glm::vec3{ 0.f, 0.f, 0.f }, glm::vec3{ 0.f, 1.f, 0.f });

	glm::mat4 vp = projection * view;
	
	Camera c {vp};

	Object train("Train");
	Object train2("Another Train");

	train.model = &m;
	train.program = &p;
	train.material_id = p.create_material();

	train2.model = &m;
	train2.program = &p;
	train2.material_id = p.create_material();


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
		asset_manager.render_asset_list();
		train.render_gui();
		train2.render_gui();

		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		va.bind();

		train.render(c);
		train2.render(c);
		
		ImGui::EndFrame();
		//ImGui::UpdatePlatformWindows();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
}

