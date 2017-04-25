#ifndef LIGHT_SHADER_UNIFORMS_H
#define LIGHT_SHADER_UNIFORMS_H

#include <GL/freeglut.h>

struct LightShaderUniforms {
	GLint positionUniform;
	GLint ambientColorUniform;
	GLint diffuseColorUniform;
	GLint specularColorUniform;

	LightShaderUniforms(GLint position, GLint ambientColor, GLint diffuseColor, GLint specularColor)
		: positionUniform(position),
		ambientColorUniform(ambientColor),
		diffuseColorUniform(diffuseColor),
		specularColorUniform(specularColor)
	{}

	LightShaderUniforms() : LightShaderUniforms(-1, -1, -1, -1) {}
};

#endif