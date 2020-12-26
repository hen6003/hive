#include <SDL.h>
#include <math.h>

int width, height;

struct colour
{
  int r,g,b;
};

void render_hex(SDL_Renderer* renderer, int centerX, int centerY, struct colour col)
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
    
    //Set the draw colour of renderer
    SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, 255);

    SDL_RenderDrawLine(renderer, oldX, oldY, newX, newY);
  }
}

void render_hex_by_index(SDL_Renderer* renderer, int indexX, int indexY, struct colour col)
{
  int offsetX = indexX * 150;
  int offsetY = indexY * 173;

  if ( abs(indexX) % 2 == 1 )
    offsetY += 87;

  render_hex(renderer, width / 2 + offsetX, height / 2 + offsetY, col);
}
 
int main(int argc, char ** argv)
{
  int quit = 0;
  int posX = 0;
  int posY = 0;
  SDL_Event event;

  struct colour col_focused;
  col_focused.r = 100;
  col_focused.g = 50;
  col_focused.b = 100;

  struct colour col_unfocused;
  col_unfocused.r = 50;
  col_unfocused.g = 50;
  col_unfocused.b = 50;

  SDL_Init(SDL_INIT_EVERYTHING);
         
  SDL_Window* window = SDL_CreateWindow("game",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1920, 1080,
    SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

  //Create a renderer for the window created above, with the first display driver present
  //and with no additional settings
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
 
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
            --posY;
            break;
        
          case SDLK_DOWN:
            ++posY;
            break;
        
          case SDLK_LEFT:
            --posX;
            break;
       
          case SDLK_RIGHT:
            ++posX;
            break;

          default:

          break;
        }
        break;
    }

    SDL_GetRendererOutputSize(renderer, &width, &height);

    //Set the draw colour of renderer to bg colour
    SDL_SetRenderDrawColor(renderer, 10, 10, 10, 255);
    
    //Clear the renderer with the draw colour
    SDL_RenderClear(renderer);
    
    for (int x = -10; x < 10; x++)
    {
      for (int y = -10; y < 10; y++)
      {
        if (x == posX && y == posY)
          render_hex_by_index(renderer,x,y,col_focused);
        else
          render_hex_by_index(renderer,x,y,col_unfocused);
      }
    }

    //Update the renderer
    SDL_RenderPresent(renderer);
  }

  //Destroy the renderer created above
  SDL_DestroyRenderer(renderer);
  
  //Destroy the window created above
  SDL_DestroyWindow(window);
             
  SDL_Quit();
                 
  return 0;
}
