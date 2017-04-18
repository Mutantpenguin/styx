uniform sampler2D bgTexture;

in vec2 UVbg;

out vec4 color;

void main()
{
	color = texture2D( bgTexture, UVbg );
}