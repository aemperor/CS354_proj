
attribute vec2 parametric;

uniform vec3 lightPosition;  // Object-space
uniform vec3 eyePosition;    // Object-space
uniform vec2 torusInfo;

varying vec2 normalMapTexCoord;

varying vec3 lightDirection;
varying vec3 halfAngle;
varying vec3 eyeDirection;
varying vec3 c0, c1, c2;

void main()
{
  const float pi2 = 6.28318530;  // 2 times Pi
  normalMapTexCoord = vec2(-6.0*parametric[0], -2.0*parametric[1]);

  float M = torusInfo[0];
  float N = torusInfo[1];
  
  float coeff = M + N*cos(parametric[1]*pi2);
  float y = coeff*sin(parametric[0]*pi2);
  float x = coeff*cos(parametric[0]*pi2);
  float z = N*sin(parametric[1]*pi2);
  gl_Position = gl_ModelViewProjectionMatrix * vec4(x, y, z, 1);
  
  eyeDirection = vec3(0);  // XXX fix me
  lightDirection = vec3(0);  // XXX fix me
  halfAngle = vec3(0);  // XXX fix me
  c0 = vec3(0);  // XXX fix me
  c1 = vec3(0);  // XXX fix me
  c2 = vec3(0);  // XXX fix me
}

