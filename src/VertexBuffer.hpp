#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <format>
#include <iostream>

#include <glad/gl.h>


enum class ShaderDataType {
	Float,
	Vec2,
	Vec3,
	Vec4,
	Int,
	Int2, 
	Int3, 
	Int4,
	Mat3,
	Mat4
};


constexpr size_t GetGLTypeSize(GLenum gl_type) {
	switch (gl_type) {
		case GL_FLOAT: return 4;
		case GL_INT:   return 4;
	}

	return 0;
}


constexpr GLenum GetGLType(ShaderDataType dt) {
	switch (dt) {
	case ShaderDataType::Float: return GL_FLOAT;
	case ShaderDataType::Vec2:	return GL_FLOAT;
	case ShaderDataType::Vec3:	return GL_FLOAT;
	case ShaderDataType::Vec4:	return GL_FLOAT;
	case ShaderDataType::Int:	return GL_INT;
	case ShaderDataType::Int2:	return GL_INT;
	case ShaderDataType::Int3:	return GL_INT;
	case ShaderDataType::Int4:	return GL_INT;
	case ShaderDataType::Mat3:	return GL_FLOAT;
	case ShaderDataType::Mat4:	return GL_FLOAT;
	}

	return GL_NONE;
}


constexpr size_t GetComponentCount(ShaderDataType dt) {
	switch (dt) {
	case ShaderDataType::Float: return 1;
	case ShaderDataType::Vec2:	return 2;
	case ShaderDataType::Vec3:	return 3;
	case ShaderDataType::Vec4:	return 4;
	case ShaderDataType::Int:	return 1;
	case ShaderDataType::Int2:	return 2;
	case ShaderDataType::Int3:	return 3;
	case ShaderDataType::Int4:	return 4;
	case ShaderDataType::Mat3:	return 3 * 3;
	case ShaderDataType::Mat4:	return 4 * 4;
	}

	return GL_NONE;
}


constexpr size_t GetSize(ShaderDataType dt) {
	return GetGLTypeSize(GetGLType(dt)) * GetComponentCount(dt);
}



struct VertexAttribute {
	std::string name;
	ShaderDataType data_type;
	size_t offset = 0;
};


struct VertexArrayLayout {
	std::vector<VertexAttribute> vertex_attributes;

	VertexArrayLayout() {

	}

	VertexArrayLayout(std::initializer_list<VertexAttribute> il)
		: vertex_attributes(il) { };

	constexpr std::vector<VertexAttribute>::iterator begin() { return vertex_attributes.begin(); }
	constexpr std::vector<VertexAttribute>::iterator end() { return vertex_attributes.end(); }

};


// An OpenGL Vertex Array Object
class VertexArray {
private:
	uint32_t m_id = 0;
	uint32_t m_stride = 0;

	uint32_t m_buffer_id = 0;

	VertexArrayLayout m_layout = {};

public:
	VertexArray() {
		glGenVertexArrays(1, &m_id);
		glBindVertexArray(m_id);

		glGenBuffers(1, &m_buffer_id);
		glBindBuffer(GL_ARRAY_BUFFER, m_buffer_id);
	}

	~VertexArray() {
		if (m_id) {
			glDeleteVertexArrays(1, &m_id);
		}

		if (m_buffer_id) {
			glDeleteBuffers(1, &m_buffer_id);
		}
	}

	void bind() {
		glBindVertexArray(m_id);
		glBindBuffer(GL_ARRAY_BUFFER, m_buffer_id);
	}

	void set_layout(VertexArrayLayout layout) {
		bind();

		size_t size = 0;

		// First count the size of our layout
		for (const auto& attribute : layout) {
			size += GetSize(attribute.data_type);
		}

		// Then loop over and set everything up
		size_t index = 0; 
		size_t offset = 0;

		for (auto& attribute : layout) {

			const ShaderDataType& dt = attribute.data_type;

			std::cout << std::format("glVertexArrayAttribFormat(id={}, index={}, GetSize(dt)={}, GetGLType(dt)={}, false, offset={});", m_id, index, GetSize(dt), GetGLType(dt), offset);

			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index, GetComponentCount(dt), GetGLType(dt), false, size, (const void*)offset);
			//glVertexArrayAttribFormat(m_id, index, GetSize(dt), GetGLType(dt), false, offset);

			attribute.offset = offset;

			offset += GetSize(dt);
			index++;
		}

		m_stride = size;
	}
	

	void resize() {

	}


	void set_data(std::vector<uint8_t> data) {
		glNamedBufferData(m_buffer_id, data.size(), &data[0], GL_STATIC_DRAW);
	}

};


class VertexBuffer {
	
};

