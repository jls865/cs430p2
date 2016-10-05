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

double sqr(double v) {
  return v*v;
}

double dot_product(double* v1, double* v2, int n){
    double result = 0.0;
    int i;
    for(i=0;i<n;i++){
        result += v1[i]*v2[i];
    }
    return result;
}
//hello

void normalize(double* v) {
  double len = sqrt(sqr(v[0]) + sqr(v[1]) + sqr(v[2]));
  v[0] /= len;
  v[1] /= len;
  v[2] /= len;
}

int writePPM(FILE* destination, ppm_data* data, int width, int height){
    FILE* fh = fopen(destination, "w");
    fprintf(fh, "P6 %d %d 255\n", width, height);
    fwrite(data, sizeof(ppm_data), width*height, fh);
    fclose(fh);
}

double sphere_intersection(double* v1, double* v2, double* C, double r){
    double x,y,z,quad,dist1,dist2;
    x = sqr(v2[0])+sqr(v2[1])+sqr(v2[2]);
    y = (2 * (v1[0] * v2[0] - v2[0] * C[0] + v1[1] * v2[1] - v2[1] * C[1] + v1[2] * v2[2] - v2[2] * C[2]));
	z = sqr(v1[0]) - 2*v1[0]*C[0] + sqr(C[0]) + sqr(v1[1]) - 2*v1[1]*C[1] + sqr(C[1])
        + sqr(v1[2]) - 2*v1[2]*C[2] + sqr(C[2]) - sqr(r);

    quad = sqr(y) - 4*x*z;
    if(quad <0){
        return -1;
    }
    quad = sqrt(quad);

    dist1 =(-y - quad)/(2*x);
    if(dist1>0){
        return dist1;
    }
    dist2 =(-y +quad)/(2*x);
    if(dist2>0){
        return dist2;
    }
    else{return -1;}







}

double plane_intersection(double* v1, double* v2, double* C, double* n){
    normalize(n);
    double d, v,distance;
    d= dot_product(n,v2, 3);
    if(d == 0){
        return -1;
    }
    v = -(dot_product(n,v1,3))+sqrt(sqr(C[0]-v1[0])+ sqr(C[1]-v1[1])+ sqr(C[2]-v1[2]));

    distance = v/d;
    if(distance>0){
        return distance;
    }else{return  -1;}

}


void do_raycast(int width, int height;){
    ppm_data current;
    ppm_data* ptr = buffer;
    double x,y,z;
    int height, width, i,j;

    double v1[3] = {0,0,0};//makes the origin array
    double v2[3] = {0,0,0};//makes the direction array
    double v3[3] = {0,0,1};// makes the array we will add on to the z is +1 away from camera

    //makes the current all 0 so if its not changed it stays 0
    current.r = 0;
    current.g = 0;
    current.b = 0;

    //sets cameras values because its position at 0,0,0, z +1 is accounted for in v3;
    x=0;
    y=0;
    z=0;
    //sets numbers so that it can loop width x height times(i x j)
    i = width;
    j = height;

    //get the pixel size by dividing camera width and height by inputs width and height
    

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
	//writePPM(argv[4],)
	return 0;


}



