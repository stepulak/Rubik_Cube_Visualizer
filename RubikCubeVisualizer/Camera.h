#ifndef CAMERA_H
#define CAMERA_H

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/freeglut.h>

// Simple camera with "arcball" mechanism
// Quaternions are overkill for this project since it has no complex animations
class Camera {
private:

	static constexpr float ZOOM_Q = 1.1f;
	static constexpr float ROTATION_Q = 0.005f;
	static constexpr float MAX_RADIUS = 12.f;
	static constexpr float MIN_RADIUS = 2.f;

	glm::mat4 m_projectionMatrix;
	glm::vec3 m_eyePosition;
	float m_xAngle;
	float m_yAngle;
	float m_radius;

	void RecalculateEyePosition();

public:

	Camera(float windowWidth, float windowHeight);
	virtual ~Camera() {}

	const glm::mat4& GetProjectionMatrix() const { return m_projectionMatrix; }
	const glm::vec3& GetEyePosition() const { return m_eyePosition; }
	glm::mat4 GetMatrix() const { return m_projectionMatrix * GetViewMatrix(); }
	glm::mat4 GetViewMatrix() const { return glm::lookAt(m_eyePosition, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f)); }
	
	void Resize(float windowWidth, float windowHeight);

	Camera& Rotate(float dX, float dY);
	Camera& ZoomIn();
	Camera& ZoomOut();
	Camera& Reset();
};

#endif