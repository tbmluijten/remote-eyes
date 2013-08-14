#pragma comment(linker, "/NODEFAULTLIB:libc.lib")
#include <stdlib.h>
#include <string>
#include <fstream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glut.h>    // Header File For The GLUT Library 
#include <GL/gl.h>	// Header File For The OpenGL32 Library
#include <GL/glu.h>	// Header File For The GLu32 Library
// #include <unistd.h>     // Header File For sleeping.
#include <opencv/cv.h>
#include <opencv/highgui.h>


/* ASCII code for the escape key. */
#define ESCAPE 27

/* The number of our GLUT window */
int window; 

CvCapture* capture;

void InitGL(int Width, int Height);
void DrawGLScene();
void keyPressed(unsigned char key, int x, int y);
void ReSizeGLScene(int Width, int Height);

GLuint shaderProgram, vertexShader, fragmentShader;

int main(int argc, char **argv) 
{  
  glutInit(&argc, argv);  

  /* Select type of Display mode:   
     Double buffer 
     RGBA color
     Alpha components supported 
     Depth buffer */  
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);  

  /* get a 640 x 480 window */
  glutInitWindowSize(640, 480);  

  /* the window starts at the upper left corner of the screen */
  glutInitWindowPosition(0, 0);  

  /* Open a window */  
  window = glutCreateWindow("FPV Viewer");  

  /* Register the function to do all our OpenGL drawing. */
  glutDisplayFunc(&DrawGLScene);  

  /* Go fullscreen.  This is the soonest we could possibly go fullscreen. */
  // glutFullScreen();

  /* Even if there are no events, redraw our gl scene. */
  glutIdleFunc(&DrawGLScene);

  /* Register the function called when our window is resized. */
  glutReshapeFunc(&ReSizeGLScene);

  /* Register the function called when the keyboard is pressed. */
  glutKeyboardFunc(&keyPressed);

  /* Initialize our window. */
  InitGL(640, 480);
  
  /* Start Event Processing Engine */  
  glutMainLoop();  

  return 1;
}

std::string get_file_contents(const char *filename)
{
  std::ifstream in(filename, std::ios::in | std::ios::binary);
  if (in)
  {
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    return(contents);
  }
  // throw(errno);
}

/* A general OpenGL initialization function.  Sets all of the initial parameters. */
void InitGL(int Width, int Height)	        // We call this right after our OpenGL window is created.
{
	unsigned char data[720*480*3];
	int c = 0;
	
	unsigned int textureID;
	char log[2000];

	std::string VShaderSrc = get_file_contents("vshader.txt");
	const char* VShaderString = VShaderSrc.c_str();
	int VShaderLength = VShaderSrc.length();

	std::string FShaderSrc = get_file_contents("fshader.txt");
	const char* FShaderString = FShaderSrc.c_str();
	int FShaderLength = FShaderSrc.length();

	glShadeModel(GL_SMOOTH);
	glClearColor(0,0,0,0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// init cam capture
	capture = cvCaptureFromCAM( CV_CAP_ANY );
	if ( !capture ) {
	     fprintf( stderr, "ERROR: capture is NULL \n" );
	     getchar();
	     return;
	}

	//generate a texture ID and bind to texture
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);


	// create a texture for the image...
	glTexImage2D(GL_TEXTURE_2D,            	 //target
				 0,                         //level of detail
				 3,                         //colour components
				 720,                       //width
				 480,                       //height
				 0,                         //border
				 GL_RGB,                    //image format
				 GL_UNSIGNED_BYTE,
				 data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glEnable(GL_TEXTURE_2D);

	// SHADER
	glewInit();
	shaderProgram = glCreateProgram();
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	//glShaderSource(vertexShader, 1, &VShaderString, &VShaderLength);
	//glCompileShader(vertexShader);
	// glAttachShader(shaderProgram, vertexShader);

	glGetShaderInfoLog(vertexShader, 2000, NULL, log);
	printf("%s", log);

	glShaderSource(fragmentShader, 1, &FShaderString, &FShaderLength);
	glCompileShader(fragmentShader);
	glAttachShader(shaderProgram, fragmentShader);

	glGetShaderInfoLog(fragmentShader, 2000, NULL, log);
	printf("%s", log);

	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);
}

/* The function called when our window is resized (which shouldn't happen, because we're fullscreen) */
void ReSizeGLScene(int Width, int Height)
{
  if (Height==0)				// Prevent A Divide By Zero If The Window Is Too Small
    Height=1;

  glViewport(0, 0, Width, Height);		// Reset The Current Viewport And Perspective Transformation

  glMatrixMode(GL_MODELVIEW);


	GLint loc = glGetUniformLocation(shaderProgram, "resolution");
	if (loc != -1)
	{
	   glUniform2f(loc, Width, Height);
	}
}

// unsigned char data[640*480*3];
IplImage* frame;

/* The main drawing function. */
void DrawGLScene()
{

	/*
	int c = 0;
	for(c = 0; c < 640*480; c++) {
		data[c] = (unsigned char)rand();
	}
	*/

    frame = cvQueryFrame( capture );

     if ( !frame ) {
       fprintf( stderr, "ERROR: frame is null...\n" );
     }

  glTexSubImage2D(GL_TEXTURE_2D,
	                0,
			0,
			0,
			720,
			480,
			GL_BGR_EXT,
			GL_UNSIGNED_BYTE,
			frame->imageData);

  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer

  // glMatrixMode(GL_MODELVIEW);

  // glLoadIdentity();
  glBegin(GL_QUADS);
	glTexCoord2f(1,1);
	glVertex2f(-1,-1);

	glTexCoord2f(0,1);
	glVertex2f(1,-1);

	glTexCoord2f(0,0);
	glVertex2f(1,1);

	glTexCoord2f(1,0);
	glVertex2f(-1,1);
  glEnd();

  // glFlush();
  glutSwapBuffers();

}

/* The function called whenever a key is pressed. */
void keyPressed(unsigned char key, int x, int y) 
{
    /* avoid thrashing this procedure */
    // usleep(100);

    /* If escape is pressed, kill everything. */
    if (key == ESCAPE) 
    { 
	/* shut down our window */
	glutDestroyWindow(window); 
	
	/* exit the program...normal termination. */
	exit(0);                   
    }

	if(key == 'f') {
		glutFullScreen();
	}
}


