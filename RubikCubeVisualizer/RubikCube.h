#ifndef RUBIK_CUBE_H
#define RUBIK_CUBE_H

#include "UnitCube.h"
#include "Sticker.h"
#include <memory>
#include <vector>
#include <array>
#include <mutex>

class RubikCube final {
public:
	
	enum RotationType {
		X_AXIS,
		Y_AXIS,
		Z_AXIS,
		NONE,
	};

private:

	enum FaceIndex {
		TOP = 0,
		BOTTOM,
		LEFT,
		RIGHT,
		FRONT,
		BACK
	};

	typedef std::vector<Sticker::Color> StickerLine;
	typedef std::vector<StickerLine> Face;

	static const unsigned int MAX_STICKERS_PER_LINE = 15u;
	static const float ROTATION_TIME;

	std::array<Face, 6> m_faces;
	std::unique_ptr<UnitCube> m_unitCube;
	std::unique_ptr<Sticker> m_sticker;

	RotationType m_rotationType;
	unsigned int m_rotationIndex;
	bool m_rotationClockwise;
	float m_rotationTimer;
	
	mutable std::mutex m_mutex;

	// Reset all rotation* values to "zero", do not destroy anything
	void ResetAll();

	// Destroy cube's content
	void DestroyAll();

	void FillFaceWithColor(FaceIndex face, Sticker::Color c, unsigned int numStickersEdge);
	
	void RotateFace(FaceIndex face, bool clockwise);
	void SwapStickerColors(Sticker::Color& c1, Sticker::Color& c2, Sticker::Color& c3, Sticker::Color& c4);

	void SwapFacesXAxisRotation();
	void SwapFacesYAxisRotation();
	void SwapFacesZAxisRotation();

	inline float GetRotationAngle() const
		{ return m_rotationTimer / ROTATION_TIME * glm::half_pi<float>() * ((m_rotationClockwise) ? 1.f : -1.f); }

	inline float GetStickerSize() const
		{ return m_unitCube->CubeSize() / (GetNumStickersPerEdge() * m_sticker->StickerSize()); }

	void DrawFace(FaceIndex face,
		const glm::mat4& rotationMatrix,
		unsigned int startX, unsigned int startY,
		unsigned int endX, unsigned int endY,
		const Camera& camera,
		const MatrixShaderUniforms& matrixUniforms, 
		const MaterialShaderUniforms& materialUniforms) const;

	void DrawCubeNoRotation(const Camera& camera,
		const MatrixShaderUniforms& matrixUniforms,
		const MaterialShaderUniforms& materialUniforms) const;

	void DrawCubeXAxisRotation(const Camera& camera,
		const MatrixShaderUniforms& matrixUniforms,
		const MaterialShaderUniforms& materialUniforms) const;

	void DrawCubeYAxisRotation(const Camera& camera,
		const MatrixShaderUniforms& matrixUniforms,
		const MaterialShaderUniforms& materialUniforms) const;

	void DrawCubeZAxisRotation(const Camera& camera,
		const MatrixShaderUniforms& matrixUniforms,
		const MaterialShaderUniforms& materialUniforms) const;

	void DrawUnitCubeGenericRotation(const Camera& camera,
		const glm::vec3& transformationVec,
		const MatrixShaderUniforms& matrixUniforms,
		const MaterialShaderUniforms& materialUniforms) const;

public:

	// Number of stickers per edge = Cube's level
	RubikCube(GLint positionShaderAttribute, GLint normalShaderAttribute, unsigned int numStickersEdge = 3);
	RubikCube(GLint positionShaderAttribute, GLint normalShaderAttribute, const std::string& filepath);
	~RubikCube();

	RubikCube(RubikCube&& r);
	RubikCube& operator=(RubikCube&& r);

	RubikCube(const RubikCube&) = delete;
	RubikCube& operator=(const RubikCube&) = delete;

	// Number of stickers per edge = Cube's level
	inline unsigned int GetNumStickersPerEdge() const { return m_faces[0].size(); }
	inline bool IsRotating() const { return m_rotationType != NONE; }

	// Rotate one of the cube's faces
	// May throw an exception if rotationIndex is greater than GetNumStickersPerEdge()
	// Return false if the cube is unavailable (rotating), true if rotation started performing succesfully
	bool Rotate(RotationType rotationType, unsigned int rotationIndex, bool rotationClockwise);

	void Update(float deltaTime);

	// Create new cube with given number of stickers per edge
	void NewCube(unsigned int numStickersEdge = 3);

	void LoadFromFile(const std::string& filepath);
	void SaveIntoFile(const std::string& filepath) const;

	void Draw(const Camera& camera,
		const MatrixShaderUniforms& matrixUniforms,
		const MaterialShaderUniforms& materialUniforms) const;
};

#endif