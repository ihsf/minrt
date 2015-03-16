#include "EmbreeStuff.h"

__align(32) RTCScene EmbreeStuff::scene;

bool EmbreeStuff::compiledOnce = false;

EmbreeStuff::EmbreeStuff(){
}

EmbreeStuff::~EmbreeStuff(){
  rtcDeleteScene(scene);
  rtcExit();
}

void EmbreeStuff::init(){
  rtcInit(NULL);

  if (Engine::embreeIntersect == 4)
    scene = rtcNewScene(RTC_SCENE_STATIC, RTC_INTERSECT4); // Intel SSE: shoot packets of 4 rays
  else if (Engine::embreeIntersect == 8)
    scene = rtcNewScene(RTC_SCENE_STATIC, RTC_INTERSECT8); // Intel AVX: shoot packets of 8 rays
  else {
    cout << "Invalid value " << Engine::embreeIntersect << " for Engine::embreeIntersect" << endl;
    exit(1);
  }
}

void EmbreeStuff::addSampleMesh1(){
  RTCGeometryFlags geomFlags = RTC_GEOMETRY_STATIC;
  int numTriangles = 8;
  int numVertices = 32;

  unsigned geomID = rtcNewTriangleMesh(scene, geomFlags, numTriangles, numVertices, 1);

  float localVertices[8 * 4];

  //                      x						           y					                 z						              bogus       
  localVertices[0] = -37.604755f;  localVertices[1] = 32.398792f;  localVertices[2] = 35.277058f;   localVertices[3] = 0;
  localVertices[4] = 37.695309f;   localVertices[5] = 32.398792f;  localVertices[6] = 35.277058f;   localVertices[7] = 0;
  localVertices[8] = -37.604755f;  localVertices[9] = 32.398792f;  localVertices[10] = -35.795361f; localVertices[11] = 0;
  localVertices[12] = 37.695309f;  localVertices[13] = 32.398792f; localVertices[14] = -35.795361f; localVertices[15] = 0;

  localVertices[16] = -37.604755f; localVertices[17] = 0.317234f;  localVertices[18] = 35.277061f;  localVertices[19] = 0;
  localVertices[20] = 37.695309f;  localVertices[21] = 0.317234f;  localVertices[22] = 35.277061f;  localVertices[23] = 0;
  localVertices[24] = -37.604755f; localVertices[25] = 0.317238f;  localVertices[26] = -35.795357f; localVertices[27] = 0;
  localVertices[28] = 37.695309f;  localVertices[29] = 0.317238f;  localVertices[30] = -35.795357f; localVertices[31] = 0;

  Vertex* vertices = (Vertex*)rtcMapBuffer(scene, geomID, RTC_VERTEX_BUFFER);
  for (int i = 0; i < numVertices; i += 4){
    vertices[i / 4].x = localVertices[i];
    vertices[i / 4].y = localVertices[i + 1];
    vertices[i / 4].z = localVertices[i + 2];
    vertices[i / 4].a = 0;
  }
  rtcUnmapBuffer(scene, geomID, RTC_VERTEX_BUFFER);

  int indices[8 * 4];
  indices[0] = 1; indices[1] = 0; indices[2] = 5; indices[3] = 0;
  indices[4] = 4; indices[5] = 5; indices[6] = 0; indices[7] = 0;
  indices[8] = 3; indices[9] = 1; indices[10] = 7; indices[11] = 0;
  indices[12] = 5; indices[13] = 7; indices[14] = 1; indices[15] = 0;

  indices[16] = 2; indices[17] = 3; indices[18] = 6; indices[19] = 0;
  indices[20] = 7; indices[21] = 6; indices[22] = 3; indices[23] = 0;
  indices[24] = 0; indices[25] = 2; indices[26] = 4; indices[27] = 0;
  indices[28] = 6; indices[29] = 4; indices[30] = 2; indices[31] = 0;

  rtcUnmapBuffer(scene, geomID, RTC_VERTEX_BUFFER);

  Triangle* triangles = (Triangle*)rtcMapBuffer(scene, geomID, RTC_INDEX_BUFFER);
  for (int i = 0; i < numTriangles; i++){
    triangles[i].v0 = indices[i * 4];
    triangles[i].v1 = indices[i * 4 + 1];
    triangles[i].v2 = indices[i * 4 + 2];
  }
  rtcUnmapBuffer(scene, geomID, RTC_INDEX_BUFFER);
}


void EmbreeStuff::addSampleMesh2(){
  RTCGeometryFlags geomFlags = RTC_GEOMETRY_STATIC;

  int numTriangles = 2;
  int numVertices = 16;

  unsigned geomID = rtcNewTriangleMesh(scene, geomFlags, numTriangles, numVertices, 1);

  float localVertices[4 * 4];

  //                      x						           y					                 z						              bogus     
  localVertices[0] = -37.604755f; localVertices[1] = 0.317234f; localVertices[2] = 35.277061f;  localVertices[3] = 0;
  localVertices[4] = 37.695309f;  localVertices[5] = 0.317234f; localVertices[6] = 35.277061f;  localVertices[7] = 0;
  localVertices[8] = -37.604755f; localVertices[9] = 0.317238f; localVertices[10] = -35.795357f; localVertices[11] = 0;
  localVertices[12] = 37.695309f;  localVertices[13] = 0.317238f; localVertices[14] = -35.795357f; localVertices[15] = 0;

  Vertex* vertices = (Vertex*)rtcMapBuffer(scene, geomID, RTC_VERTEX_BUFFER);
  for (int i = 0; i < numVertices; i += 4){
    vertices[i / 4].x = localVertices[i];
    vertices[i / 4].y = localVertices[i + 1];
    vertices[i / 4].z = localVertices[i + 2];
    vertices[i / 4].a = 0;
  }
  rtcUnmapBuffer(scene, geomID, RTC_VERTEX_BUFFER);

  int indices[2 * 4];
  indices[0] = 1; indices[1] = 0; indices[2] = 3; indices[3] = 0;
  indices[4] = 2; indices[5] = 3; indices[6] = 0; indices[7] = 0;

  Triangle* triangles = (Triangle*)rtcMapBuffer(scene, geomID, RTC_INDEX_BUFFER);
  for (int i = 0; i < numTriangles; i++){
    triangles[i].v0 = indices[i * 4];
    triangles[i].v1 = indices[i * 4 + 1];
    triangles[i].v2 = indices[i * 4 + 2];
  }
  rtcUnmapBuffer(scene, geomID, RTC_INDEX_BUFFER);
}

void EmbreeStuff::addSampleMesh3(){
  RTCGeometryFlags geomFlags = RTC_GEOMETRY_STATIC;

  int numTriangles = (14904 / 3);
  int numVertices = 14904 * 4;

  unsigned geomID = rtcNewTriangleMesh(scene, geomFlags, numTriangles, numVertices, 1);

  Vertex* vertices = (Vertex*)rtcMapBuffer(scene, geomID, RTC_VERTEX_BUFFER);
  for (int i = 0; i < numVertices; i += 4){
    vertices[i / 4].x = meshVertices[i];
    vertices[i / 4].y = meshVertices[i + 1];
    vertices[i / 4].z = meshVertices[i + 2];
    vertices[i / 4].a = 0;
  }


  Triangle* triangles = (Triangle*)rtcMapBuffer(scene, geomID, RTC_INDEX_BUFFER);
  for (int i = 0; i < numTriangles; i++){
    triangles[i].v0 = meshTriangles[i * 3];
    triangles[i].v1 = meshTriangles[i * 3 + 1];
    triangles[i].v2 = meshTriangles[i * 3 + 2];
  }
  rtcUnmapBuffer(scene, geomID, RTC_VERTEX_BUFFER);
  rtcUnmapBuffer(scene, geomID, RTC_INDEX_BUFFER);
}


void EmbreeStuff::compile(){
  if (!compiledOnce){
    rtcCommit(scene);
    compiledOnce = true;
  }
}

