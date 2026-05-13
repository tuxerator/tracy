#ifndef HIT_RECORD_GLSL
#define HIT_RECORD_GLSL

struct HitRecord {
  float t;
  vec3 position;
  vec3 normal;
  bool frontFace;
  int materialIndex;
};

void hitRecord_setFaceNormal(inout HitRecord rec, vec3 rayDir, vec3 outwardNormal) {
  rec.frontFace = dot(rayDir, outwardNormal) < 0.0;
  rec.normal = rec.frontFace ? outwardNormal : -outwardNormal;
}

#endif
