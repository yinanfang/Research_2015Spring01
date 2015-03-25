#include "ShaderHelper.h"

GLuint shaderProgram;
mat4 projection, modelview, camera;
mat4 offsetModelview;
int screenWidth = 800, screenHeight = 800;
extern float transx ,transy ,transz ,rotx ,roty ,rotz ,sclx ,scly ,sclz; 


mat4 getProjectionMatrix(double fov, double ar, double n, double f) {
  double halfFOV = 0.5 * fov * (M_PI / 180.f);
  double foc = cos(halfFOV) / sin(halfFOV);
  return mat4(
	      vec4(foc/ar, 0, 0, 0),
	      vec4(0, foc, 0, 0),
	      vec4(0, 0, (f+n)/(n-f), (2*n*f)/(n-f)),
	      vec4(0, 0, -1, 0)
	      );
}

void initShaderHelper() {

  shaderProgram = createShaderProgram("D:/Lucas/oculusHiballDemo/renderAllContentInRift/shader.vert", 
  				"D:/Lucas/oculusHiballDemo/renderAllContentInRift/shader.frag");

  projection = getProjectionMatrix(60.0, (1.0*screenWidth)/screenHeight, 1.0, 10000.0);

#if PRINT_PROJECTION_MATRIX
  printf("Initial projection matrix \n");
  printMat4(projection);
#endif

  camera = identity3D();
  modelview = translation3D(vec3(transx, transy, transz)) * rotation3D(vec3(1,0,0), rotx) * rotation3D(vec3(0,1,0), roty) * rotation3D(vec3(0,0,1), rotz)* scaling3D(vec3(0.07));

#if PRINT_MODELVIEW_MATRIX
  printf("Initial modelview matrix \n");
  printMat4(modelview);
#endif
	

}
