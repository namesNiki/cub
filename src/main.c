#include <stdio.h>
#include <curses.h>
#include <math.h>
#include <stdlib.h>

#define DEFAULT_CHAR '#'

typedef struct {
   int x;
   int y;
   int z;
} Vector3;

typedef struct {
   int x;
   int y;
} Vector2;

typedef struct {
  Vector3* verticies;
  int verticies_length;
  Vector2* edges;
  int edges_length;
} Mesh;

typedef struct {
  int matrix[3][3];
} Matrix3;

Matrix3 gen_translation_matrix(int x, int y, int z) {
  Matrix3 m;
  int mat[3][3] = {
    {x, 0, 0},
    {0, y, 0},
    {0, 0, z},
  };

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      m.matrix[i][j] = 0;
    }
  }
  m.matrix[0][0] = x;
  m.matrix[1][1] = y;
  m.matrix[2][2] = z;
  return m;
}
Vector3 apply_matrix_vector_to_number(Vector3 vector, int number) {
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

  Vector3 vertex;
  for (int i = 0; i < mesh->verticies_length; i++) {
    vertex = (Vector3){
      mesh->verticies[i].x + 20,
      mesh->verticies[i].y + 50,
      mesh->verticies[i].z,
    };
    rendered_points[i] = plot_3d(&vertex, focal_length);

  }
  for (int i = 0; i < mesh->edges_length; i++) {
    drawline(&rendered_points[mesh->edges[i].x], &rendered_points[mesh->edges[i].y]);
  }
}

void plot(Vector2 p) {
   mvaddch(p.x, p.y, DEFAULT_CHAR);
}

void drawline(Vector2* point0, Vector2 *point1) { 
   int X0 = point0->x, Y0 = point0->y;
   int X1 = point1->x, Y1 = point1->y;

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

int calculate_distance_from_middle(Vector2 point, int focal_length) {
  int up = point.x * focal_length;
  int down = focal_length + point.y;
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
  keypad(stdscr, true);

  Mesh mesh;
  mesh.edges_length = 12;
  mesh.verticies_length= 8;

  mesh.verticies = (Vector3[]){
    {-1, -1, -1}, // 0
    { 1, -1, -1}, // 1
    {-1,  1, -1}, // 2
    { 1,  1, -1}, // 3
    {-1, -1,  1}, // 4
    { 1, -1,  1}, // 5
    {-1,  1,  1}, // 6
    { 1,  1,  1}, // 7
  };

  mesh.edges = (Vector2[]) {
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

  mesh = apply_matrix_to_mesh(mesh, gen_translation_matrix(15, 30, 15));

  render_mesh(&mesh, 160);
  

  refresh();
  getch();


  // Matrix3 matrix = gen_translation_matrix(3, 3, 3);

  
  // Vector3 test = {1, 2, 1};
  // Vector3 out = apply_matrix_to_vertex(test, matrix);
  // printf("%i, %i, %i", out.x, out.y, out.z);

  printf("{%i, %i, %i}\n", mesh.verticies[0].x, mesh.verticies[0].y, mesh.verticies[0].z);
  
  endwin();
  printf("{%i, %i, %i}\n", mesh.verticies[0].x, mesh.verticies[0].y, mesh.verticies[0].z);
}
