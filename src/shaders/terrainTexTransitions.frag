#version 130

in vec2 UV;
in float alpha;

out vec4 color;

uniform sampler2D myTextureSampler;

void main(){
	color.rgb = texture2D( myTextureSampler, UV ).rgb;

	color.a = alpha;

  if (color.a < 0)
    color.a = 0;
  else if (color.a > 1)
    color.a = 1;
}
