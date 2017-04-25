#ifndef CAMERA_H
#define CAMERA_H

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/freeglut.h>

// Simple camera with "arcball" mechanism
// Quaternions are overkill for this project since it has no complex animations
class Camera {
private:

	static const float ZOOM_Q;
	static const float ROTATION_Q;
	static const float MAX_RADIUS;
	static const float MIN_RADIUS;

	glm::mat4 m_projectionMatrix;
	glm::vec3 m_eyePosition;
	float m_xAngle;
	float m_yAngle;
	float m_radius;

	void RecalculateEyePosition();

public:

	Camera(float windowWidth, float windowHeight);
	virtual ~Camera() {}

	inline const glm::mat4& GetProjectionMatrix() const { return m_projectionMatrix; }
	inline const glm::vec3& GetEyePosition() const { return m_eyePosition; }
	inline glm::mat4 GetMatrix() const { return m_projectionMatrix * GetViewMatrix(); }
	inline glm::mat4 GetViewMatrix() const { return glm::lookAt(m_eyePosition, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f)); }
	
	void Resize(float windowWidth, float windowHeight);

	Camera& Rotate(float dX, float dY);
	Camera& ZoomIn();
	Camera& ZoomOut();
	Camera& Reset();
};

#endif