uniform float waitRadsPerSecond;

out vec2 UVskull;
out vec2 UVwait;

void main()
{
	gl_Position = modelViewProjectionMatrix * vec4( vertex, 1 );

	const vec2 translation = vec2( 0.5f, 0.5f );

	const float waitAngle = Timer.time * waitRadsPerSecond / 1000;

	const float waitAngleCos = cos( waitAngle );
	const float waitAngleSin = sin( waitAngle );

	UVskull = texcoord;

	UVwait = ( mat2( waitAngleCos, waitAngleSin, -waitAngleSin, waitAngleCos  ) * ( texcoord - translation ) ) + translation;
}