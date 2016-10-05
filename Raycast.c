#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
// Polymorphism in C
//Creates the 3 objects using a parent object struct.
typedef struct {
  int type; // 0 = plane, 1 = sphere, 2 = camera
  double color[3];//color vector;
  double height;//height of the view plane read in on command
  double width;//width of the view plane read in on cmd
  union {
    struct {
      double* position;
      double* color;
      double* normal;
    } plane;
    struct {
      double* position;
      double radius;
      double* color;
    } sphere;
    struct {
      double width;
      double height;
    } camera;
  };
} Object;


typedef struct{
unsigned char r,g,b;

}ppm_data;

int line = 1;
Object object[128];

static inline double sqr(double v) {
  return v*v;
}

double dot_product(double* v1, double* v2, int n){
    double result = 0.0;
    int i;
    for(i=0;i<n;i++){
        result += v1[i]*v2[i];
        return result;
    }
}
//hello

static inline void normalize(double* v) {
  double len = sqrt(sqr(v[0]) + sqr(v[1]) + sqr(v[2]));
  v[0] /= len;
  v[1] /= len;
  v[2] /= len;
}

int writePPM(FILE* destination, ppm_data* data, int width, int height){
    FILE* fh = fopen(destination, "w");
    fprintf(fh, "P6 %d %d 255\n", width, height);
    fwrite(buffer, sizeof(Pixel), width*height, fh);
    fclose(fh);
}

double sphere_intersection(double* Ro, double* Rd, double* C, double r){


}

double* plane_intersection(double* v1, double* v2, double* C, double* n){
    double distance;
    double v1v2[3];
    v1v2[3] = v2-v1;
    double A = dot_product(n, v1, 3);
    double B = dot_product(n, v1v2, 3);
    //distance = dot_product(v1v2,n,3)/dot_product(v1,n,3);
//    return A+(((distance-A)/B)*v1v2);
return 1.0;
}


// next_c() wraps the getc() function and provides error checking and line
// number maintenance
int next_c(FILE* json) {
  int c = fgetc(json);
#ifdef DEBUG
  printf("next_c: '%c'\n", c);
#endif
  if (c == '\n') {
    line += 1;
  }
  if (c == EOF) {
    fprintf(stderr, "Error: Unexpected end of file on line number %d.\n", line);
    exit(1);
  }
  return c;
}


// expect_c() checks that the next character is d.  If it is not it emits
// an error.
void expect_c(FILE* json, int d) {
  int c = next_c(json);
  if (c == d) return;
  fprintf(stderr, "Error: Expected '%c' on line %d.\n", d, line);
  exit(1);
}


// skip_ws() skips white space in the file.
void skip_ws(FILE* json) {
  int c = next_c(json);
  while (isspace(c)) {
    c = next_c(json);
  }
  ungetc(c, json);
}


// next_string() gets the next string from the file handle and emits an error
// if a string can not be obtained.
char* next_string(FILE* json) {
  char buffer[129];
  int c = next_c(json);
  if (c != '"') {
    fprintf(stderr, "Error: Expected string on line %d.\n", line);
    exit(1);
  }
  c = next_c(json);
  int i = 0;
  while (c != '"') {
    if (i >= 128) {
      fprintf(stderr, "Error: Strings longer than 128 characters in length are not supported.\n");
      exit(1);
    }
    if (c == '\\') {
      fprintf(stderr, "Error: Strings with escape codes are not supported.\n");
      exit(1);
    }
    if (c < 32 || c > 126) {
      fprintf(stderr, "Error: Strings may contain only ascii characters.\n");
      exit(1);
    }
    buffer[i] = c;
    i += 1;
    c = next_c(json);
  }
  buffer[i] = 0;
  return strdup(buffer);
}

double next_number(FILE* json) {
  double value;
  fscanf(json, "%lf", &value);
  // Error check this..
  return value;
}

double* next_vector(FILE* json) {
  double* v = malloc(3*sizeof(double));
  expect_c(json, '[');
  skip_ws(json);
  v[0] = next_number(json);
  skip_ws(json);
  expect_c(json, ',');
  skip_ws(json);
  v[1] = next_number(json);
  skip_ws(json);
  expect_c(json, ',');
  skip_ws(json);
  v[2] = next_number(json);
  skip_ws(json);
  expect_c(json, ']');
  return v;
}


void read_scene(char* filename) {
  int c;
  FILE* json = fopen(filename, "r");

  if (json == NULL) {
    fprintf(stderr, "Error: Could not open file \"%s\"\n", filename);
    exit(1);
  }

  skip_ws(json);

  // Find the beginning of the list
  expect_c(json, '[');

  skip_ws(json);

  // Find the objects
  int index = 0;
  while (1) {

    c = fgetc(json);
    if (c == ']') {
      fprintf(stderr, "Error: This is the worst scene file EVER.\n");
      fclose(json);
      return;
    }
    if (c == '{') {
      skip_ws(json);

      // Parse the object
      char* key = next_string(json);
      if (strcmp(key, "type") != 0) {
	fprintf(stderr, "Error: Expected \"type\" key on line number %d.\n", line);
	exit(1);
      }

      skip_ws(json);

      expect_c(json, ':');

      skip_ws(json);

      char* value = next_string(json);
      index++;
      if (strcmp(value, "camera") == 0) {
            object[index].type=2;
      } else if (strcmp(value, "sphere") == 0){
            object[index].type =1;
      } else if (strcmp(value, "plane") == 0) {
            object[index].type=0;
      } else {
	fprintf(stderr, "Error: Unknown type, \"%s\", on line number %d.\n", value, line);
	exit(1);
      }

      skip_ws(json);
      while (1) {
	// , }
	c = next_c(json);
	if (c == '}') {
	  // stop parsing this object
	  break;
	} else if (c == ',') {
	  // read another field
	  skip_ws(json);
	  char* key = next_string(json);
	  skip_ws(json);
	  expect_c(json, ':');
	  skip_ws(json);
	  if ((strcmp(key, "width") == 0) ||
	      (strcmp(key, "height") == 0) ||
	      (strcmp(key, "radius") == 0)) {
            double value = next_number(json);

            if(object[index].type ==1){
                object[index].sphere.radius = value;
            }
            else if(object[index].type == 2){

                if((strcmp(key, "width") == 0)){
                    object[index].camera.width = value;
                }

                else if(strcmp(key, "height") == 0){
                    object[index].camera.height = value;

                }else{

                    fprintf(stderr, "Not a valid variable for Camera\n");}

            }else{
                    fprintf(stderr, "Not a valid type of object for width height or radius\n");}
        }
        else if ((strcmp(key, "color") == 0) ||
		     (strcmp(key, "position") == 0) ||
		     (strcmp(key, "normal") == 0)) {
                double* value = next_vector(json);

                if(object[index].type == 0){
                    if(strcmp(key, "color") == 0){
                        object[index].plane.color = value;
                    }

                    else if(strcmp(key, "position") == 0){
                        object[index].plane.position = value;
                    }
                    else if(strcmp(key, "normal") == 0){
                        object[index].plane.normal = value;

                    }else{fprintf(stderr, "Not a valid variable for plane\n");}

                }else if(object[index].type == 1){

                    if(strcmp(key, "color")== 0){
                        object[index].sphere.color = value;
                    }
                    else if(strcmp(key, "position")== 0){
                        object[index].sphere.position = value;


                    }else{fprintf(stderr, "Not a valid variable for a sphere\n");}


	    } else {fprintf(stderr, "Not a valid type of object for color position or normal\n");}

	  } else {
	    fprintf(stderr, "Error: Unknown property, \"%s\", on line %d.\n",
		    key, line);
        //char* value = next_string(json);
	  }
	  skip_ws(json);
	} else {
	  fprintf(stderr, "Error: Unexpected value on line %d\n", line);
	  exit(1);
	}
      }
      skip_ws(json);
      c = next_c(json);
      if (c == ',') {
	// noop
	skip_ws(json);
      } else if (c == ']') {
	fclose(json);
	return;
      } else {
	fprintf(stderr, "Error: Expecting ',' or ']' on line %d.\n", line);
	exit(1);
      }
    }
  }
}



//double cylinder_intersection(double* Ro, double* Rd,
//			     double* C, double r) {
//  // Step 1. Find the equation for the object you are
//  // interested in..  (e.g., cylinder)
//  //
//  // x^2 + z^2 = r^2
//  //
//  // Step 2. Parameterize the equation with a center point
//  // if needed
//  //
//  // (x-Cx)^2 + (z-Cz)^2 = r^2
//  //
//  // Step 3. Substitute the eq for a ray into our object
//  // equation.
//  //
//  // (Rox + t*Rdx - Cx)^2 + (Roz + t*Rdz - Cz)^2 - r^2 = 0
//  //
//  // Step 4. Solve for t.
//  //
//  // Step 4a. Rewrite the equation (flatten).
//  //
//  // -r^2 +
//  // t^2 * Rdx^2 +
//  // t^2 * Rdz^2 +
//  // 2*t * Rox * Rdx -
//  // 2*t * Rdx * Cx +
//  // 2*t * Roz * Rdz -
//  // 2*t * Rdz * Cz +
//  // Rox^2 -
//  // 2*Rox*Cx +
//  // Cx^2 +
//  // Roz^2 -
//  // 2*Roz*Cz +
//  // Cz^2 = 0
//  //
//  // Steb 4b. Rewrite the equation in terms of t.
//  //
//  // t^2 * (Rdx^2 + Rdz^2) +
//  // t * (2 * (Rox * Rdx - Rdx * Cx + Roz * Rdz - Rdz * Cz)) +
//  // Rox^2 - 2*Rox*Cx + Cx^2 + Roz^2 - 2*Roz*Cz + Cz^2 - r^2 = 0
//  //
//  // Use the quadratic equation to solve for t..
//  double a = (sqr(Rd[0]) + sqr(Rd[2]));
//  double b = (2 * (Ro[0] * Rd[0] - Rd[0] * C[0] + Ro[2] * Rd[2] - Rd[2] * C[2]));
//  double c = sqr(Ro[0]) - 2*Ro[0]*C[0] + sqr(C[0]) + sqr(Ro[2]) - 2*Ro[2]*C[2] + sqr(C[2]) - sqr(r);
//
//  double det = sqr(b) - 4 * a * c;
//  if (det < 0) return -1;
//
//  det = sqrt(det);
//
//  double t0 = (-b - det) / (2*a);
//  if (t0 > 0) return t0;
//
//  double t1 = (-b + det) / (2*a);
//  if (t1 > 0) return t1;
//
//  return -1;
//}
//
////int main() {
////
//  double cx = 0;
//  double cy = 0;
//  double h = 0.7;
//  double w = 0.7;
//
//  int M = 20;
//  int N = 20;
//
//  double pixheight = h / M;
//  double pixwidth = w / N;
//  int y =0;
//  int x =0;
//  for (y=0; y < M; y += 1) {
//    for (x=0; x < N; x += 1) {
//      double Ro[3] = {0, 0, 0};
//      // Rd = normalize(P - Ro)
//      double Rd[3] = {
//	cx - (w/2) + pixwidth * (x + 0.5),
//	cy - (h/2) + pixheight * (y + 0.5),
//	1
//      };
//      normalize(Rd);
//
//      double best_t = INFINITY;
////
////	switch(objects[i]->kind) {
////	case 0:
////
////	  break;
////	default:
////	  // Horrible error
////	  exit(1);
////	}
////	if (t > 0 && t < best_t) best_t = t;
////      }
////      if (best_t > 0 && best_t != INFINITY) {
////	printf("#");
////      } else {
////	printf(".");
////	switch(objects[i]->kind) {
////	case 0:
////
////	  break;
////	default:
////	  // Horrible error
////	  exit(1);
////	}
////	if (t > 0 && t < best_t) best_t = t;
////      }
////      if (best_t > 0 && best_t != INFINITY) {
////	printf("#");
////      } else {
////	printf(".");
////      }
//
//    }
//    printf("\n");
//  }
//
//  return 0;
//}


int main(int argc, char** argv){
    Object scene[128];
    int width, height;
	if(argc!=5){
        fprintf(stderr, "Not valid input for program Raycast.c");
        return -1;
	}

	char* temp = argv[3];
	if(strstr(temp, ".json")!=0){
        fprintf(stderr, "Not valid input file type");
        return -1;
	}

	width = atoi(argv[1]);
	height = atoi(argv[2]);

	if(width <=0 || height <=0){
        fprintf(stderr, "Width and Height must be more than 0");
        return -1;
	}

	read_scene(temp);
	temp = argv[4];
	if(strstr(temp, ".ppm")!=0){
        fprintf(stderr, "Not valid output file type");
        return -1;
    }
	writePPM(argv[4])
	return 0;


}


