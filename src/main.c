#include <stdio.h>
#include <time.h>
#include <curses.h>
#include <math.h>
#include <stdlib.h>

#define DEFAULT_CHAR '#'
#define PI 3.141592654

#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2

void msleep(int msec)
{
    struct timespec ts;
    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

typedef struct {
   float x;
   float y;
   float z;
} Vector3;

typedef struct {
   float x;
   float y;
} Vector2;

typedef struct {
   int x;
   int y;
} Vector2i;

typedef struct {
  Vector3* verticies;
  int verticies_length;
  Vector2i* edges;
  int edges_length;
} Mesh;

typedef struct {
  float matrix[3][3];
} Matrix3;

void print_matrix3(Matrix3 matrix) {
  printf("{%f, %f, %f}\n", matrix.matrix[0][0], matrix.matrix[0][1], matrix.matrix[0][2]);
  printf("{%f, %f, %f}\n", matrix.matrix[1][0], matrix.matrix[1][1], matrix.matrix[1][2]);
  printf("{%f, %f, %f}\n", matrix.matrix[2][0], matrix.matrix[2][1], matrix.matrix[2][2]);
}

Matrix3 gen_scale_matrix(float x, float y, float z) {
  Matrix3 m;

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      m.matrix[i][j] = 0.0;
    }
  }
  m.matrix[0][0] = x;
  m.matrix[1][1] = y;
  m.matrix[2][2] = z;
  return m;
}

Matrix3 gen_rotation_matrix(float theta, int axis) {
  theta = (theta * PI)/180.0;
  
  Matrix3 m;
  float mat_x[3][3] = {
    {1.0, 0.0, 0.0},
    {0.0, cosf(theta), -sinf(theta)},
    {0.0, sinf(theta), cosf(theta)},
  };
  float mat_y[3][3] = {
    {cosf(theta), 0.0, sinf(theta)},
    {0.0, 1.0, 0.0},
    {-sinf(theta), 0.0, cosf(theta)},
  };
  float mat_z[3][3] = {
    {cosf(theta), -sinf(theta), 0.0},
    {sinf(theta), cosf(theta), 0.0},
    {0.0, 0.0, 1.0},
  };
  switch (axis) {
    case AXIS_X: {
      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
          m.matrix[j][i] = mat_x[j][i];
        }
      }
      return m;
      break;
    }
    case AXIS_Y: {
      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
          m.matrix[i][j] = mat_y[i][j];
        }
      }
      return m;
      break;
    }
    case AXIS_Z: {
      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
          m.matrix[i][j] = mat_z[i][j];
        }
      }
      return m;
      break;
    }
  }

  return m;
}
Vector3 apply_matrix_vector_to_number(Vector3 vector, float number) {
  Vector3 out = (Vector3){
    vector.x * number,
    vector.y * number,
    vector.z * number,
  };
  return out;
}

Vector3 add_vector3s(Vector3 vec1, Vector3 vec2) {
  Vector3 out;
  out.x = vec1.x + vec2.x;
  out.y = vec1.y + vec2.y;
  out.z = vec1.z + vec2.z;
  return out;
}

Vector3 apply_matrix_to_vertex(Vector3 vertex, Matrix3 matrix) {
  Vector3 col1 = (Vector3){matrix.matrix[0][0], matrix.matrix[0][1], matrix.matrix[0][2]};
  Vector3 col2 = (Vector3){matrix.matrix[1][0], matrix.matrix[1][1], matrix.matrix[1][2]};
  Vector3 col3 = (Vector3){matrix.matrix[2][0], matrix.matrix[2][1], matrix.matrix[2][2]};

  Vector3 column1 = apply_matrix_vector_to_number(col1, vertex.x);
  Vector3 column2 = apply_matrix_vector_to_number(col2, vertex.y);
  Vector3 column3 = apply_matrix_vector_to_number(col3, vertex.z);

  return add_vector3s(column1, add_vector3s(column2, column3));  
  
}

Mesh apply_matrix_to_mesh(Mesh mesh, Matrix3 matrix) {

  for (int i = 0; i < mesh.verticies_length; i++) {
    mesh.verticies[i] = apply_matrix_to_vertex(mesh.verticies[i], matrix);
  }

  return mesh;
}

Vector2 plot_3d(Vector3* point, int focal_length);
void drawline(Vector2* point0, Vector2* point1);

void render_mesh(Mesh* mesh, int focal_length) {
  Vector2 rendered_points[mesh->verticies_length];

  int x, y;
  
  getmaxyx(stdscr, y, x);

  float y_pos, x_pos;
  y_pos = (float)y;
  x_pos = (float)x;

  Vector3 vertex;
  for (int i = 0; i < mesh->verticies_length; i++) {
    vertex = (Vector3){
      mesh->verticies[i].x + y_pos/2.0,
      mesh->verticies[i].y + x_pos/2.0,
      mesh->verticies[i].z,
    };
    rendered_points[i] = plot_3d(&vertex, focal_length);

  }
  for (int i = 0; i < mesh->edges_length; i++) {
    drawline(&rendered_points[mesh->edges[i].x], &rendered_points[mesh->edges[i].y]);
  }
}

void print_mesh(Mesh mesh) {
  printf("Verticies:\n");
  for (int i = 0; i < mesh.verticies_length; i++) {
    printf("{%f, %f, %f},\n", mesh.verticies[i].x, mesh.verticies[i].y, mesh.verticies[i].z);
  }
  
}

void plot(Vector2 p) {
   mvaddch(p.x, p.y, DEFAULT_CHAR);
}

void drawline(Vector2* point0, Vector2 *point1) { 
   int X0 = (int)point0->x, Y0 = (int)point0->y;
   int X1 = (int)point1->x, Y1 = (int)point1->y;

    int dx = X1 - X0; 
    int dy = Y1 - Y0; 
    
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy); 
    
    float Xinc = dx / (float)steps; 
    float Yinc = dy / (float)steps; 
    
    float X = X0; 
    float Y = Y0; 
    for (int i = 0; i <= steps; i++) { 
        plot((Vector2){(int)X, (int)Y}); 
        X += Xinc; 
        Y += Yinc; 
    } 
} 

int calculate_distance_from_middle(Vector2 point, float focal_length) {
  int up = point.x * focal_length;
  float down = focal_length + point.y;
  return up/down;
}

Vector2 plot_3d(Vector3* point, int focal_length) {
  int x = calculate_distance_from_middle((Vector2){point->x, point->z}, focal_length);
  int y = calculate_distance_from_middle((Vector2){point->y, point->z}, focal_length);
  
  plot((Vector2){x, y});

  return (Vector2){x, y};
}

int main() {
  initscr();
  curs_set(0);
  keypad(stdscr, true);

  Mesh mesh;
  mesh.edges_length = 12;
  mesh.verticies_length= 8;

  mesh.verticies = (Vector3[]){
    {-1.0, -1.0, -1.0}, // 0
    { 1.0, -1.0, -1.0}, // 1
    {-1.0,  1.0, -1.0}, // 2
    { 1.0,  1.0, -1.0}, // 3
    {-1.0, -1.0,  1.0}, // 4
    { 1.0, -1.0,  1.0}, // 5
    {-1.0,  1.0,  1.0}, // 6
    { 1.0,  1.0,  1.0}, // 7
  };

  mesh.edges = (Vector2i[]) {
    {0, 1},
    {0, 2},
    {0, 4},
    {1, 3},
    {1, 5},
    {2, 3},
    {2, 6},
    {3, 7},
    {4, 5},
    {4, 6},
    {5, 7},
    {6, 7},
  };

  Matrix3 rotationX = gen_rotation_matrix(2.0, AXIS_X);
  Matrix3 rotationY = gen_rotation_matrix(1.0, AXIS_Y);
  Matrix3 rotationZ = gen_rotation_matrix(3.0, AXIS_Z);


  mesh = apply_matrix_to_mesh(mesh, gen_scale_matrix(15, 15, 15));
  while (true) {
    clear();
    mesh = apply_matrix_to_mesh(mesh, rotationX);
    mesh = apply_matrix_to_mesh(mesh, rotationY);
    // mesh = apply_matrix_to_mesh(mesh, rotationZ);
    render_mesh(&mesh, 160.0);
    refresh();
    msleep(100);
  }

  endwin();
}
