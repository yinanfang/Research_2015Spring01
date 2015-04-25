#include "renderStaticObjectInRift.h"
#include "oculusRender.h"
#include "ShaderHelper.h"

int keyMode = 0;
int frameCount = 0, currentTime = 0, previousTime = 0;
float fps = 0;

float globalRotx = 0;
float globalRoty = 0;
float globalRotz = 0;

void initializeglut(int argc, char** argv) {
  glutInit( &argc, argv );
  glutInitWindowSize( RIFT_WIN_COLS, RIFT_WIN_ROWS );
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
  glutInitWindowPosition( 120, 100 );
  glutCreateWindow ( "HMD_BASED_TELEPRESENCE" );
}

void calculateFPS() {
  frameCount++;
  currentTime = glutGet(GLUT_ELAPSED_TIME);
  int timeInterval = currentTime - previousTime;
  if(timeInterval > 1000) {
    fps = frameCount / (timeInterval / 1000.0f);
    previousTime = currentTime;
    frameCount = 0;
  }
}

void resize( int w, int h ) {
  if (h == 0) 
    {
      h = 1;
    }

  glutPostRedisplay();
}

void printTimeTakenByFucntion() {
    previousTime = glutGet(GLUT_ELAPSED_TIME);
    renderStaticObjectInRift(0);
    currentTime = glutGet(GLUT_ELAPSED_TIME);
    printf("Time elapsed: %d ms \n", currentTime - previousTime);
}

void display() {
  calculateFPS();

  oculusBeginRendering();
  updateRiftTracking();
   
  SceneNumber = (glutGet(GLUT_ELAPSED_TIME)/30)%NUM_BUFFER;
  // printf("SceneNumber: %d\n", SceneNumber);
  //SceneNumber = SceneNumber++ % NUM_BUFFER;
  //printf("SceneNumber: %d\n", SceneNumber);

  for(int ieye = 0; ieye < 2; ieye++) {
    pushGLMatricesForEye(ieye);
    renderStaticObjectInRift(SceneNumber);
    popGLMatricesForEye();
  }
  oculusEndRendering();

  glutSwapBuffers();
}

void key(unsigned char k, int x, int y) {
  if(k == 'm') {
    keyMode = (++keyMode)%3; 
    printf("Key entry mode = %d \n", keyMode);
  }

  switch(k) {
  case 27: exit(0);
  case 'f': printf("fps: %f \n", fps); break;
  default:{
    switch(keyMode){
    case 0:{
      switch(k){
      case 'a': globalRotx -= 1; printf("globalRotx = %f \n", globalRotx); break;  
      case 's': globalRotx += 1; printf("globalRotx = %f \n", globalRotx); break; 
      case 'd': globalRoty -= 1; printf("globalRoty = %f \n", globalRoty); break; 
      case 'f': globalRoty += 1; printf("globalRoty = %f \n", globalRoty); break; 
      case 'g': globalRotz -= 1; printf("globalRotz = %f \n", globalRotz); break; 
      case 'h': globalRotz += 1; printf("globalRotz = %f \n", globalRotz); break; 
      default: break;
      }
      break;
    }
    case 1:{
      key_renderStaticObjectInRift(k, x, y);
      break;
    }
    case 2: {
      key_oculusRender(k, x, y);
      break;
    }
    default: break;
    }
    break;
  }
  }

  glutPostRedisplay();
}

int main(int argc, char* argv[]) {
  initializeglut(argc, argv);
  GLenum err = glewInit();
  if ( err != GLEW_OK ) {
    printf( "Failed to init GLEW.\n");
    return -1;
  }

  initOculus();
  initOculusRender();
  initShaderHelper();
  initStaticObjectRenderer();

  glutDisplayFunc( display );
  glutIdleFunc( display );
  glutKeyboardFunc( key );
  glutReshapeFunc( resize );

  getRidOfHealthWarning();
  glutMainLoop( );
  return 0;
}
