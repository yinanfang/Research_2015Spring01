#include "commonFunctions.h"

float deg2rad(float theta) {
  return (PI * theta / 180);
}

float rad2deg(float radAngle) {
  return (radAngle/PI*180.0);
}

void printGLMatrix(GLenum mat) {
    GLfloat matToRet[16];
    Mat temp;
    glGetFloatv(mat, matToRet);
    temp = Mat(4, 4, CV_32F, matToRet).clone();
    cout << fixed << temp << endl << endl;
}

