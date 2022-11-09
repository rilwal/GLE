
#include "model.hpp"

#include <map>
#include <tuple>

inline void skip_to_newline(const char*& it) {
	// Skip until the character following the next newline character
	while (*it != '\n') it++;
	it++;
}

inline void skip_whitespace(const char*& it) {
	// Skip until the next non-whitespace character
	while (std::isspace(*it)) it++;
}

inline void skip_whitespace_not_nl(const char*& it) {
	// Skip until the next non-whitespace character excluding newline
	while (*it != '\n' && std::isspace(*it)) it++;
}


inline float read_float(const char*& it) {
	// Read a float from a string of characters
	char* end;
	float n = strtof(it, &end);
	it = end;
	return n;
}

inline int read_int(const char*& it) {
	// Read a float from a string of characters
	char* end;
	int n = strtol(it, &end, 10);
	it = end;
	return n;
}


void Model::load(const char* filename) {
	path = filename;

	FILE* f;

#ifdef _WIN32
	// _s variants for windows compatibility
	errno_t err = fopen_s(&f, filename, "rb");
	if (err != 0 || f == nullptr) {
		char message[1024];
		strerror_s(message, err);
		fprintf(stderr, "Failed to load model '%s': %s \n", filename, message);
		return;
	}

#else
	f = fopen(filename.c_str(), "rb");
	if (f == nullptr) {
		fprintf(stderr, "Failed to load model '%s': %s \n", filename, strerror(errno));
		return;
	}
#endif

	// Get the file size
	fseek(f, 0, SEEK_END);
	size_t file_size = ftell(f);
	rewind(f);

	// Read whole file into data
	char* data = new char[file_size + 1];
	fread(data, file_size, 1, f);
	fclose(f);
	data[file_size] = 0;

	// Temporary index vector, tuple goes Vertex, UV, Normal
	std::vector<std::tuple<int, int, int>> _indices;

	// Iterate over data, parsing as we go
	const char* it = data;
	while (*it) {
		if (*it == 'v') {
			it++;
			if (*it == ' ') {
				// Vertex
				// Format is:
				// v {x} {y} {z} {w}
				// w is optional

				float x, y, z, w;

				skip_whitespace(it);
				x = read_float(it);
				skip_whitespace(it);
				y = read_float(it);
				skip_whitespace(it);
				z = read_float(it);
				skip_whitespace_not_nl(it);
				if (*it != '\n')
					w = read_float(it);
				else
					w = 1.0f;
				
				vertices.push_back({ x, y, z, w });
			}
			else if (*it == 'n') {
				// Normal
				// Format:
				// vn {x} {y} {z}
				it++;
				float x, y, z;

				skip_whitespace(it);
				x = read_float(it);
				skip_whitespace(it);
				y = read_float(it);
				skip_whitespace(it);
				z = read_float(it);

				normals.push_back({ x, y, z });
			}
			else if (*it == 't') {
				// Texture UV
				// Format
				// vt {u} {v}
				// TODO: support the proper format vt {u} [{v} {w}]

				it++;
				float u, v;

				skip_whitespace(it);
				u = read_float(it);
				skip_whitespace(it);
				v = read_float(it);

				uvs.push_back({ u, v });
			}
		}
		else if (*it == 'f') {
			// Faces
			// Format:
			// f {v}[/{t}[/{n}]]
			// Where:
			//   v is the index of the vertex to render
			//   t is the index of the texture coordinate
			//   n is the index of the vertex normal
			it++;
			size_t v = 0, t = 0, n = 0;

			for (int i = 0; i < 3; i++) {
				skip_whitespace(it);
				v = read_int(it);
				if (*it == '/') {
					it++;
					t = read_int(it);
					if (*it == '/') {
						it++;
						n = read_int(it);
					}
				}

				_indices.push_back({ v - 1, t - 1, n - 1 });
			}
		}
		else if (*it == '#') {} // do nothing for comments
		else {
			// Unsupported lines
			printf("Unsupported Line: ");
			while (*it != '\n') putchar(*it++);
			putchar('\n');
		}
		skip_to_newline(it);

	}

	delete[] data;

	// Now it's time to make this into a format that makes sense for modern OpenGL
	std::map<std::tuple<int, int, int>, int> conversion_map;
	std::vector<glm::vec4> _vertices;
	std::vector<glm::vec2> _uvs;
	std::vector<glm::vec3> _normals;
	int i = 0; // running tally

	for (auto& [v, vt, vn] : _indices) {
		int index = 0;
		if (conversion_map.contains( { v, vt, vn })) {
			index = conversion_map[{v, vt, vn}];
		}
		else {
			index = i++;
			conversion_map[{v, vt, vn}] = index;
			if (v != -1)	//should never be -1
				_vertices.push_back(vertices[v]);
			if (vt != -1)
				_uvs.push_back(uvs[vt]);
			if (vn != -1)
			_normals.push_back(normals[vn]);
		}

		indices.push_back(index);
	}

	vertices = _vertices;
	normals = _normals;
	uvs = _uvs;

	bool generate_tangents   = false;
	bool generate_bitangents = false;

	bool has_norm = normals.size() != 0;
	bool has_uvs  = uvs.size() != 0;

	size_t stride = sizeof(glm::vec4);
	if (has_norm) stride += sizeof(glm::vec3);
	if (has_uvs)  stride += sizeof(glm::vec2);

	if (generate_tangents)	 stride += sizeof(glm::vec3);
	if (generate_bitangents) stride += sizeof(glm::vec3);


	if (generate_tangents) {
		glm::vec3* tan1 = new glm::vec3[vertices.size()];
		glm::vec3* tan2 = new glm::vec3[vertices.size()];

		for (size_t a = 0; a < vertices.size() / 3; a++) {

		}
	}


	gl_data.resize(stride * vertices.size());
	for (size_t i = 0; i < vertices.size(); i++) {
		size_t offset = 0;
		memcpy(&gl_data[i * stride + offset], &vertices[i], sizeof(glm::vec4));
		offset += sizeof(glm::vec4);

		if (has_norm) {
			memcpy(&gl_data[i * stride + offset], &normals[i], sizeof(glm::vec3));
			offset += sizeof(glm::vec3);
		}

		if (has_uvs) {
			memcpy(&gl_data[i * stride + offset], &uvs[i], sizeof(glm::vec2));
			offset += sizeof(glm::vec2);
		}

	}


	layout = {};

	layout.append({ "vertex_pos", ShaderDataType::Vec4 });
	if (has_norm)			 layout.append({ "normal", ShaderDataType::Vec3 });
	if (has_uvs)			 layout.append({ "uvs", ShaderDataType::Vec2 });
	if (generate_tangents)   layout.append({ "tangent", ShaderDataType::Vec3 });
	if (generate_bitangents) layout.append({ "bitangent", ShaderDataType::Vec3 });

}

void Model::unload() {
	vertices.clear();
	normals.clear();
	uvs.clear();
	indices.clear();
}