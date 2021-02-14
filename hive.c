#include <SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include <math.h>

static const GLchar* vertex_shader_source =
  "#version 120\n"
  "attribute vec2 coord2d;\n"
  "void main()\n"
  "{\n"
  "  gl_Position = vec4(coord2d, 0.0, 1.0);\n"
  "}\n";

static const GLchar* fragment_shader_source =
  "#version 120\n"
  "void main()\n"
  "{\n"
  "  gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
  "}\n";

int width, height;

height = 1800;
width = 1920;

#define WinWidth 1920
#define WinHeight 1800

struct colour
{
  int r,g,b;
};

enum tile_types
{
  none,
  bee,
  spider,
  beetle,
  grasshopper,
  ant,
};

void render_line(int x1, int y1, int x2, int y2, struct colour col)
{
  glBegin(GL_LINES);
    glColor3i(col.r, col.g, col.b);
    glVertex2f(oldX, oldY);
    glVertex2f(newX, newY);
  glEnd();
}

void render_hex(int centerX, int centerY, struct colour col)
{
  float const PI = 3.14159265;
  const int nsides = 6;

  float angle = 0.0f;
  float incr = 2.0 * PI / nsides;
  int Radius = 100;

  int newX = Radius * cos(angle) + centerX;
  int newY = Radius * sin(angle) + centerY;
  
  for (int i=0; i<nsides; i++)
  {
    int oldX = newX;
    int oldY = newY;
    angle += incr;
    newX = Radius * cos(angle) + centerX;
    newY = Radius * sin(angle) + centerY;
    
    glBegin(GL_LINES);
      glColor3f(1,0,0);
      glVertex2f(oldX, oldY);
      glVertex2f(newX, newY);
    glEnd();
  }
}

void render_hex_by_index(int indexX, int indexY, struct colour col)
{
  int offsetX = indexX * 150;
  int offsetY = indexY * 173;

  if ( abs(indexX) % 2 == 1 )
    offsetY += 86;

  render_hex(width / 2 + offsetX, height / 2 + offsetY, col);
}

void choose_tile_type(int pos, struct colour col_focused, struct colour col_unfocused)
{
  SDL_Rect rect;
  rect.x = 0;
  rect.y = height+1-height/4;
  rect.w = width;
  rect.h = height;
  
  // SDL_RenderDrawLine(renderer, 0, height-height/4, width, height-height/4);
  // SDL_SetRenderDrawColor(renderer, 20, 20, 20, 100);
  // SDL_RenderFillRect(renderer, &rect);

  for (int i=0;i<5;++i)
  {
    if (i == pos)
      render_hex(renderer, width / 8 * (i+2), height - height / 8, col_focused);
    else
      render_hex(renderer, width / 8 * (i+2), height - height / 8, col_unfocused);
  }
}

void quit_menu(int pos,struct colour col_unfocused, struct colour col_focused)
{
  SDL_Rect rect;
  rect.x = 0;
  rect.y = height+1-height/4;
  rect.w = width;
  rect.h = height;

  struct colour col_quit;
  col_quit.r = 255;
  col_quit.g = 0;
  col_quit.b = 0;
  
  // SDL_RenderDrawLine(renderer, 0, height-height/4, width, height-height/4);
  // SDL_SetRenderDrawColor(renderer, 20, 20, 20, 100);
  // SDL_RenderFillRect(renderer, &rect);

  if (pos)
  {
    render_hex(renderer, width / 2 - 200, height - height / 8, col_focused);
    render_hex(renderer, width / 2 + 200, height - height / 8, col_quit);
  }
  else
  {
    render_hex(renderer, width / 2 + 200, height - height / 8, col_focused);
    render_hex(renderer, width / 2 - 200, height - height / 8, col_unfocused);
  }
}
 
#define MAX_SIZE 20

int main(int argc, char ** argv)
{
  int quit = 0;
  int show_type_menu = 0;
  int show_quit_menu = 0;
  int posX = 0;
  int posY = 0;
  int type_select_pos;
  int quit_select_pos;
  SDL_Event event;
  
  struct colour col;

  struct colour col_focused;
  col_focused.r = 100;
  col_focused.g = 50;
  col_focused.b = 100;

  struct colour col_unfocused;
  col_unfocused.r = 50;
  col_unfocused.g = 50;
  col_unfocused.b = 50;

  enum tile_types tiles[MAX_SIZE][MAX_SIZE];
        
  SDL_Rect rect;
  rect.w = 50;
  rect.h = 50;
  
  u32 WindowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP;

  GLuint fragment_shader, program, vertex_shader;

  /* Vertex shader */
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL); 
  glCompileShader(vertex_shader);
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &log_length);

  /* Fragment shader */
  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &log_length);

  /* Link shaders */
  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
  
  for (int x = 0; x < MAX_SIZE; ++x)
    for (int y = 0; y < MAX_SIZE; ++y)
      tiles[x][y] = none;

  SDL_Init(SDL_INIT_EVERYTHING);

  SDL_Window *window = SDL_CreateWindow("HIVE", 0, 0, WinWidth, WinHeight, WindowFlags);
  assert(window);
  SDL_GLContext Context = SDL_GL_CreateContext(window);
 
  while (!quit)
  {
    SDL_WaitEvent(&event);
                   
    switch (event.type)
    {
      case SDL_QUIT:
        quit = 1;
        break;
      
      case SDL_KEYDOWN:
        //Select surfaces based on key press
        switch( event.key.keysym.sym )
        {
          case SDLK_UP:
            if (show_type_menu)
              break;
            --posY;
            break;
        
          case SDLK_DOWN:
            if (show_type_menu)
              break;
            ++posY;
            break;
        
          case SDLK_LEFT:
            if (show_type_menu)
            {
              if (type_select_pos > 0)
                --type_select_pos;
            }
            else if (show_quit_menu)
            {
              if (quit_select_pos < 1)
                ++quit_select_pos;
            }
            else
              --posX;
            break;
       
          case SDLK_RIGHT:
            if (show_type_menu)
            {
              if (type_select_pos < 4)
                ++type_select_pos;
            }
            else if (show_quit_menu)
            {
              if (quit_select_pos > 0)
                --quit_select_pos;
            }
            else
              ++posX;
            break;
          
          case SDLK_RETURN:
            if (show_type_menu)
            {
              show_type_menu = 0;
              tiles[posX+10][posY+10] = type_select_pos + 1;
            }
            else if (show_quit_menu)
            {
              show_quit_menu = 0;
              quit = !quit_select_pos;
            }
            else
            {
              show_type_menu = 1;
              type_select_pos = 2;
            }
            break;

          case SDLK_q:
            if (show_type_menu)
              show_type_menu = 0;
            else if (show_quit_menu)
              show_quit_menu = 0;
            else
            {
              quit_select_pos = 1;
              show_quit_menu = 1;
            }
            break;
        }
        break;
    }

    SDL_GetRendererOutputSize(renderer, &width, &height);

    glViewport(0, 0, WinWidth, WinHeight);
    glClearColor(0.2f, 0.2f, 1.2f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);

    SDL_GL_SwapWindow(window);

    for (int x = -MAX_SIZE/2; x < MAX_SIZE/2; x++)
      for (int y = -MAX_SIZE/2; y < MAX_SIZE/2; y++)
      {
        switch (tiles[x+10][y+10])
        {
          case none:
            col.r = 20;
            col.g = 20;
            col.b = 20;
            break;
          case bee:
            col.r = 255;
            col.g = 255;
            col.b = 0;
            break;
          case spider:
            col.r = 160;
            col.g = 100;
            col.b = 0;
            break;
          case beetle:
            col.r = 255;
            col.g = 0;
            col.b = 255;
            break;
          case grasshopper:
            col.r = 0;
            col.g = 255;
            col.b = 0;
            break;
          case ant:
            col.r = 0;
            col.g = 0;
            col.b = 255;
            break;
        }

        render_hex_by_index(renderer,x,y,col);
      }

    render_hex_by_index(renderer,posX,posY,col_focused);

    if (show_type_menu)
      choose_tile_type(renderer, type_select_pos, col_focused, col_unfocused);
    else if (show_quit_menu)
      quit_menu(renderer, quit_select_pos, col_unfocused, col_focused);

    SDL_GL_SwapWindow(window);
  }
  
  //Destroy the window created above
  SDL_DestroyWindow(window);
             
  SDL_Quit();
                 
  return 0;
}
