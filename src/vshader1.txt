varying vec3 normal;
varying vec4 pos;
varying vec4 rawpos;

uniform float angle;

void main() {
  vec4 v = vec4(gl_Vertex);
  v.z = v.z + sin(2.0*v.x + angle)*0.2;

  normal = gl_NormalMatrix * gl_Normal;
  gl_Position = gl_ModelViewProjectionMatrix * v;
  pos = gl_ModelViewMatrix * v;
  rawpos = v;
  gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
}
