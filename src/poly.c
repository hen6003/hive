#include <math.h>
#include <malloc.h>

#include "poly.h"

// Point inside polygon checking code
// Credit to https://www.geeksforgeeks.org/how-to-check-if-a-given-point-lies-inside-a-polygon/

// Given three colinear points p, q, r, the function checks if 
// point q lies on line segment 'pr' 
static bool OnSegment(Vector2 p, Vector2 q, Vector2 r) 
{ 
  if (q.x <= fmaxf(p.x, r.x) && q.x >= fminf(p.x, r.x) && 
      q.y <= fmaxf(p.y, r.y) && q.y >= fminf(p.y, r.y)) 
    return true; 
  return false; 
} 

// To find orientation of ordered triplet (p, q, r). 
// The function returns following values 
// 0 --> p, q and r are colinear 
// 1 --> Clockwise 
// 2 --> Counterclockwise 
static int Orientation(Vector2 p, Vector2 q, Vector2 r) 
{ 
  int val = (q.y - p.y) * (r.x - q.x) - 
    (q.x - p.x) * (r.y - q.y); 
  
  if (val == 0) return 0; // colinear 
  return (val > 0)? 1: 2; // clock or counterclock wise 
} 

// The function that returns true if line segment 'p1q1' 
// and 'p2q2' intersect. 
static bool DoIntersect(Vector2 p1, Vector2 q1, Vector2 p2, Vector2 q2) 
{ 
  // Find the four orientations needed for general and 
  // special cases 
  int o1 = Orientation(p1, q1, p2); 
  int o2 = Orientation(p1, q1, q2); 
  int o3 = Orientation(p2, q2, p1); 
  int o4 = Orientation(p2, q2, q1); 
  
  // General case 
  if (o1 != o2 && o3 != o4) 
    return true; 
  
  // Special Cases 
  // p1, q1 and p2 are colinear and p2 lies on segment p1q1 
  if (o1 == 0 && OnSegment(p1, p2, q1)) return true; 
  
  // p1, q1 and p2 are colinear and q2 lies on segment p1q1 
  if (o2 == 0 && OnSegment(p1, q2, q1)) return true; 
  
  // p2, q2 and p1 are colinear and p1 lies on segment p2q2 
  if (o3 == 0 && OnSegment(p2, p1, q2)) return true; 
  
  // p2, q2 and q1 are colinear and q1 lies on segment p2q2 
  if (o4 == 0 && OnSegment(p2, q1, q2)) return true; 
  
  return false; // Doesn't fall in any of the above cases 
} 

// End of GeeksForGeeks code


// Another function from GeeksForGeeks
// Returns true if the point p lies inside the polygon[] with n vertices 
bool CheckCollisionPointPolyEx(Vector2 point, Vector2 *polygon, int vertexCount) 
{ 
  // There must be at least 3 vertices in polygon[] 
  if (vertexCount < 3) return false; 
  
  // Create a point for line segment from p to infinite 
  Vector2 extreme = {10000, point.y}; 
  
  // Count intersections of the above line with sides of polygon 
  int count = 0, i = 0; 
  do
  { 
    int next = (i+1)%vertexCount; 
    
    // Check if the line segment from 'p' to 'extreme' intersects 
    // with the line segment from 'polygon[i]' to 'polygon[next]' 
    if (DoIntersect(polygon[i], polygon[next], point, extreme)) 
    { 
      // If the point 'p' is colinear with line segment 'i-next', 
      // then check if it lies on segment. If it lies, return true, 
      // otherwise false 
      if (Orientation(polygon[i], point, polygon[next]) == 0) return OnSegment(polygon[i], point, polygon[next]); 
      
      count++; 
    } 
    i = next; 
  } while (i != 0); 
  
  // Return true if count is odd, false otherwise 
  return count&1; // Same as (count%2 == 1) 
}

bool CheckCollisionPointPoly(Vector2 point, Vector2 center, int sides, float radius)
{
  bool ret;
  Vector2 *polygon = malloc((sides+2) * sizeof(Vector2));
  float incr = 2.0 * M_PI / sides;
  float angle = 0.0f;
  
  Vector2 newPos = {radius * cos(angle) + center.x,
    radius * sin(angle) + center.y};
  
  //cairo_move_to(ctx, newX, newY);
  polygon[0] = newPos;
  
  for (int i = 0; i < sides+1; i++) {
    angle += incr;
    newPos = (Vector2) {radius * cos(angle) + center.x,
      radius * sin(angle) + center.y};
    
    polygon[i+1] = newPos;
  }

  ret = CheckCollisionPointPolyEx(point, polygon, sides+1); 

  free(polygon);
  return ret;
}
