/*
Programul afiseaza un patrat pe care il translateaza 
pe axa x la apasarea sagetilor stanga, dreapta
*/
#include "glos.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glaux.h>
#include <math.h>

void myinit(void);
void CALLBACK display(void);
void CALLBACK myReshape(GLsizei w, GLsizei h);
void CALLBACK MutaStanga(void);
void CALLBACK RotireDreapta(AUX_EVENTREC *event);
void CALLBACK RotireStanga(AUX_EVENTREC *event);


static GLUquadricObj *q;
static GLfloat x=0;
static GLfloat y = 0,z=0,alfa = 0;
static int beta=0 ,gama=0;
static GLfloat pozSursa[] = { 60.0, 200.0, -50.0, 1.0 };


void calcCoeficientiPlan(float P[3][3], float coef[4])
{
	float v1[3], v2[3];
	float length;
	static const int x = 0;
	static const int y = 1;
	static const int z = 2;

	//calculeaza doi vectori din trei puncte
	v1[x] = P[0][x] - P[1][x];
	v1[y] = P[0][y] - P[1][y];
	v1[z] = P[0][z] - P[1][z];

	v2[x] = P[1][x] - P[2][x];
	v2[y] = P[1][y] - P[2][y];
	v2[z] = P[1][z] - P[2][z];
	/*se calculeaza produsul vectorial al celor doi vectori
	care reprezinta un al treilea vector perpendicular pe plan
	ale carui componente sunt chiar coeficienti A, B, C din ecuatia planului */
	coef[x] = v1[y] * v2[z] - v1[z] * v2[y];
	coef[y] = v1[z] * v2[x] - v1[x] * v2[z];
	coef[z] = v1[x] * v2[y] - v1[y] * v2[x];

	//normalizeaza vectorul
	length = (float)sqrt((coef[x] * coef[x]) + (coef[y] * coef[y]) + (coef[z] * coef[z]));
	coef[x] /= length;
	coef[y] /= length;
	coef[z] /= length;
}
void MatriceUmbra(GLfloat puncte[3][3], GLfloat pozSursa[4], GLfloat mat[4][4])
{
	GLfloat coefPlan[4];
	GLfloat temp;
	//determina coeficientii planului
	calcCoeficientiPlan(puncte, coefPlan);
	//determina si pe D
	coefPlan[3] = -((coefPlan[0] * puncte[2][0]) + (coefPlan[1] * puncte[2][1]) + (coefPlan[2] * puncte[2][2]));
	//temp=A*xL+B*yL+C*zL+D*w
	temp = coefPlan[0] * pozSursa[0] + coefPlan[1] * pozSursa[1] + coefPlan[2] * pozSursa[2] + coefPlan[3] * pozSursa[3];
	//prima coloana
	mat[0][0] = temp - pozSursa[0] * coefPlan[0];
	mat[1][0] = 0.0f - pozSursa[0] * coefPlan[1];
	mat[2][0] = 0.0f - pozSursa[0] * coefPlan[2];
	mat[3][0] = 0.0f - pozSursa[0] * coefPlan[3];
	//a doua coloana
	mat[0][1] = 0.0f - pozSursa[1] * coefPlan[0];
	mat[1][1] = temp - pozSursa[1] * coefPlan[1];
	mat[2][1] = 0.0f - pozSursa[1] * coefPlan[2];
	mat[3][1] = 0.0f - pozSursa[1] * coefPlan[3];
	//a treia coloana
	mat[0][2] = 0.0f - pozSursa[2] * coefPlan[0];
	mat[1][2] = 0.0f - pozSursa[2] * coefPlan[1];
	mat[2][2] = temp - pozSursa[2] * coefPlan[2];
	mat[3][2] = 0.0f - pozSursa[2] * coefPlan[3];
	//a patra coloana
	mat[0][3] = 0.0f - pozSursa[3] * coefPlan[0];
	mat[1][3] = 0.0f - pozSursa[3] * coefPlan[1];
	mat[2][3] = 0.0f - pozSursa[3] * coefPlan[2];
	mat[3][3] = temp - pozSursa[3] * coefPlan[3];
}


void myinit (void) {
    glClearColor(0.0f, 0.4f, 1.0f, 1.0f);
	
	glEnable(GL_MAP2_VERTEX_3);	//se valideaza un anumit tip de evaluare
	//glShadeModel(GL_SMOOTH); 	//umbrire constanta pe poligoane
	glLineStipple(1, 0x0F0F);

		GLfloat light_position[] = { 10.0f, 10.0f, 0.0f, 0.1f};
    glLightfv(GL_LIGHT0, GL_POSITION, pozSursa);
	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
		//GLfloat light_diffuse[] = { 0.75, 0.75, 0.75, 10.0 };
	//	GLfloat ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		GLfloat mat_shininess[] = { 128.0f };
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
//	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, pozSursa);
	//seteaza materialul
	//glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glFrontFace(GL_CW);//orientarea implicit[ a vârfurilor în sensul rotirii acelor
	glEnable(GL_LIGHTING); //activare iluminare
	glEnable(GL_LIGHT0); //activare sursa 0
	glEnable(GL_AUTO_NORMAL); //activare calculare normale daca vârfurile 
				  //s-au determinat cu GL_MAP2_VERTEX_3 sau GL_MAP2_VERTEX_4
	glEnable(GL_NORMALIZE); //activare normalizare (vectori unitari) vectori
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST); //activare test adâncime
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
}

void CALLBACK MutaStanga(void)
{
	x=x-10;
}

void CALLBACK MutaDreapta(void)
{
	x=x+10;
}
void CALLBACK RotireStanga(AUX_EVENTREC *event)
{
	alfa = alfa + 10;
	
}
void CALLBACK RotireDreapta(AUX_EVENTREC *event)
{
	alfa = alfa - 10;

}
void CALLBACK RotireInFata(void)
{
	gama = gama - 15;

}
void CALLBACK RotireInSpate(void)
{
	gama = gama + 15;

}
void CALLBACK plan()
{
	glTranslatef(x, y-30, z);
	glColor3f(0.0f, 0.7f, 0.0f);
	glRotatef(gama, 1, 0, 0);

	glBegin(GL_QUAD_STRIP);

	glVertex3f(-300.0f, -150.0f, 200.0f);
	glVertex3f(-300.0f, -160.0f, 200.0f);
	glVertex3f(300.0f, -150.0f, 200.0f);
	glVertex3f(300.0f, -160.0f, 200.0f);
	glVertex3f(300.0f, -150.0f, -200.0f);
	glVertex3f(300.0f, -160.0f, -200.0f);
	glVertex3f(-300.0f, -150.0f, -200.0f);
	glVertex3f(-300.0f, -160.0f, -200.0f);
	glVertex3f(-300.0f, -150.0f, 200.0f);
	glVertex3f(-300.0f, -160.0f, 200.0f);

	glEnd();

	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_QUADS);

	glVertex3f(-300.0f, -150.0f, 200.0f);
	glVertex3f(300.0f, -150.0f, 200.0f);
	glVertex3f(300.0f, -150.0f, -200.0f);
	glVertex3f(-300.0f, -150.0f, -200.0f);

	glEnd();


	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_QUADS);

	glVertex3f(-300.0f, -160.0f, -200.0f);
	glVertex3f(300.0f, -160.0f, -200.0f);
	glVertex3f(300.0f, -160.0f, 200.0f);
	glVertex3f(-300.0f, -160.0f, 200.0f);

	glEnd();
	
		/*glBegin(GL_QUADS);
	glVertex3f(500.0f, -165.0f, -500.0f);
	glVertex3f(-500.0f, -165.0f, -500.0f);
	glVertex3f(-500.0f, -165.0f, 500.0f);
	glVertex3f(500.0f, -165.0f, 500.0f);
	glEnd();*/
}
void CALLBACK all(void)
{ 
	GLfloat matUmbra[4][4];
	//oricare trei puncte din plan in sens CCW
	GLfloat puncte[3][3] = { { -50.0f,-164.0f,-50.0f },
	{ -50.0f,-164.0f,50.0f },{ 50.0f,-164.0f,50.0f } };
	//sterge bufferul de culoare si de adancime
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//determina matricea pentru calcularea umbrei
	MatriceUmbra(puncte, pozSursa, matUmbra);
	//salveaza starea matricei de modelare-vizualizare si se reprezinta cubul rotit
	glPushMatrix();
	glLightfv(GL_LIGHT0, GL_POSITION, pozSursa);
	//corp moara
	q = gluNewQuadric();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluQuadricDrawStyle(q, GLU_FILL);
	glRotatef(alfa, 0, 1, 0);
	glRotatef(gama, 1, 0, 0);
	//glPushMatrix();

	//corpul moarei
	glPushMatrix();
	glColor3f(0.8f, 0.8f, 0.8f);
	glTranslatef(x + 10, y + 10, z + 50);
	glRotatef(90, 1, 0, 0);
	// axa x pe orizontala, axa z pe verticala si axa y spre mine
	gluCylinder(q, 35, 60, 190, 8, 10);
	glPopMatrix();

	glPushMatrix();
	//acoperisul moarei
	glColor3f(0.5f, 0.5f, 0.0f);
	glTranslatef(x + 10, y + 60, z + 50);
	glRotatef(90, 1, 0, 0);
	//glRotatef(alfa, 0, 0, 1);
	gluCylinder(q, 3, 50, 50, 100, 10);


	glColor3f(0.0f, 0.0f, 0.0f);
	glTranslatef(x, y + 55, z + 20);
	glRotatef(90, 1, 0, 0);
	glRotatef((beta % 360) + 40, 0, 0, 1);
	gluDisk(q, 5, 8, 200, 3);

	glPushMatrix();
	glColor3f(0.3f, 0.0f, 0.0f);
	glTranslatef(x, y, z - 5);
	//glRotatef(alfa, 0, 0, 1 );
	gluCylinder(q, 5, 5, 50, 1000, 50);
	glPopMatrix();

	//palete
	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(x + 5, y, z);
	glRotatef(90, 0, 1, 0);
	glRotatef(15, 0, 0, 1);
	//glRotatef(alfa, 0, 0, 1 );
	gluCylinder(q, 5, 30, 100, 2, 5);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(x - 5, y, z);
	glRotatef(-90, 0, 1, 0);
	glRotatef(15, 0, 0, 1);
	gluCylinder(q, 5, 30, 100, 2, 5);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(x, y - 5, z);
	glRotatef(90, 1, 0, 0);
	glRotatef(105, 0, 0, 1);
	gluCylinder(q, 5, 30, 100, 2, 5);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(x, y + 5, z);
	glRotatef(-90, 1, 0, 0);
	glRotatef(105, 0, 0, 1);
	gluCylinder(q, 5, 30, 100, 2, 5);
	glPopMatrix();

	//glPopMatrix();
	glPopMatrix();//reface matricea
				  //deseneaza umbra
				  //mai intai de dezactiveaza iluminarea si se salveaza starea matricei de proiectie
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glPushMatrix();
	//se inmulteste matricea curenta de modelare-vizualizare cu maricea de umbrire
	glMultMatrixf((GLfloat*)matUmbra);
	//se deseneaza cubul proiectat pe plan-adica umbra cubului cu gri
	glColor3f(0.7f, 0.7f, 0.7f);


	//------------//umbra moara//--------------//

	//corpul moarei

	glPushMatrix();
	glColor3f(0.2f, 0.2f, 0.2f);
	glTranslatef(x + 10, y + 10, z + 50);
	glRotatef(90, 1, 0, 0);
	// axa x pe orizontala, axa z pe verticala si axa y spre mine
	gluCylinder(q, 35, 60, 190, 8, 10);
	glPopMatrix();

	glPushMatrix();
	//acoperisul moarei
	glColor3f(0.2f, 0.2f, 0.2f);
	glTranslatef(x + 10, y + 60, z + 50);
	glRotatef(90, 1, 0, 0);
	//glRotatef(alfa, 0, 0, 1);
	gluCylinder(q, 3, 50, 50, 100, 10);


	glColor3f(0.2f, 0.2f, 0.2f);
	glTranslatef(x, y + 55, z + 20);
	glRotatef(90, 1, 0, 0);
	glRotatef((beta % 360) + 40, 0, 0, 1);
	gluDisk(q, 5, 8, 200, 3);

	glPushMatrix();
	glColor3f(0.2f, 0.2f, 0.2f);
	glTranslatef(x, y, z - 5);
	//glRotatef(alfa, 0, 0, 1 );
	gluCylinder(q, 5, 5, 50, 1000, 50);
	glPopMatrix();

	//palete
	glPushMatrix();
	glColor3f(0.2f, 0.2f, 0.2f);
	glTranslatef(x + 5, y, z);
	glRotatef(90, 0, 1, 0);
	glRotatef(15, 0, 0, 1);
	glRotatef(alfa, 0, 0, 1 );
	gluCylinder(q, 5, 30, 100, 2, 5);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.2f, 0.2f, 0.2f);
	glTranslatef(x - 5, y, z);
	glRotatef(-90, 0, 1, 0);
	glRotatef(15, 0, 0, 1);
	gluCylinder(q, 5, 30, 100, 2, 5);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.2f, 0.2f, 0.2f);
	glTranslatef(x, y - 5, z);
	glRotatef(90, 1, 0, 0);
	glRotatef(105, 0, 0, 1);
	gluCylinder(q, 5, 30, 100, 2, 5);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.2f, 0.2f, 0.2f);
	glTranslatef(x, y + 5, z);
	glRotatef(-90, 1, 0, 0);
	glRotatef(105, 0, 0, 1);
	gluCylinder(q, 5, 30, 100, 2, 5);
	glPopMatrix();

	gluDeleteQuadric(q);
	glPopMatrix();
	glPopMatrix();//reface matricea de proiectie
				  //deseneaza sursa de lumina ca o sfera mica galbena in pozitia reala
	glPushMatrix();
	glTranslatef(pozSursa[0], pozSursa[1], pozSursa[2]);
	glColor3f(1.0f, 1.0f, 0.0f);
	auxSolidSphere(10);
	glPopMatrix();
	
	//restaureaza starea variabilelor de iluminare
	glPopAttrib();
	glFlush();
}


void CALLBACK display (void)
{
	q = gluNewQuadric();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	
	gluQuadricDrawStyle(q, GLU_LINE);
 glRotatef(alfa, 0, 1, 0);
  GLfloat more_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };

	all();
	glPushMatrix();
	glColor3f(0.0f, 1.0f, 0.0f);
	plan();
	glPopMatrix();


	glMaterialfv(GL_FRONT, GL_AMBIENT, more_ambient);
    glFlush();
}
void CALLBACK IdleFunction(void) {
	beta += 5;
	display();
	Sleep(100);
}

/*void CALLBACK myReshape(GLsizei w, GLsizei h)
{
    if (!h) return;			//transformare anizotropica, forma se modifica functie de forma(dimens) viewportului
    glViewport(0, 0, w, h);	//daca w>h stabilim ca baza inaltime, si stab unit logica de dimens in fct de h(h/320, 320 lungime lat patrat)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho (-160.0, 160.0, -160.0, 
        160.0, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
}*/

void CALLBACK myReshape(GLsizei w, GLsizei h)	
{
    if (!h) return;
    glViewport(0, 0, w, h);	
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)		
    glOrtho (-250.0, 250.0, 250.0*(GLfloat)h/(GLfloat)w, 
        -250.0*(GLfloat)h/(GLfloat)w, -350.0, 350.0);
    else 
    glOrtho (-250.0*(GLfloat)w/(GLfloat)h, 
       250.0*(GLfloat)w/(GLfloat)h, -250.0, 250.0, -350.0, 350.0);
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(-30.0,-1.0,1.0,1.0);
}

int main(int argc, char** argv)
{
	auxInitDisplayMode(AUX_SINGLE | AUX_RGB | AUX_DEPTH16);
    auxInitPosition (0, 0, 1200, 700);
    auxInitWindow ("Moara ce-si invarte paletele");
    myinit ();
	auxKeyFunc (AUX_DOWN, RotireInFata);
	auxKeyFunc (AUX_UP, RotireInSpate);
	auxMouseFunc(AUX_LEFTBUTTON, AUX_MOUSEDOWN, RotireStanga);
	auxMouseFunc(AUX_RIGHTBUTTON, AUX_MOUSEDOWN, RotireDreapta);

	auxIdleFunc(IdleFunction);
    auxReshapeFunc (myReshape);
    auxMainLoop(display);
    return(0);
}
