#include "lvl5_math.h"

typedef struct {
  f32 samples[16];
} Spectrum;

#define LIGHT_SPEED 299472458.0f
#define PLANCK 6.626e-34f

f32 get_energy(f32 wavelength) {
  f32 result = LIGHT_SPEED*PLANCK/wavelength;
  return result;
}

f32 get_flux(f32 energy, f32 time) {
  f32 result = energy/time;
  return result;
}

f32 get_irradience(f32 flux, f32 area) {
  f32 result = flux/area;
  return result;
}

f32 get_intensity(f32 flux, f32 solid_angle) {
  f32 result = flux/solid_angle;
  return result;
}

typedef struct {
  v3 p;
  v3 d;
} Ray;

typedef struct {
  v3 p;
  f32 r;
} Sphere;

typedef struct {
  v3 n;
  f32 d;
} Plane;

typedef enum {
  Shape_Type_PLANE,
  Shape_Type_SPHERE,
} Shape_Type;

typedef struct {
  union {
    Plane plane;
    Sphere sphere;
  };
  i32 mat;
  Shape_Type type;
} Shape;

typedef struct {
  v3 emit_color;
  v3 reflect_color;
  f32 scatter;
} Material;

typedef struct {
  Material *materials;
  i32 material_count;
  
  Shape *shapes;
  i32 shape_count;
} World;
