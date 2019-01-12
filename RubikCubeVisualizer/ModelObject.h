#ifndef MODEL_OBJECT_H
#define MODEL_OBJECT_H

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

class ModelObject {
protected:

	glm::mat4 m_modelMatrix;

public:

	ModelObject() { ResetTransformations(); }
	virtual ~ModelObject() {}

	ModelObject& Translate(const glm::vec3& vec) {
		m_modelMatrix = glm::translate(m_modelMatrix, vec);
		return *this;
	}

	ModelObject& Rotate(float angle, const glm::vec3& vec) {
		m_modelMatrix = glm::rotate(m_modelMatrix, angle, vec);
		return *this;
	}

	ModelObject& Scale(const glm::vec3& vec) {
		m_modelMatrix = glm::scale(m_modelMatrix, vec);
		return *this;
	}

	ModelObject& ResetTransformations() {
		m_modelMatrix = glm::mat4(1.f);
		return *this;
	}

	ModelObject& Translate(float x, float y, float z) { return Translate(glm::vec3(x, y, z)); }
	ModelObject& Rotate(float angle, float x, float y, float z) { return Rotate(angle, glm::vec3(x, y, z)); }
	ModelObject& Scale(float sx, float sy, float sz) { return Scale(glm::vec3(sx, sy, sz)); }

	const glm::mat4& GetModelMatrix() const { return m_modelMatrix; }
	glm::mat4 GetNormalMatrix() const { return glm::inverse(glm::transpose(glm::mat4(m_modelMatrix))); }
};

#endif