
#include "model.hpp"

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


// TODO: Rewrite this so it works with OpenGL 4 (the vertex, texture and normal indices need to be the same).
void Model::load_from_obj(std::string filename) {
	FILE* f;

#ifdef _WIN32
	// _s variants for windows compatibility
	errno_t err = fopen_s(&f, filename.c_str(), "rb");
	if (err != 0 || f == nullptr) {
		char message[1024];
		strerror_s(message, err);
		fprintf(stderr, "Failed to load model '%s': %s \n", filename.c_str(), message);
		return;
	}

#else
	f = fopen(filename.c_str(), "rb");
	if (f == nullptr) {
		fprintf(stderr, "Failed to load model '%s': %s \n", filename.c_str(), strerror(errno));
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

	// Iterate over data, parsing as we go
	const char* it = data;
	while (*it) {
		if (*it == 'v') {
			it++;
			if (*it == ' ') {
				// Vertex
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
				// Texture
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

				indices.push_back(v);
			}
		}
		else if (*it == '#') {}
		else {
			// Unsupported lines
			printf("Unsupported Line: ");
			while (*it != '\n') putchar(*it++);
			putchar('\n');
		}
	}
	skip_to_newline(it);

	delete[] data;
}