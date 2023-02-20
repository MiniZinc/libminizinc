% Type-inst synonyms for record types used in the model
type Dimensions = record(int: width, int: height);
type Coordinates = record(var 0..infinity: x, var 0..infinity: y);
type Rectangle = Dimensions ++ Coordinates;

% No overlap predicate
predicate no_overlap(Rectangle: rectA, Rectangle: rectB) = 
  rectA.x + rectA.width <= rectB.x
  \/ rectB.x + rectB.width <= rectA.x
  \/ rectA.y + rectA.height <= rectB.y
  \/ rectB.y + rectB.height <= rectA.y;

% Instance data
array[_] of Dimensions: rectDim;
Dimensions: area;

% Decision variables
array[index_set(rectDim)] of Coordinates: rectCoord ::no_output;

array[_] of Rectangle: rectangles ::output = [ rectDim[i] ++ rectCoord[i] | i in index_set(rectDim)];

% Constraint: rectangles must be placed within the area
constraint forall(rect in rectangles) (
  rect.x + rect.width <= area.width
  /\ rect.y + rect.height <= area.height
);

% Constraint: no rectangles can overlap
constraint forall(i, j in index_set(rectangles) where i < j) (
  no_overlap(rectangles[i], rectangles[j])
);