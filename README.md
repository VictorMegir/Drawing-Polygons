# Drawing Shapes
An application that allows the user to draw 2D polygons. This application was made using OpenGL libraries.

### OpenGL
We have used the OpenGL library to create a canvas upon which the user is able to place points, connect them and create colored polygons.
Using OpenGL's function we created a menu which allows the user to select among several actions to perform on the polygons they have created.

### Use cases
The user can left click on the canvas and open up the menu that contains all the possible actions they can perform. Those actions include:
* Placing points in the canvas. Each consecutive point is connected by a line. The user can select the color of the lines.
* Selecting a color for filling the polygon. The application offers several color choices from OpenGL's color library. When the polygon is closed the application fills its interior with the selcted color.
* Closing a polygon by connecting the first and last points. The application is able to detect selfintersecting polygons and color them properly.
* Clipping the polygon. This wil request the user to select two points in the canvas that define a clipping rectangle.
* Triangulating a polygon. This will draw the lines of the triangles that make up a polygon.
* Extruding the canvas into 3D space. This will transform the canvas into 3D space and will display all the polygons the user has created into this 3D canvas. The user can control a moving camera with WASD while in 3D space.
