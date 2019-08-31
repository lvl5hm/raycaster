#include "ray.h"
#include "bmp.h"
#include "lvl5_random.h"
#include <stdlib.h>

#define ERR 0.0001f

Rand global_rand;

typedef struct {
  v3 n;
  f32 t;
  b32 success;
} Ray_Hit;

Ray_Hit ray_intersect_sphere(Ray ray, Sphere sphere) {
  Ray_Hit result = {0};
  
  v3 rel_p = v3_sub(ray.p, sphere.p);
  
  f32 a = v3_dot(ray.d, ray.d);
  f32 b = v3_dot(rel_p, ray.d)*2;
  f32 c = v3_dot(rel_p, rel_p) - sqr_f32(sphere.r);
  
  f32 root_term = sqrt_f32(b*b - 4*a*c);
  if (root_term > ERR) {
    f32 t0 = (-b - root_term)/(2*a);
    f32 t1 = (-b + root_term)/(2*a);
    
    if (t0 > ERR && t0 < t1) {
      result.t = t0;
      result.success = true;
    } else if (t1 > ERR && t1 < t0) {
      result.t = t1;
      result.success = true;
    }
    
    v3 hit_p = v3_add(ray.p, v3_mul(ray.d, result.t));
    result.n = v3_unit(v3_sub(hit_p, sphere.p));
  }
  
  return result;
}

Ray_Hit ray_intersect_plane(Ray ray, Plane plane) {
  Ray_Hit result = {0};
  
  f32 denom = v3_dot(ray.d, plane.n);
  if (abs_f32(denom) > ERR) {
    f32 t = (-v3_dot(ray.p, plane.n) - plane.d)/denom;
    if (t > ERR) {
      result.t = t;
      result.n = plane.n;
      result.success = true;
    }
  }
  
  return result;
}


v3 raycast(World *world, Ray ray) {
  v3 result = V3(0, 0, 0);
  v3 attenuation = V3(1, 1, 1);
  
  for (i32 bounce_index = 0; bounce_index < 5; bounce_index++) {
    i32 hit_mat_index = 0;
    Ray_Hit hit = {0};
    hit.t = INFINITY;
    
    for (i32 shape_index = 0; shape_index < world->shape_count; shape_index++) {
      Shape shape = world->shapes[shape_index];
      
      Ray_Hit cur_hit = {0};
      switch (shape.type) {
        case Shape_Type_SPHERE:
        cur_hit = ray_intersect_sphere(ray, shape.sphere);
        break;
        
        case Shape_Type_PLANE:
        cur_hit = ray_intersect_plane(ray, shape.plane);
        break;
      }
      if (cur_hit.success && cur_hit.t < hit.t) {
        hit_mat_index = shape.mat;
        hit = cur_hit;
      }
    }
    
    if (hit_mat_index) {
      Material mat = world->materials[hit_mat_index];
      result = v3_add(result, v3_hadamard(mat.emit_color, attenuation));
      
      f32 cos_atten = v3_dot(v3_negate(ray.d), hit.n);
      if (cos_atten < 0) {
        cos_atten = 0;
      }
      attenuation = v3_hadamard(attenuation, v3_mul(mat.reflect_color, cos_atten));
      
      v3 pure_bounce = v3_sub(ray.d, v3_mul(hit.n, 2*v3_dot(hit.n, ray.d)));
      v3 random_bounce = v3_unit(v3_add(hit.n, V3(random_bilateral(&global_rand),
                                                  random_bilateral(&global_rand),
                                                  random_bilateral(&global_rand))));
      
      ray.p = v3_add(ray.p, v3_mul(ray.d, hit.t));
      ray.d = lerp_v3(random_bounce, pure_bounce, 
                      V3(mat.scatter, mat.scatter, mat.scatter));
    } else {
      Material mat = world->materials[hit_mat_index];
      result = v3_add(result, v3_hadamard(mat.emit_color, attenuation));
      break;
    }
  }
  
  return result;
}


typedef union {
  struct {
    f32 xyz;
    f32 xy;
    f32 xz;
    f32 yz;
  };
  struct {
    f32 inner;
    v3 outer;
  };
} Rotor3;

v3 v3_wedge(v3 a, v3 b) {
  v3 result;
  result.x = a.x*b.y - b.x*a.y;
  result.y = a.x*b.z - b.x*a.z;
  result.z = a.y*b.z - b.y*a.z;
  return result;
}

#if 0
v4 v3_geometric(v3 a, v3 b) {
  v4 result;
  result.inner = v3_dot(a, b);
  result.outer = v3_wedge(a, b);
  return result;
}
#endif

Rotor3 rotor3(v3 from, v3 to) {
  Rotor3 result;
  result.inner = v3_dot(from, to);
  result.outer = v3_wedge(from, to);
  return result;
}

#if 0
v3 v3_rotate(v3 v, Rotor3 rot) {
  v4 q;
  q.xyz = -v.x*rot.yz + v.y*rot.xz - v.z*rot.xy;
  q.xy = rot.xyz*v.x
    q.xz = rot.xyz*v.y
    q.yz = rot.xyz*v.z
  
    v3 result = {0};
  return result;
}
#endif

int main() {
  u8 foo = 150;
  
  return 0;
  
  
  
  printf("running raytracer...\n");
  
  global_rand = make_random_sequence(432454564);
  
  Material materials[] = {
    (Material){
      .emit_color = V3(0.8f, 0.8f, 1.0f),
    },
    (Material){
      .reflect_color = V3(1, 0.4f, 0.3f),
      .scatter = 0.6f,
    },
    (Material){
      .reflect_color = V3(0.95f, 0.95f, 0.95f),
      .scatter = 1.0f,
    },
    (Material){
      .reflect_color = V3(0.4f, 0.8f, 0.3f),
      .scatter = 0.8f,
    },
  };
  
  Shape shapes[] = {
    (Shape){
      .sphere = (Sphere){
        .p = V3(-0.1f, 0, 1.1f),
        .r = 1.0f,
      },
      .mat = 2,
      .type = Shape_Type_SPHERE,
    },
    (Shape){
      .sphere = (Sphere){
        .p = V3(3, 0, 1.0f),
        .r = 2.0f,
      },
      .mat = 3,
      .type = Shape_Type_SPHERE,
    },
    (Shape){
      .plane = (Plane){
        .n = V3(0, 0, 1),
        .d = 0,
      },
      .mat = 1,
      .type = Shape_Type_PLANE,
    },
  };
  
  World world;
  world.materials = materials;
  world.material_count = array_count(materials);
  world.shapes = shapes;
  world.shape_count = array_count(shapes);
  
  v3 global_z = V3(0, 0, 1);
  
  v3 camera_p = V3(0, 10, 1);
  v3 camera_z = v3_unit(camera_p);
  v3 camera_x = v3_unit(v3_cross(camera_z, global_z));
  v3 camera_y = v3_unit(v3_cross(camera_x, camera_z));
  
  Bitmap bmp = {0};
  bmp.width = 1366;
  bmp.height = 768;
  bmp.data = malloc(bmp.width*bmp.height*4);
  f32 bmp_ratio = (f32)bmp.width/(f32)bmp.height;
  
  f32 film_w = 0.2f;
  f32 film_h = film_w/bmp_ratio;
  f32 film_d = -0.2f;
  
  
  for (i32 y = 0; y < bmp.height; y++) {
    for (i32 x = 0; x < bmp.width; x++) {
      i32 ray_count = 64;
      f32 ray_contrib = 1/(f32)ray_count;
      
      v3 color = V3(0, 0, 0);
      for (i32 ray_index = 0; ray_index < ray_count; ray_index++) {
        f32 jitter = 0.001f;
        f32 film_x = (f32)x/(f32)bmp.width*2 - 1 + jitter*random_bilateral(&global_rand);
        f32 film_y = (f32)y/(f32)bmp.height*2 - 1 + jitter*random_bilateral(&global_rand);
        
        v3 film_p = v3_add(v3_mul(camera_x, film_x*film_w),
                           v3_add(v3_mul(camera_y, film_y*film_h),
                                  v3_mul(camera_z, film_d)));
        Ray ray;
        ray.p = camera_p;
        ray.d = v3_unit(film_p);
        
        
        v3 ray_color = raycast(&world, ray);
        color = v3_add(color, v3_mul(ray_color, ray_contrib));
      }
      
      v4 srgb = color_linear_to_srgb(v3_to_v4(color, 1.0f));
      u32 color_u32 = color_v4_to_u32(srgb);
      ((u32 *)bmp.data)[y*bmp.width + x] = color_u32;
    }
    printf("progress: %0.2f\n", (f32)y/bmp.width);
  }
  
  save_bmp("test.bmp", bmp);
  
  return 0;
}
