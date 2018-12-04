//arrays, buffers, size
#define asize(array) sizeof(array)/(sizeof(array[0]))
#define buffer_offset(i) ((void*)(i))
#define kiloBytes(value) value * 1024
#define megaBytes(value) value * kiloBytes(1024)
#define gigaBytes(value) value * megaBytes(1024)
#define teraBytes(value) value * gigaBytes(1024)

//colors
#define RED vec4(1, 0, 0, 1)
#define GREEN vec4(0, 1, 0, 1)
#define BLUE vec4(0, 0, 1, 1)
#define TRAN vec4(0.97f, 0.83f, 0.17, 1.0f)
//vectors
#define vec3_zero vec3(0, 0, 0)
#define vec3_up vec3(0, 1, 0)
#define vec3_right vec3(1, 0, 0)
#define vec3_forward vec3(0, 0, -1)
#define vec2_zero vec2(0, 0)
#define fvec2_zero fvec2(0, 0, 0)

//math functions
#define rand_range() (double)rand() / (double)RAND_MAX;
#define degrees_to_radians(angle_degrees) (angle_degrees * M_PI / 180.0)
#define clip(n, lower, upper) if (n < lower) n= lower; else if (n > upper) n= upper
