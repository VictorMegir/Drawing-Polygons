#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include "triangulate.h"
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream> 

using namespace std;

typedef struct point
{
	float k,l;
}Points;

typedef struct tri
{
	Points a,b,c;

}Triangles;

typedef struct poly
{
	Points azor[100];
	int counter,tricounter;
	Triangles rex[100];
	float color_lines[3];
	float color_fill[3];
	float length;
}polygon;

void init(void);
void display(void);
void mouse(int button,int state,int x,int y);
void keyboard(unsigned char key,int x,int y);
void nonselfintersecting(Points azor[],int counter);
void createGLUTMenus();
void processMenuEvents(int option);
void closeintersection(Points azor[],int counter);
void copyPoly(polygon p);
void clipping(float xstart,float ystart,float xfinish,float yfinish);
void clip(int poly_points[][2], int &poly_size, int x1, int y1, int x2, int y2);
void suthHodgClip(int poly_points[][2], int poly_size, int clipper_points[][2], int clipper_size);
int x_intersect(int x1, int y1, int x2, int y2,int x3, int y3, int x4, int y4);
int y_intersect(int x1, int y1, int x2, int y2,int x3, int y3, int x4, int y4);
void clearClip(float xmin,float xmax,float ymin,float ymax);
void mouseMove(int x, int y);
void display3D();

// angle of rotation for the camera direction
float angle = 0.0f;

// actual vector representing the camera's direction
float lx = 0.0f, lz = -1.0f;

// XZ position of the camera
float  x=250.0f, y=250.0f, z = 160.0f;

// the key states. These variables will be zero
//when no key is being presses
float deltaAngle = 0.0f;
float deltaMove = 0.0f;
int xOrigin = -1;


float r,g,b;
int check = 0;									//flag for point placement mode
int flag = 0;									//flag for drawing mode
int close = 0;									//flag for when polygon closes
int polygon_num = -1;								//number of polygons
int destroy_two,destroy = 0;							//flag for selfintersections
int tpressed = 0;								//when T is pressed
int appear = 0;									//when triangles appear and then disappear
int lineflag = 0;								//flag for choosing line color
int fillflag = 0;								//flag for choosing fill color
int clipflag = 0;								//flag for clipping
int drawRect = 0;								//flag for drawing clipping rectangle
int clipped = 0;								//flag for clipped state
int klik = 0;									//flag for clipping rectangle points
int new_action = 0;								//flag for new action so that the triangles dissapear
Points p1[3];									//clipping rectangle points array
int extrude = 0;								//flag for extrude
int w;										//counter for clipping rectangle points

polygon array[200];								//array of polygons
polygon temp[300];								//temporary array with polygons for selfintersection
			
void init(void)
{
	if(extrude)
	{
		glutInitDisplayMode( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
		glClearDepth(1.0f);											// Set background depth to farthest
		glEnable(GL_DEPTH_TEST);  							// Enable depth testing for z-culling
		glDepthFunc(GL_LEQUAL);    							// Set the type of depth-test
		glShadeModel(GL_SMOOTH);   							// Enable smooth shading
	}
	else
	{
		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );	//creates a single frame buffer of RGB color capacity.
	}
	glClearColor(1.0,1.0,1.0,1.0);
}

void printPoint(Points a)
{
	printf("%f\t",a.k);
	printf("%f\n",a.l);
}

void display(void)
{  
	glColor3f(r,g,b); 											// sets the current drawing (foreground) color to blue 
	glPointSize(4);
	if(extrude == 1)
	{	
		display3D();
	}
	glMatrixMode(GL_PROJECTION);						// sets the current matrix to projection
	glLoadIdentity();												//multiply the current matrix by identity matrix
	gluOrtho2D(0.0, 600.0, 0.0, 500.0);		  //sets the parallel(orthographic) projection of the full frame buffer
	
	int i,j;
	if(new_action == 1)
	{
			for(i=0;i<polygon_num+1;i++)
	 		{
		 		for(j=0;j<array[polygon_num].tricounter;j++)
		 		{
					glBegin(GL_LINES);
						glColor3f(array[i].color_fill[0],array[i].color_fill[1],array[i].color_fill[2]);
						glVertex2f(array[i].rex[j].c.k,array[i].rex[j].c.l);
						glVertex2f(array[i].rex[j].a.k,array[i].rex[j].a.l);
					glEnd();
				}
			}
	}
	
	if( w<2 && klik == 1)
	 {
		glBegin(GL_POINTS);									    // writes pixels on the frame buffer with the current drawing color
		glColor3f(0.85f,0.10f,0.33f);						//baurdaux
		if(w==0)
		{
			p1[0].k=x;
			p1[0].l=y;
		}
		else
		{
			p1[1].k=x;
			p1[1].l=y;
			clipflag = 0;
		}
		
		glVertex2f(p1[0].k,p1[0].l);
		glVertex2f(p1[1].k,p1[1].l);

		klik = 0;
		glEnd();
		if(w>0)
		{	
				clipping(p1[0].k,p1[0].l,p1[1].k,p1[1].l);
		}
		w++;
	}
	if( clipped == 1 && appear == 1 )
 	{
 		int i,j;
 		for(i=0;i<polygon_num+1;i++)
 		{
	 		for(j=0;j<=array[polygon_num].tricounter;j++)
	 		{
					glBegin(GL_LINES);
						glColor3f(0.0f, 0.5f, 0.0f); 										//always green
						glVertex2f(array[i].rex[j].c.k,array[i].rex[j].c.l);
						glVertex2f(array[i].rex[j].a.k,array[i].rex[j].a.l);
					glEnd();
			}
		}
	} 
	clipped = 0;
	if(check || close)  
	{	
			int i;		
			for(i=0;i<=polygon_num;i++)
			{
				temp[i]=array[i];
			}
			glBegin(GL_POINTS);					// writes pixels on the frame buffer with the current drawing color	
			Points currentP;
			currentP.k=x;
			currentP.l=y;
			temp[polygon_num].azor[temp[polygon_num].counter]=currentP;
			nonselfintersecting(temp[polygon_num].azor,temp[polygon_num].counter);
			if(destroy==0)		
			{
				array[polygon_num].azor[array[polygon_num].counter]=currentP;
				glVertex2i(x,y);
				if(close!=1){
					array[polygon_num].counter++;
				}
			}
			destroy = 0;
		glEnd();
	}
	if(clipped == 0)
	{
		glBegin(GL_LINES);
	 		glColor3f(array[polygon_num].color_lines[0], array[polygon_num].color_lines[1], array[polygon_num].color_lines[2]);
	 		int j;	
			for(j=0; j<array[polygon_num].counter; j++)
			{
				if(j!=0)
				{	
					glVertex2f(array[polygon_num].azor[j].k,array[polygon_num].azor[j].l);
					glVertex2f(array[polygon_num].azor[j-1].k,array[polygon_num].azor[j-1].l);
				}
			}
		glEnd();
	}
	clipped = 0;
	if(close == 1)
	{
		closeintersection(array[polygon_num].azor,array[polygon_num].counter);
		if(destroy_two == 0)
		{
			copyPoly(array[polygon_num]);	
			glBegin(GL_LINES);
 			glColor3f(array[polygon_num].color_lines[0], array[polygon_num].color_lines[1], array[polygon_num].color_lines[2]);
				glVertex2f(array[polygon_num].azor[array[polygon_num].counter-1].k,array[polygon_num].azor[array[polygon_num].counter-1].l);
				glVertex2f(array[polygon_num].azor[0].k,array[polygon_num].azor[0].l);
			glEnd();
			flag = 0;
		}
		int l,r;
		
		for(l=0;l<polygon_num+1;l++)
		{
	 		for(r=0;r<array[polygon_num].tricounter;r++)
	 		{
				if(destroy_two == 0 )
				{
					glBegin(GL_TRIANGLES);
		 			glColor3f(array[l].color_fill[0], array[l].color_fill[1], array[l].color_fill[2]);
					glVertex2f(array[l].rex[r].a.k,array[l].rex[r].a.l);
					glVertex2f(array[l].rex[r].b.k,array[l].rex[r].b.l);
					glVertex2f(array[l].rex[r].c.k,array[l].rex[r].c.l);					
					glEnd();						
					//redisplay every line ever
					glBegin(GL_LINES);
						glColor3f(array[l].color_lines[0], array[l].color_lines[1], array[l].color_lines[2]);
						int k;	
						for(k=0; k<array[l].counter; k++)
						{
							if(k!=0)
							{					
								glVertex2f(array[l].azor[k].k,array[l].azor[k].l);
								glVertex2f(array[l].azor[k-1].k,array[l].azor[k-1].l);
							}
						}
						glVertex2f(array[l].azor[array[l].counter-1].k,array[l].azor[array[l].counter-1].l);
						glVertex2f(array[l].azor[0].k,array[l].azor[0].l);
					glEnd();
				}
			}
		}
		destroy_two = 0;
	 	close=0;
	 }
	
 	if(tpressed == 1)
 	{
 		int i,j;
 		for(i=0;i<polygon_num+1;i++)
 		{
	 		for(j=0;j<=array[polygon_num].tricounter;j++)
	 		{
				if(appear == 1 )
				{
					glBegin(GL_LINES);
						glColor3f(0.0f, 0.5f, 0.0f); 					//always green
						glVertex2f(array[i].rex[j].c.k,array[i].rex[j].c.l);
						glVertex2f(array[i].rex[j].a.k,array[i].rex[j].a.l);
					glEnd();
				}
				else
				{
					glBegin(GL_LINES);
						glColor3f(array[i].color_fill[0],array[i].color_fill[1],array[i].color_fill[2]);
						glVertex2f(array[i].rex[j].c.k,array[i].rex[j].c.l);
						glVertex2f(array[i].rex[j].a.k,array[i].rex[j].a.l);
					glEnd();
				}
			}
		}
	} 
	
	glFlush(); 	
}

void computePos(float deltaMove) {

	x += deltaMove * lx * 0.1f;
	z += deltaMove * lz * 0.1f;
}

void display3D()
{
   if(extrude==0)return;
   if (deltaMove){
				computePos(deltaMove);
   }
   for(int u=0; u<=polygon_num; u++)
   {
		  if(array[u].length<=0.0)
		  {
					printf("GIVE THE EXTRUSION LENGTH FOR POLYGON %d:\n",u+1);
					scanf("%f",&array[u].length);
		 	}
   }
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
   glMatrixMode(GL_PROJECTION);     // To operate on model-view matrix
 
   // Render a color-cube consisting of 6 quads with different colors
   glLoadIdentity();                 // Reset the model-view matrix

   gluPerspective(90.0f, 1.2f, 0.1f, 400.0f);

   // Get Back to the Modelview
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();                 // Reset the model-view matrix
   gluLookAt(x, 250.0f, z,x + lx, 250.0f, z + lz, 0.0f, 1.0f, 0.0f);
   
   for(int k=0; k<=polygon_num; k++)
   {
			glBegin(GL_QUADS);
			glColor3f(array[k].color_lines[0], array[k].color_lines[1], array[k].color_lines[2]);
			for(int i=0; i<array[k].counter; i++)
			{
					glVertex3f(array[k].azor[i].k,array[k].azor[i].l,-array[k].length);
					glVertex3f(array[k].azor[i].k,array[k].azor[i].l,array[k].length);		
					glVertex3f(array[k].azor[i+1].k,array[k].azor[i+1].l,array[k].length);
					glVertex3f(array[k].azor[i+1].k,array[k].azor[i+1].l,-array[k].length);
			}
			glEnd();
			
			glBegin(GL_QUADS);
			glColor3f(array[k].color_lines[0], array[k].color_lines[1], array[k].color_lines[2]);
			glVertex3f(array[k].azor[0].k,array[k].azor[0].l,array[k].length);
			glVertex3f(array[k].azor[0].k,array[k].azor[0].l,-array[k].length);		
			glVertex3f(array[k].azor[array[k].counter].k,array[k].azor[array[k].counter].l,-array[k].length);
			glVertex3f(array[k].azor[array[k].counter].k,array[k].azor[array[k].counter].l,array[k].length);
			glEnd();
		  	   
			glBegin(GL_TRIANGLES);
			glColor3f(array[k].color_fill[0], array[k].color_fill[1], array[k].color_fill[2]);		
			for(int j=0; j<=array[k].tricounter; j++)
			{
				glVertex3f(array[k].rex[j].a.k,array[k].rex[j].a.l,array[k].length);
				glVertex3f(array[k].rex[j].b.k,array[k].rex[j].b.l,array[k].length);
				glVertex3f(array[k].rex[j].c.k,array[k].rex[j].c.l,array[k].length);
				 
				glVertex3f(array[k].rex[j].a.k,array[k].rex[j].a.l,-array[k].length);
				glVertex3f(array[k].rex[j].b.k,array[k].rex[j].b.l,-array[k].length);
				glVertex3f(array[k].rex[j].c.k,array[k].rex[j].c.l,-array[k].length);
			}
			glEnd();
			
			if(tpressed)
			{
					if(appear == 1 )
					{
							for(int f=0; f<=array[k].tricounter;f++)
							{
								glBegin(GL_LINES);
									glColor3f(0.0f, 0.5f, 0.0f); 					//always green
									glVertex3f(array[k].rex[f].c.k,array[k].rex[f].c.l,array[k].length);
									glVertex3f(array[k].rex[f].a.k,array[k].rex[f].a.l,array[k].length);
								glEnd();
							}
							for(int r=0; r<=array[k].tricounter;r++)
							{
								glBegin(GL_LINES);
									glColor3f(0.0f, 0.5f, 0.0f); 					//always green
									glVertex3f(array[k].rex[r].c.k,array[k].rex[r].c.l,-array[k].length);
									glVertex3f(array[k].rex[r].a.k,array[k].rex[r].a.l,-array[k].length);
								glEnd();
							}
					}
			}
			
			glBegin(GL_LINES);
			glColor3f(array[k].color_lines[0], array[k].color_lines[1], array[k].color_lines[2]);
			for(int i=0; i<array[k].counter; i++)
			{
				glVertex3f(array[k].azor[i].k,array[k].azor[i].l,array[k].length);
				glVertex3f(array[k].azor[i+1].k,array[k].azor[i+1].l,array[k].length);
			}
			glVertex3f(array[k].azor[0].k,array[k].azor[0].l,array[k].length);
			glVertex3f(array[k].azor[array[k].counter].k,array[k].azor[array[k].counter].l,array[k].length);
			glEnd();
			
			glBegin(GL_LINES);
			glColor3f(array[k].color_lines[0], array[k].color_lines[1], array[k].color_lines[2]);
			for(int i=0; i<array[k].counter; i++)
			{
				glVertex3f(array[k].azor[i].k,array[k].azor[i].l,-array[k].length);
				glVertex3f(array[k].azor[i+1].k,array[k].azor[i+1].l,-array[k].length);
			}
			glVertex3f(array[k].azor[0].k,array[k].azor[0].l,-array[k].length);
			glVertex3f(array[k].azor[array[k].counter].k,array[k].azor[array[k].counter].l,-array[k].length);
			glEnd();
			
			glBegin(GL_LINES);
			glColor3f(array[k].color_lines[0], array[k].color_lines[1], array[k].color_lines[2]);
			for(int i=0; i<array[k].counter; i++)
			{
				glVertex3f(array[k].azor[i].k,array[k].azor[i].l,array[k].length);
				glVertex3f(array[k].azor[i].k,array[k].azor[i].l,-array[k].length);
			}
			glEnd();
	}
	glutSwapBuffers();  // Swap the front and back frame buffers (double buffering)
	return;
}

void closeintersection(Points azor[],int counter)
{
		float a,b,k,c,k_total,c_total;
		float xTempMax,xTempMin,xTotalMax,xTotalMin;
		float yTempMax,yTempMin,yTotalMax,yTotalMin;
	
		k_total= (azor[counter].l-azor[0].l)/ (azor[counter].k-azor[0].k); 
		c_total= azor[0].l - (k_total * azor[0].k);
		if(azor[0].k>=azor[counter].k && azor[counter].k!=0)
		{
				xTotalMax=azor[0].k;
				xTotalMin=azor[counter].k;
		}
		else
		{
				xTotalMax=azor[counter].k;
				xTotalMin=azor[0].k;
		}
		if(azor[0].k>=azor[counter].k && azor[counter].k!=0)
		{
				xTotalMax=azor[0].k;
				xTotalMin=azor[counter].k;
		}
		else
		{
				xTotalMax=azor[counter].k;
				xTotalMin=azor[0].k;
		}
		if(azor[0].l>=azor[counter].l && azor[counter].l!=0)
		{
				yTotalMax=azor[0].l;
				yTotalMin=azor[counter].l;
		}
		else
		{
				yTotalMax=azor[counter].l;
				yTotalMin=azor[0].l;
		}		
	
		for(int i=0; i<counter; i++)
		{
				k= (azor[i+1].l-azor[i].l)/ (azor[i+1].k-azor[i].k); 
				c= azor[i].l - (k *azor[i].k);
				a = (c_total-c)/(k-k_total);
				b = k*a+c;
		
				if(azor[i].k>=azor[i+1].k && azor[i+1].k!= 0)
				{
						xTempMax=azor[i].k;
						xTempMin=azor[i+1].k;
				}
				else
				{
						xTempMax=azor[i+1].k;
						xTempMin=azor[i].k;
				}
				if(azor[i].l>=azor[i+1].l && azor[i+1].l!= 0)
				{
						yTempMax=azor[i].l;
						yTempMin=azor[i+1].l;
				}
				else
				{
						yTempMax=azor[i+1].l;
						yTempMin=azor[i].l;
				}
				if(a>xTempMin && a<xTempMax && a>xTotalMin && a<xTotalMax && b>yTempMin && b<yTempMax && b>yTotalMin && b<yTotalMax)
				{			
						printf("\n");
						destroy_two = 1;
				}	
		}
}
void nonselfintersecting(Points azor[],int counter)
{	
		float a,b,k,c,k_total,c_total;
		float xTempMax,xTempMin,xTotalMax,xTotalMin;
		float yTempMax,yTempMin,yTotalMax,yTotalMin;

		k_total= (azor[counter].l-azor[counter-1].l)/ (azor[counter].k-azor[counter-1].k); 
		c_total= azor[counter-1].l - (k_total *azor[counter-1].k);
		if(azor[counter-1].k>=azor[counter].k && azor[counter].k!=0)
		{
				xTotalMax=azor[counter-1].k;
				xTotalMin=azor[counter].k;
		}
		else
		{
				xTotalMax=azor[counter].k;
				xTotalMin=azor[counter-1].k;
		}
		if(azor[counter-1].k>=azor[counter].k && azor[counter].k!=0)
		{
				xTotalMax=azor[counter-1].k;
				xTotalMin=azor[counter].k;
		}
		else
		{
				xTotalMax=azor[counter].k;
				xTotalMin=azor[counter-1].k;
		}
		if(azor[counter-1].l>=azor[counter].l && azor[counter].l!=0)
		{
				yTotalMax=azor[counter-1].l;
				yTotalMin=azor[counter].l;
		}
		else
		{
				yTotalMax=azor[counter].l;
				yTotalMin=azor[counter-1].l;
		}	
		for(int i=0; i<counter; i++)
		{
				k= (azor[i+1].l-azor[i].l)/ (azor[i+1].k-azor[i].k); 
				c= azor[i].l - (k *azor[i].k);
				a = (c_total-c)/(k-k_total);
				b = k*a+c;
		
				if(azor[i].k>=azor[i+1].k && azor[i+1].k!= 0)
				{
						xTempMax=azor[i].k;
						xTempMin=azor[i+1].k;
				}
				else
				{
						xTempMax=azor[i+1].k;
						xTempMin=azor[i].k;
				}
				if(azor[i].l>=azor[i+1].l && azor[i+1].l!= 0)
				{
						yTempMax=azor[i].l;
						yTempMin=azor[i+1].l;
				}
				else
				{
						yTempMax=azor[i+1].l;
						yTempMin=azor[i].l;
				}
				if(a>xTempMin && a<xTempMax && a>xTotalMin && a<xTotalMax && b>yTempMin && b<yTempMax && b>yTotalMin && b<yTotalMax)
				{			
						printf("\n");
						destroy = 1;
				}	
		}
}

static const float EPSILON=0.0000000001f;

float Triangulate::Area(const Vector2dVector &contour) 
{
	int n = contour.size();
	float A=0.0f;
	for(int p=n-1,q=0; q<n; p=q++){
		A+= contour[p].GetX()*contour[q].GetY() - contour[q].GetX()*contour[p].GetY();
	}
	return A*0.5f;
}
bool Triangulate::InsideTriangle(float Ax, float Ay,float Bx, float By,float Cx, float Cy,float Px, float Py)
{
	float ax, ay, bx, by, cx, cy, apx, apy, bpx, bpy, cpx, cpy;
	float cCROSSap, bCROSScp, aCROSSbp;

	ax = Cx - Bx;  ay = Cy - By;
	bx = Ax - Cx;  by = Ay - Cy;
	cx = Bx - Ax;  cy = By - Ay;
	apx= Px - Ax;  apy= Py - Ay;
	bpx= Px - Bx;  bpy= Py - By;
	cpx= Px - Cx;  cpy= Py - Cy;

	aCROSSbp = ax*bpy - ay*bpx;
	cCROSSap = cx*apy - cy*apx;
	bCROSScp = bx*cpy - by*cpx;

	return ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f));
}

bool Triangulate::Snip(const Vector2dVector &contour,int u,int v,int w,int n,int *V)
{
	int p;
	float Ax, Ay, Bx, By, Cx, Cy, Px, Py;

	Ax = contour[V[u]].GetX();
	Ay = contour[V[u]].GetY();

	Bx = contour[V[v]].GetX();
	By = contour[V[v]].GetY();

	Cx = contour[V[w]].GetX();
	Cy = contour[V[w]].GetY();

	if ( EPSILON > (((Bx-Ax)*(Cy-Ay)) - ((By-Ay)*(Cx-Ax))) ) return false;

	for (p=0;p<n;p++)
	{
		if( (p == u) || (p == v) || (p == w) ) continue;
		Px = contour[V[p]].GetX();
		Py = contour[V[p]].GetY();
		if (InsideTriangle(Ax,Ay,Bx,By,Cx,Cy,Px,Py)) return false;
	}
	return true;
}

bool Triangulate::Process(const Vector2dVector &contour,Vector2dVector &result)
{
	/* allocate and initialize list of Vertices in polygon */

	int n = contour.size();
	if ( n < 3 ) return false;

	int *V = new int[n];

	/* we want a counter-clockwise polygon in V */

	if ( 0.0f < Area(contour) )
	for (int v=0; v<n; v++) V[v] = v;
	else
	for(int v=0; v<n; v++) V[v] = (n-1)-v;

	int nv = n;

	/*  remove nv-2 Vertices, creating 1 triangle every time */
	int count = 2*nv;   /* error detection */

	for(int m=0, v=nv-1; nv>2; )
	{
	/* if we loop, it is probably a non-simple polygon */
	if (0 >= (count--))
	{
	//** Triangulate: ERROR - probable bad polygon!
	return false;
	}

	/* three consecutive vertices in current polygon, <u,v,w> */
	int u = v  ; if (nv <= u) u = 0;     	/* previous */
	v = u+1; if (nv <= v) v = 0;     	/* new v    */
	int w = v+1; if (nv <= w) w = 0;    	/* next     */

	if ( Snip(contour,u,v,w,nv,V) )
	{
		int a,b,c,s,t;

		/* true names of the vertices */
		a = V[u]; b = V[v]; c = V[w];

		/* output Triangle */
		result.push_back( contour[a] );
		result.push_back( contour[b] );
		result.push_back( contour[c] );

		m++;
		/* remove v from remaining polygon */
		for(s=v,t=v+1;t<nv;s++,t++) V[s] = V[t]; nv--;

		/* resest error detection counter */
		count = 2*nv;
   		}
	}
	delete V;
	return true;
}
  
// Returns x-value of point of intersectipn of two 
// lines 
int x_intersect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) 
{ 
	int num = (x1*y2 - y1*x2) * (x3-x4) - (x1-x2) * (x3*y4 - y3*x4); 
	int den = (x1-x2) * (y3-y4) - (y1-y2) * (x3-x4); 
	return num/den; 
} 
  
// Returns y-value of point of intersectipn of 
// two lines 
int y_intersect(int x1, int y1, int x2, int y2,  int x3, int y3, int x4, int y4) 
{ 
	int num = (x1*y2 - y1*x2) * (y3-y4) - (y1-y2) * (x3*y4 - y3*x4); 
	int den = (x1-x2) * (y3-y4) - (y1-y2) * (x3-x4); 
	return num/den; 
} 
  
void clip(int poly_points[][2], int &poly_size, int x1, int y1, int x2, int y2) 
{ 
    int new_points[200][2], new_poly_size = 0; 
  
    // (ix,iy),(kx,ky) are the co-ordinate values of 
    // the points 
    for (int i = 0; i < poly_size; i++) 
    { 
        // i and k form a line in polygon 
        int k = (i+1) % poly_size; 
        int ix = poly_points[i][0], iy = poly_points[i][1]; 
        int kx = poly_points[k][0], ky = poly_points[k][1]; 
  
        // Calculating position of first point 
        // w.r.t. clipper line 
        int i_pos = (x2-x1) * (iy-y1) - (y2-y1) * (ix-x1); 
  
        // Calculating position of second point 
        // w.r.t. clipper line 
        int k_pos = (x2-x1) * (ky-y1) - (y2-y1) * (kx-x1); 
  
        // Case 1 : When both points are inside 
        if (i_pos < 0  && k_pos < 0) 
        { 
            //Only second point is added 
            new_points[new_poly_size][0] = kx; 
            new_points[new_poly_size][1] = ky; 
            new_poly_size++; 
        } 
  
        // Case 2: When only first point is outside 
        else if (i_pos >= 0  && k_pos < 0) 
        { 
            // Point of intersection with edge 
            // and the second point is added 
            new_points[new_poly_size][0] = x_intersect(x1, 
                              y1, x2, y2, ix, iy, kx, ky); 
            new_points[new_poly_size][1] = y_intersect(x1, 
                              y1, x2, y2, ix, iy, kx, ky); 
            new_poly_size++; 
  
            new_points[new_poly_size][0] = kx; 
            new_points[new_poly_size][1] = ky; 
            new_poly_size++; 
        } 
  
        // Case 3: When only second point is outside 
        else if (i_pos < 0  && k_pos >= 0) 
        { 
            //Only point of intersection with edge is added 
            new_points[new_poly_size][0] = x_intersect(x1, 
                              y1, x2, y2, ix, iy, kx, ky); 
            new_points[new_poly_size][1] = y_intersect(x1, 
                              y1, x2, y2, ix, iy, kx, ky); 
            new_poly_size++; 
        } 
  
        // Case 4: When both points are outside 
        else
        { 
            //No points are added 
        } 
    }  
    // Copying new points into original array 
    // and changing the no. of vertices 
    poly_size = new_poly_size; 
    for (int i = 0; i < poly_size; i++) 
    { 
        poly_points[i][0] = new_points[i][0]; 
        poly_points[i][1] = new_points[i][1]; 
    } 
} 
  
// Implements Sutherland–Hodgman algorithm 
void suthHodgClip(int poly_points[][2], int poly_size, int clipper_points[][2], int clipper_size) 
{ 
	//i and k are two consecutive indexes 
	int i;
	for (i=0; i<clipper_size; i++) 
	{ 
		int k = (i+1) % clipper_size; 

		// We pass the current array of vertices, it's size 
		// and the end points of the selected clipper line 
		clip(poly_points, poly_size, clipper_points[i][0], clipper_points[i][1], clipper_points[k][0],clipper_points[k][1]); 
	} 
	
	for(int e=0; e<=polygon_num; e++)
	{	
			for(int p=0; p<array[polygon_num].tricounter; p++)
			{
				array[e].rex[p].a.k=0.0;
				array[e].rex[p].a.l=0.0;
				array[e].rex[p].b.k=0.0;
				array[e].rex[p].b.l=0.0;
				array[e].rex[p].c.k=0.0;
				array[e].rex[p].c.l=0.0;
			}
			array[e].counter=poly_size;
			
			for(int j=0; j<poly_size; j++)
			{
				array[e].azor[j].k=poly_points[j][0];
				array[e].azor[j].l=poly_points[j][1];				
			}
			copyPoly(array[e]);
	}
	clipped=1;
	
	int j;
	for(j=0; j < poly_size; j++)
	{
		glBegin(GL_POINTS);
		glColor3f(0.0f,0.0f,1.0f);			//points of intersection become blue
		glVertex2f(poly_points[j][0],poly_points[j][1]);
		glEnd();
	}
} 
void clearClip(float xmin,float xmax,float ymin,float ymax)
{
		glBegin(GL_QUADS);
  	glColor3f(1.0f,1.0f,1.0f);
  	glVertex2f(0.0,0.0);
  	glVertex2f(0.0,500.0);
  	glVertex2f(xmin,500.0);
  	glVertex2f(xmin,0.0);
  	glEnd();
  	
  	glBegin(GL_QUADS);
  	glColor3f(1.0f,1.0f,1.0f);
  	glVertex2f(0.0,ymax);
  	glVertex2f(600.0,ymax);
  	glVertex2f(600.0,500.0);
  	glVertex2f(0.0,500.0);
  	glEnd();
  	
  	glBegin(GL_QUADS);
  	glColor3f(1.0f,1.0f,1.0f);
  	glVertex2f(xmax,500.0);
  	glVertex2f(600.0,500.0);
  	glVertex2f(600.0,0.0);
  	glVertex2f(xmax,0.0);
  	glEnd();
  	
  	glBegin(GL_QUADS);
  	glColor3f(1.0f,1.0f,1.0f);
  	glVertex2f(0.0,ymin);
  	glVertex2f(600.0,ymin);
  	glVertex2f(600.0,0.0);
  	glVertex2f(0.0,0.0);
  	glEnd();
  	glFlush();
}

void clipping(float xstart,float ystart,float xfinish,float yfinish)
{
	Points start,finish;
	Points start_down,finish_up;

	start.k=xstart;
	start.l=ystart;	

	finish.k=xfinish;
	finish.l=yfinish;

	start_down.k=xstart;
	start_down.l=yfinish;
	
	finish_up.k=xfinish;
	finish_up.l=ystart;
	
	float xmax,ymax,xmin,ymin;
	if(start.k>finish.k)
	{
		xmax=start.k;
		xmin=finish.k;
	}	
	else
	{
		xmin=start.k;
		xmax=finish.k;
	}
	if(start.l>finish.l)
	{
		ymax=start.l;
		ymin=finish.l;
	}	
	else
	{
		ymin=start.l;
		ymax=finish.l;
	}
	
	// Defining polygon vertices in clockwise order
	int poly_size = array[polygon_num].counter;
	int poly_points[20][2];
	int j;
	for(j=0;j<poly_size;j++)
	{
		poly_points[j][0]=array[polygon_num].azor[j].k;
		poly_points[j][1]=array[polygon_num].azor[j].l;
	}
	
	int clipper_size = 4; 
	int clipper_points[][2] = {{(int)start.k,(int)start.l},{(int)finish_up.k,(int)finish_up.l},
												{(int)finish.k,(int)finish.l},{(int)start_down.k,(int)start_down.l}};

		//Calling the clipping function 
  	suthHodgClip(poly_points, poly_size, clipper_points, clipper_size);
  	
  	clearClip(xmin,xmax,ymin,ymax);	
		glBegin(GL_LINES);
		glColor3f(0.85f,0.10f,0.33f);					//baurdaux
		glVertex2f(start.k,start.l);
		glVertex2f(finish_up.k,finish_up.l);
		
		glVertex2f(finish_up.k,finish_up.l);
		glVertex2f(finish.k,finish.l);
		
		glVertex2f(finish.k,finish.l);
		glVertex2f(start_down.k,start_down.l);

		glVertex2f(start_down.k,start_down.l);
		glVertex2f(start.k,start.l);
		glEnd();
		glFlush();
}

void copyPoly(polygon p)
{
		Vector2dVector a;
		int i;
		for(i=0;i<p.counter;i++){
			a.push_back(Vector2d(p.azor[i].k,p.azor[i].l));
		}
		Vector2dVector result;

		//  Invoke the triangulator to triangulate this polygon.
		Triangulate::Process(a,result);

		// print out the results.
		int tcount = result.size()/3;

		for (int i=0; i<tcount; i++)
		{
				const Vector2d &p1 = result[i*3+0];
				const Vector2d &p2 = result[i*3+1];
				const Vector2d &p3 = result[i*3+2];

				Points q1,q2,q3;
				q1.k=p1.GetX();
				q1.l=p1.GetY();

				q2.k=p2.GetX();
				q2.l=p2.GetY();

				q3.k=p3.GetX();
				q3.l=p3.GetY();
				if(destroy == 0 && destroy_two == 0)
				{	
						array[polygon_num].rex[array[polygon_num].tricounter].a=q1;
						array[polygon_num].rex[array[polygon_num].tricounter].b=q2;
						array[polygon_num].rex[array[polygon_num].tricounter].c=q3;
						array[polygon_num].tricounter++;
				}
		}
}
void keyboard(unsigned char key,int x,int y)
{
		switch(key)
		{
			case 27:
				exit(0);
			case 84:
				tpressed=1;
				appear=!appear;
				break;
		}
}
void pressKey(int key, int x, int y)
{
		switch (key)
		{
			case GLUT_KEY_UP: 
				deltaMove = 3.5f; 
				printf("\n");
				break;
			case GLUT_KEY_DOWN: 
				deltaMove = -3.5f; 
				printf("\n");
				break;
		}
}

void releaseKey(int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_UP:
		case GLUT_KEY_DOWN: deltaMove = 0; break;
	}
}

void mouseMove(int x, int y)
{
	// this will only be true when the left button is down
	if (xOrigin >= 0)
	{
		// update deltaAngle
		deltaAngle = (x - xOrigin) * 0.001f;

		// update camera's direction
		lx = sin(angle + deltaAngle);
		lz = -cos(angle + deltaAngle);
	}
}

void mouse(int button, int state, int mousex, int mousey)
{
		if(flag==1 )
		{
			if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
			{
				check = 1;
				x = mousex;
				y = 500-mousey;
			}
			if(button==GLUT_LEFT_BUTTON && state==GLUT_UP)
			{
				check = 0; 			//so that click doesnt count as two once on release
			}
		}
		if(clipflag == 1)
		{
			if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
			{
				klik=1;
				x = mousex;
				y = 500-mousey;
			}
			if(button==GLUT_LEFT_BUTTON && state==GLUT_UP)
			{
				klik = 0; 			//so that click doesnt count as two once on release
			}
		}
		if(extrude)
		{
			if (button == GLUT_LEFT_BUTTON)
			{
				// when the button is released
				if (state == GLUT_UP)
				{
					angle += deltaAngle;
					xOrigin = -1;
				}
				// state = GLUT_DOWN
				else{
					xOrigin = x;
				}
			}
		}
		if(button==GLUT_RIGHT_BUTTON && state==GLUT_DOWN){
			close = 1;
		}
		glutPostRedisplay();
}
void processMenuEvents(int option)
{
	switch (option)
	{
		case 1:
			extrude = 1;
			break;
		case 2:
			exit(0);
			break;
		case 3:
			flag = 1;
			if(polygon_num==1000)exit(0);
			polygon_num++;
			if(fillflag == 0)
			{
				array[polygon_num].color_fill[0]=1.0f;
				array[polygon_num].color_fill[1]=1.0f;
				array[polygon_num].color_fill[2]=1.0f;
			}
			if(lineflag == 0)
			{
				array[polygon_num].color_lines[0]=0.0f;
				array[polygon_num].color_lines[1]=0.0f;
				array[polygon_num].color_lines[2]=0.0f;
			}
			break;
		case 4:
			clipflag = 1;
			drawRect=1;
			clipped = 1;
			new_action = 1;
			w=0;
			break;
		case 5:
			array[polygon_num+1].color_lines[0]=1.0f;
			array[polygon_num+1].color_lines[1]=1.0f;
			array[polygon_num+1].color_lines[2]=1.0f;
			lineflag=1;
			break;
		case 6:
			array[polygon_num+1].color_lines[0]=0.0f;
			array[polygon_num+1].color_lines[1]=0.0f;
			array[polygon_num+1].color_lines[2]=0.0f;
			lineflag=1;
			break;
		case 7:
			array[polygon_num+1].color_lines[0]=1.0f;
			array[polygon_num+1].color_lines[1]=0.0f;
			array[polygon_num+1].color_lines[2]=0.0f;
			lineflag=1;
			break;
		case 8:
			array[polygon_num+1].color_lines[0]=0.0f;
			array[polygon_num+1].color_lines[1]=0.5f;
			array[polygon_num+1].color_lines[2]=0.0f;
			lineflag=1;			
			break;
		case 9:
			array[polygon_num+1].color_lines[0]=0.75f;
			array[polygon_num+1].color_lines[1]=0.75f;
			array[polygon_num+1].color_lines[2]=0.75f;
			lineflag=1;
			break;
		case 10:
			array[polygon_num+1].color_lines[0]=0.0f;
			array[polygon_num+1].color_lines[1]=0.0f;
			array[polygon_num+1].color_lines[2]=1.0f;
			lineflag=1;
			break;
		case 11:
			array[polygon_num+1].color_lines[0]=1.0f;
			array[polygon_num+1].color_lines[1]=1.0f;
			array[polygon_num+1].color_lines[2]=0.0f;
			lineflag=1;
			break;
		case 12:
			array[polygon_num+1].color_lines[0]=1.0f;
			array[polygon_num+1].color_lines[1]=0.0f;
			array[polygon_num+1].color_lines[2]=1.0f;
			lineflag=1;
			break;
		case 13:
			array[polygon_num+1].color_lines[0]=1.0f;
			array[polygon_num+1].color_lines[1]=0.5f;
			array[polygon_num+1].color_lines[2]=0.0f;
			lineflag=1;
			break;
		case 14:
			array[polygon_num+1].color_lines[0]=0.89f;
			array[polygon_num+1].color_lines[1]=0.47f;
			array[polygon_num+1].color_lines[2]=0.20f;
			lineflag=1;
			break;
		case 15:
			array[polygon_num+1].color_lines[0]=0.0f;
			array[polygon_num+1].color_lines[1]=1.0f;
			array[polygon_num+1].color_lines[2]=1.0f;
			lineflag=1;
			break;
		case 16:
			array[polygon_num+1].color_lines[0]=0.5f;
			array[polygon_num+1].color_lines[1]=1.0f;
			array[polygon_num+1].color_lines[2]=0.5f;
			lineflag=1;
			break;
		case 17:
			array[polygon_num+1].color_lines[0]=0.5f;
			array[polygon_num+1].color_lines[1]=1.0f;
			array[polygon_num+1].color_lines[2]=0.0f;
			lineflag=1;
			break;
		case 18:
			array[polygon_num+1].color_lines[0]=0.55f;
			array[polygon_num+1].color_lines[1]=0.47f;
			array[polygon_num+1].color_lines[2]=0.14f;
			lineflag=1;
			break;
		case 19:
			array[polygon_num+1].color_lines[0]=0.85f;
			array[polygon_num+1].color_lines[1]=0.85f;
			array[polygon_num+1].color_lines[2]=0.95f;
			lineflag=1;
			break;
		case 20:
			array[polygon_num+1].color_lines[0]=1.0f;
			array[polygon_num+1].color_lines[1]=0.43f;
			array[polygon_num+1].color_lines[2]=0.78f;
			lineflag=1;
			break;
			case 21:
			array[polygon_num+1].color_fill[0]=1.0f;
			array[polygon_num+1].color_fill[1]=1.0f;
			array[polygon_num+1].color_fill[2]=1.0f;
			fillflag=1;
			break;
		case 22:
			array[polygon_num+1].color_fill[0]=0.0f;
			array[polygon_num+1].color_fill[1]=0.0f;
			array[polygon_num+1].color_fill[2]=0.0f;
			fillflag=1;
			break;
		case 23:
			array[polygon_num+1].color_fill[0]=1.0f;
			array[polygon_num+1].color_fill[1]=0.0f;
			array[polygon_num+1].color_fill[2]=0.0f;
			fillflag=1;
			break;
		case 24:
			array[polygon_num+1].color_fill[0]=0.0f;
			array[polygon_num+1].color_fill[1]=0.5f;
			array[polygon_num+1].color_fill[2]=0.0f;
			fillflag=1;
			break;
		case 25:
			array[polygon_num+1].color_fill[0]=0.75f;
			array[polygon_num+1].color_fill[1]=0.75f;
			array[polygon_num+1].color_fill[2]=0.75f;
			fillflag=1;
			break;
		case 26:
			array[polygon_num+1].color_fill[0]=0.0f;
			array[polygon_num+1].color_fill[1]=0.0f;
			array[polygon_num+1].color_fill[2]=1.0f;
			fillflag=1;
			break;
		case 27:
			array[polygon_num+1].color_fill[0]=1.0f;
			array[polygon_num+1].color_fill[1]=1.0f;
			array[polygon_num+1].color_fill[2]=0.0f;
			fillflag=1;
			break;
		case 28:
			array[polygon_num+1].color_fill[0]=1.0f;
			array[polygon_num+1].color_fill[1]=0.0f;
			array[polygon_num+1].color_fill[2]=1.0f;
			fillflag=1;
			break;
		case 29:
			array[polygon_num+1].color_fill[0]=1.0f;
			array[polygon_num+1].color_fill[1]=0.5f;
			array[polygon_num+1].color_fill[2]=0.0f;
			fillflag=1;
			break;
		case 30:
			array[polygon_num+1].color_fill[0]=0.89f;
			array[polygon_num+1].color_fill[1]=0.47f;
			array[polygon_num+1].color_fill[2]=0.20f;
			fillflag=1;
			break;
		case 31:
			array[polygon_num+1].color_fill[0]=0.0f;
			array[polygon_num+1].color_fill[1]=1.0f;
			array[polygon_num+1].color_fill[2]=1.0f;
			fillflag=1;
			break;
		case 32:
			array[polygon_num+1].color_fill[0]=0.5f;
			array[polygon_num+1].color_fill[1]=1.0f;
			array[polygon_num+1].color_fill[2]=0.5f;
			fillflag=1;
			break;
		case 33:
			array[polygon_num+1].color_fill[0]=0.5f;
			array[polygon_num+1].color_fill[1]=1.0f;
			array[polygon_num+1].color_fill[2]=0.0f;
			fillflag=1;
			break;
		case 34:
			array[polygon_num+1].color_fill[0]=0.55f;
			array[polygon_num+1].color_fill[1]=0.47f;
			array[polygon_num+1].color_fill[2]=0.14f;
			fillflag=1;
			break;
		case 35:
			array[polygon_num+1].color_fill[0]=0.85f;
			array[polygon_num+1].color_fill[1]=0.85f;
			array[polygon_num+1].color_fill[2]=0.95f;
			fillflag=1;
			break;
		case 36:
			array[polygon_num+1].color_fill[0]=1.0f;
			array[polygon_num+1].color_fill[1]=0.43f;
			array[polygon_num+1].color_fill[2]=0.78f;
			fillflag=1;
			break;
	}
}

void createGLUTMenus()
{
		int menu,submenu_id,linecolor_id,fillcolor_id;
	
		submenu_id = glutCreateMenu(processMenuEvents);
		glutAddMenuEntry("POLYGON",3);
		glutAddMenuEntry("EXIT",2);
		glutAddMenuEntry("CLIPPING",4);
		glutAddMenuEntry("EXTRUDE",1);
	
		linecolor_id = glutCreateMenu(processMenuEvents);
		glutAddMenuEntry("WHITE",5);
		glutAddMenuEntry("BLACK",6);		
		glutAddMenuEntry("RED",7);
		glutAddMenuEntry("DARK GREEN",8);
		glutAddMenuEntry("GREY",9);
		glutAddMenuEntry("BLUE",10);
		glutAddMenuEntry("YELOW",11);
		glutAddMenuEntry("PURPLE",12);
		glutAddMenuEntry("ORANGE",13);
		glutAddMenuEntry("MANDARIN ORANGE",14);
		glutAddMenuEntry("CYAN",15);
		glutAddMenuEntry("LIGHT GREEN",16);
		glutAddMenuEntry("GREENISH YELLOW",17);
		glutAddMenuEntry("BRONZE",18);
		glutAddMenuEntry("QUARTZ",19);
		glutAddMenuEntry("NEONPINK",20);		
	
		fillcolor_id=glutCreateMenu(processMenuEvents);
	
		glutAddMenuEntry("WHITE",21);
		glutAddMenuEntry("BLACK",22);		
		glutAddMenuEntry("RED",23);
		glutAddMenuEntry("DARK GREEN",24);
		glutAddMenuEntry("GREY",25);
		glutAddMenuEntry("BLUE",26);
		glutAddMenuEntry("YELOW",27);
		glutAddMenuEntry("PURPLE",28);
		glutAddMenuEntry("ORANGE",29);
		glutAddMenuEntry("MANDARIN ORANGE",30);
		glutAddMenuEntry("CYAN",31);
		glutAddMenuEntry("LIGHT GREEN",32);
		glutAddMenuEntry("GREENISH YELLOW",33);
		glutAddMenuEntry("BRONZE",34);
		glutAddMenuEntry("QUARTZ",35);
		glutAddMenuEntry("NEONPINK",36);	
	
		menu = glutCreateMenu(processMenuEvents);
		glutAddSubMenu("ACTION", submenu_id);
		glutAddSubMenu("LINE_COLOR", linecolor_id);
		glutAddSubMenu("FILL_COLOR", fillcolor_id);

		glutAttachMenu(GLUT_MIDDLE_BUTTON);			// attach the menu to the middle button
}

int main(int argc, char** argv)
{
		glutInit(&argc, argv); 								// Initialize GLUT
		init();	
		glutInitWindowSize(600, 500);					// Set the window's initial width & height 
		glutInitWindowPosition(50, 50); 			// Position the window's initial top-left corner
		glutCreateWindow("Window"); 					// Create window with the given title 	
		glClearColor(1, 1, 1, 0); 						// sets the backgraound color to white light
		glClear(GL_COLOR_BUFFER_BIT); 				// clears the frame buffer and set values defined in glClearColor() function call 
		glutDisplayFunc(display);							// calls display function
		glutIdleFunc(display3D);							// use for movement in 3D mode
		glutIgnoreKeyRepeat(1);
		glutKeyboardFunc(keyboard);						
		glutSpecialFunc(pressKey);						// arrow key movement in 3D mode
		glutSpecialUpFunc(releaseKey);
		glutMouseFunc(mouse);
		glutMotionFunc(mouseMove);						//camera movement in 3D mode
		glEnable(GL_DEPTH_TEST);				
		createGLUTMenus();		
		glutMainLoop();
		return 0;
}
