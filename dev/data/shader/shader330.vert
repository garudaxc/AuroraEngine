

#include "glInclude.shader"

uniform mat4 mWorld;
uniform mat4 mView;
uniform mat4 mProj;

in vec4 vPosition;
in vec3 vNormal;
in vec2 vUV;

out vec3 v_normal;
out vec2 v_texcoord;
void main() {

	vec4 worldPos = mWorld * vPosition;
	vec4 viewPos = mView * worldPos;
	gl_Position = mProj * viewPos;

	v_normal = mat3(mWorld) * vNormal;
	v_texcoord = vUV;
}

