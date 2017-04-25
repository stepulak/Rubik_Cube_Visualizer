#ifndef UNIT_CUBE_H
#define UNIT_CUBE_H

#define GLEW_STATIC
#include <GL/glew.h>

#include "Camera.h"
#include "MatrixShaderUniforms.h"
#include "MaterialShaderUniforms.h"

// Simple unit cube which is used for drawing Rubik's cube parts (after specific transformations ofc.)
class UnitCube final {
private:

	GLuint m_verticesAndNormalsVBO;
	GLuint m_indicesVBO;
	GLuint m_numIndices;
	GLuint m_cubeVAO;

	glm::mat4 m_modelMatrix;

	// Reset all members to initial values, doesn't destroy anything
	void ResetAll();

	// Destroy and free content
	void DestroyAll();

	void CreateVerticesAndNormalsVBO();
	void CreateIndicesVBO();
	void CreateCubeVAO(GLint positionShaderAttribute, GLint normalShaderAttribute);

public:

	UnitCube(GLint positionShaderAttribute, GLint normalShaderAttribute);
	~UnitCube();

	UnitCube(UnitCube&& uc);
	UnitCube& operator=(UnitCube&& uc);

	// FYI: OpenGL mesh
	UnitCube(const UnitCube& c) = delete;
	UnitCube& operator=(const UnitCube&) = delete;

	inline float CubeSize() const { return 1.f; }
	inline const glm::mat4 GetModelMatrix() const { return m_modelMatrix; }
	inline glm::mat4 GetNormalMatrix() const { return glm::inverse(glm::transpose(glm::mat4(m_modelMatrix))); }

	UnitCube& Translate(const glm::vec3& vec);
	UnitCube& Rotate(float angle, const glm::vec3& vec);
	UnitCube& Scale(const glm::vec3& vec);
	UnitCube& Reset();

	inline UnitCube& Translate(float x, float y, float z) { return Translate(glm::vec3(x, y, z)); }
	inline UnitCube& Rotate(float angle, float x, float y, float z) { return Rotate(angle, glm::vec3(x, y, z)); }
	inline UnitCube& Scale(float sx, float sy, float sz) { return Scale(glm::vec3(sx, sy, sz)); }

	void Draw(const Camera& camera,
		const MatrixShaderUniforms& matrixUniforms,
		const MaterialShaderUniforms& materialUniforms) const;
};

#endif