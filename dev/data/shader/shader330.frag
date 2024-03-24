#include "glInclude.shader"

uniform vec3 vLightDir = vec3(1, -1, 1);
uniform vec3 vLightColor = vec3(1, 1, 1);

in vec3 v_normal;
in vec2 v_texcoord;

uniform sampler2D texDiffuse;
uniform sampler2D texNormal;

layout (location = 0) out vec4 color;

void main() {
	
	vec3 l = normalize(vLightDir);

	float diffuse = max(dot(l, v_normal), 0.0);
	vec3 tex0 = texture2D(texDiffuse, v_texcoord).rgb;
	vec3 tex1 = texture2D(texNormal, v_texcoord).rgb;

	vec3 tex = tex0 * tex1;
	
	color = vec4(v_texcoord.xy, 0, 1);
	//color = vec4(tex0, 1);
	//color = vec4(tex * vLightColor * diffuse, 1);
}
