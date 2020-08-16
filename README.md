# Drawing Shapes
An application that allows the user to draw 2D polygons made with OpenGL.

### OpenGL
We have used the OpenGL library to create a canvas upon which the user is able to place points, connect them and create colored polygons.

### Use cases
The user can left click on the canvas and open up menu that contains all the possible actions they can perform. Those actions include:
* Placing points in the canvas. Each consecutive point is connected by a line. The user can select the color of the lines. 
* Closing a polygon by connecting the first and last points.
* Selecting a color for filling the polygon.
* Clipping the polygon.
* Triangulating a polygon. This will draw the lines of the triangles that make up a polygon.
* Extruding the canvas into 3D space. This will transform the canvas into 3D space and will display all the polygons the user has created into this 3D canvas. The user can control a moving camera with WASD while in 3D space.
