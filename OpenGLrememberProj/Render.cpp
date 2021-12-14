#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

double* normalVector(double x[3], double y[3], double z[3]) {
	double A[] = { x[0] - y[0], x[1] - y[1],x[2] - y[2] };
	double B[] = { x[0] - z[0], x[1] - z[1],x[2] - z[2] };
	double b = z[0];
	double nx = A[1] * B[2] - B[1] * A[2];
	double ny = B[0] * A[2] - A[0] * B[2];
	double nz = A[0] * B[1] - B[0] * A[1];
	double lenght = sqrt(pow(nx, 2) + pow(ny, 2) + pow(nz, 2));

	return  new double[3]{ nx / lenght,ny / lenght,nz / lenght };
}

//êëàññ äëÿ íàñòðîéêè êàìåðû
class CustomCamera : public Camera
{
public:
	//äèñòàíöèÿ êàìåðû
	double camDist;
	//óãëû ïîâîðîòà êàìåðû
	double fi1, fi2;

	
	//çíà÷íèÿ ìàñåðû ïî óìîë÷àíèþ
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//ñ÷èòàåò ïîçèöèþ êàìåðû, èñõîäÿ èç óãëîâ ïîâîðîòà, âûçûâàåòñÿ äâèæêîì
	void SetUpCamera()
	{
		//îòâå÷àåò çà ïîâîðîò êàìåðû ìûøêîé
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//ôóíêöèÿ íàñòðîéêè êàìåðû
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //ñîçäàåì îáúåêò êàìåðû


//Êëàññ äëÿ íàñòðîéêè ñâåòà
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//íà÷àëüíàÿ ïîçèöèÿ ñâåòà
		pos = Vector3(1, 1, 3);
	}

	
	//ðèñóåò ñôåðó è ëèíèè ïîä èñòî÷íèêîì ñâåòà, âûçûâàåòñÿ äâèæêîì
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//ëèíèÿ îò èñòî÷íèêà ñâåòà äî îêðóæíîñòè
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//ðèñóåì îêðóíîñòü
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ïàðàìåòðû èñòî÷íèêà ñâåòà
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// õàðàêòåðèñòèêè èçëó÷àåìîãî ñâåòà
		// ôîíîâîå îñâåùåíèå (ðàññåÿííûé ñâåò)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// äèôôóçíàÿ ñîñòàâëÿþùàÿ ñâåòà
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// çåðêàëüíî îòðàæàåìàÿ ñîñòàâëÿþùàÿ ñâåòà
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //ñîçäàåì èñòî÷íèê ñâåòà




//ñòàðûå êîîðäèíàòû ìûøè
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//ìåíÿåì óãëû êàìåðû ïðè íàæàòîé ëåâîé êíîïêå ìûøè
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//äâèãàåì ñâåò ïî ïëîñêîñòè, â òî÷êó ãäå ìûøü
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//âûïîëíÿåòñÿ ïåðåä ïåðâûì ðåíäåðîì
void initRender(OpenGL *ogl)
{
	//íàñòðîéêà òåêñòóð

	//4 áàéòà íà õðàíåíèå ïèêñåëÿ
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//íàñòðîéêà ðåæèìà íàëîæåíèÿ òåêñòóð
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//âêëþ÷àåì òåêñòóðû
	glEnable(GL_TEXTURE_2D);
	

	//ìàññèâ òðåõáàéòíûõ ýëåìåíòîâ  (R G B)
	RGBTRIPLE *texarray;

	//ìàññèâ ñèìâîëîâ, (âûñîòà*øèðèíà*4      4, ïîòîìó÷òî   âûøå, ìû óêàçàëè èñïîëüçîâàòü ïî 4 áàéòà íà ïèêñåëü òåêñòóðû - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//ãåíåðèðóåì ÈÄ äëÿ òåêñòóðû
	glGenTextures(1, &texId);
	//áèíäèì àéäèøíèê, âñå ÷òî áóäåò ïðîèñõîäèòü ñ òåêñòóðîé, áóäòå ïðîèñõîäèòü ïî ýòîìó ÈÄ
	glBindTexture(GL_TEXTURE_2D, texId);

	//çàãðóæàåì òåêñòóðó â âèäåîïÿìÿòü, â îïåðàòèâêå íàì áîëüøå  îíà íå íóæíà
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//îò÷èñòêà ïàìÿòè
	free(texCharArray);
	free(texarray);

	//íàâîäèì øìîí
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//êàìåðó è ñâåò ïðèâÿçûâàåì ê "äâèæêó"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// íîðìàëèçàöèÿ íîðìàëåé : èõ äëèíû áóäåò ðàâíà 1
	glEnable(GL_NORMALIZE);

	// óñòðàíåíèå ñòóïåí÷àòîñòè äëÿ ëèíèé
	glEnable(GL_LINE_SMOOTH); 


	//   çàäàòü ïàðàìåòðû îñâåùåíèÿ
	//  ïàðàìåòð GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ëèöåâûå è èçíàíî÷íûå ðèñóþòñÿ îäèíàêîâî(ïî óìîë÷àíèþ), 
	//                1 - ëèöåâûå è èçíàíî÷íûå îáðàáàòûâàþòñÿ ðàçíûìè ðåæèìàìè       
	//                ñîîòâåòñòâåííî ëèöåâûì è èçíàíî÷íûì ñâîéñòâàì ìàòåðèàëîâ.    
	//  ïàðàìåòð GL_LIGHT_MODEL_AMBIENT - çàäàòü ôîíîâîå îñâåùåíèå, 
	//                íå çàâèñÿùåå îò ñòî÷íèêîâ
	// ïî óìîë÷àíèþ (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}





void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//àëüôàíàëîæåíèå
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//íàñòðîéêà ìàòåðèàëà
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//ôîíîâàÿ
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//äèôóçíàÿ
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//çåðêàëüíàÿ
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//ðàçìåð áëèêà
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//÷òîá áûëî êðàñèâî, áåç êâàäðàòèêîâ (ñãëàæèâàíèå îñâåùåíèÿ)
	glShadeModel(GL_SMOOTH);
	//===================================
	//Ïðîãàòü òóò  prog here
	// figure start

	// нижняя плоскость
	double p1[] = { 0, 0, 0 };
	double p2[] = { 5, 6, 0 };
	double p3[] = { 3, 7, 0 };
	double p4[] = { 6, 10, 0 };
	double p5[] = { -1, 12, 0 };
	double p6[] = { 5, 14, 0 };

	glNormal3d(0, 0, -1);
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3dv(p1);
	glVertex3dv(p2);
	glVertex3dv(p3);
	glVertex3dv(p4);
	glVertex3dv(p5);
	glVertex3dv(p6);
	glEnd();

	

	double p7[] = { 6, 10, 0 };
	double p8[] = { 5, 6, 0 };
	double p9[] = { 11, 10, 0 };
	double p10[] = { 12, 2, 0 };

	glBegin(GL_TRIANGLE_STRIP);
	glVertex3dv(p7);
	glVertex3dv(p8);
	glVertex3dv(p9);
	glVertex3dv(p10);
	glEnd();

	// верхняя плоскость
	double t1[] = { 0, 0, 5 };
	double t2[] = { 5, 6, 5 };
	double t3[] = { 3, 7, 5 };
	double t4[] = { 6, 10, 5 };
	double t5[] = { -1, 12, 5 };
	double t6[] = { 5, 14, 5 };

	glBegin(GL_TRIANGLE_STRIP);
	glNormal3d(0, 0, 1);
	glVertex3dv(t1);
	glVertex3dv(t2);
	glVertex3dv(t3);
	glVertex3dv(t4);
	glVertex3dv(t5);
	glVertex3dv(t6);
	glEnd();

	double t7[] = { 6, 10, 5 };
	double t8[] = { 5, 6, 5 };
	double t9[] = { 11, 10, 5 };
	double t10[] = { 12, 2, 5 };

	glBegin(GL_TRIANGLE_STRIP);
	glNormal3d(0, 0, 1);
	glVertex3dv(t7);
	glVertex3dv(t8);
	glVertex3dv(t9);
	glVertex3dv(t10);
	glEnd();

	// боковая часть

	// P1
	/*double b1[] = { 0, 0, 0 };
	double b2[] = { 0, 0, 5 };
	double b3[] = { 5, 6, 0 };
	double b4[] = { 5, 6, 5 };
	double b5[] = { 12, 2, 0 };
	double b6[] = { 12, 2, 5 };
	double b7[] = { 11, 10, 0 };
	double b8[] = { 11, 10, 5 };
	double b9[] = { 6, 10, 0 };
	double b10[] = { 6, 10, 5 };
	double b11[] = { 5, 14, 0 };
	double b12[] = { 5, 14, 5 };
	double b13[] = { -1, 12, 0 };
	double b14[] = { -1, 12, 5 };
	double b15[] = { 3, 7, 0 };
	double b16[] = { 3, 7, 5 };
	double b17[] = { 0, 0, 0 };
	double b18[] = { 0, 0, 5 };*/

	double b1[] = { 0, 0, 0 };
	double b1_h[] = { 0, 0, 5 };
	double b2[] = { 5, 6, 0 };
	double b2_h[] = { 5, 6, 5 };
	double b3[] = { 12, 2, 0 };
	double b3_h[] = { 12, 2, 5 };
	double b4[] = { 11, 10, 0 };
	double b4_h[] = { 11, 10, 5 };
	double b5[] = { 6, 10, 0 };
	double b5_h[] = { 6, 10, 5 };
	double b6[] = { 5, 14, 0 };
	double b6_h[] = { 5, 14, 5 };
	double b7[] = { -1, 12, 0 };
	double b7_h[] = { -1, 12, 5 };
	double b8[] = { 3, 7, 0 };
	double b8_h[] = { 3, 7, 5 };
	double b9[] = { 0, 0, 0 };
	double b9_h[] = { 0, 0, 5 };

	glBegin(GL_QUADS);
	double* normal = normalVector(b2, b1, b2_h);
	glNormal3d(-normal[0], -normal[1], -normal[2]);
	glColor3d(0.25, 0.7, 0);
	glVertex3dv(b1);
	glVertex3dv(b1_h);
	glVertex3dv(b2_h);
	glVertex3dv(b2);
	glEnd();

	glBegin(GL_QUADS);
	double* normal2 = normalVector(b3, b2, b3_h);	
	glNormal3d(-normal2[0], -normal2[1], -normal2[2]);
	glColor3d(0.25, 0.7, 0);
	glVertex3dv(b2);
	glVertex3dv(b2_h);
	glVertex3dv(b3_h);
	glVertex3dv(b3);
	glEnd();

	glBegin(GL_QUADS);
	double* normal3 = normalVector(b3, b2, b3_h);
	glNormal3d(normal3[0], normal3[1], normal3[2]);
	glColor3d(0.25, 0.7, 0);
	glVertex3dv(b3);
	glVertex3dv(b3_h);
	glVertex3dv(b4_h);
	glVertex3dv(b4);
	glEnd();

	// 4
	glBegin(GL_QUADS);
	double* normal4 = normalVector(b4, b3, b4_h);
	//glNormal3d(-normal4[0], normal4[1], normal4[2]);
	//glNormal3d(-normal4[0], -normal4[1], normal4[2]);
	//glNormal3d(-normal4[0], -normal4[1], -normal4[2]);
	glNormal3d(-normal4[0], -normal4[1], normal4[2]);
	glColor3d(0.25, 0.7, 0);
	glVertex3dv(b4);
	glVertex3dv(b4_h);
	glVertex3dv(b5_h);
	glVertex3dv(b5);
	glEnd();

	glBegin(GL_QUADS);
	double* normal5 = normalVector(b5, b4, b5_h);
	glNormal3d(-normal5[0], -normal5[1], -normal5[2]);
	glColor3d(0.25, 0.7, 0);
	glVertex3dv(b5);
	glVertex3dv(b5_h);
	glVertex3dv(b6_h);
	glVertex3dv(b6);
	glEnd();


	// 6
	glBegin(GL_QUADS);
	double* normal6 = normalVector(b6, b5, b6_h);
	//glNormal3d(normal6[0], normal6[1], normal6[2]);
	//glNormal3d(normal6[0], -normal6[1], -normal6[2]);
	glNormal3d(normal6[0], -normal6[1], normal6[2]);
	glColor3d(0.25, 0.7, 0);
	glVertex3dv(b6);
	glVertex3dv(b6_h);
	glVertex3dv(b7_h);
	glVertex3dv(b7);
	glEnd();

	// 7
	glBegin(GL_QUADS);
	double* normal7 = normalVector(b7, b6, b7_h);
	glNormal3d(-normal7[0], -normal7[1], -normal7[2]);
	//glNormal3d(-normal7[0], normal7[1], -normal7[2]);
	glColor3d(0.25, 0.7, 0);
	glVertex3dv(b7);
	glVertex3dv(b7_h);
	glVertex3dv(b8_h);
	glVertex3dv(b8);
	glEnd();


	
	glBegin(GL_QUADS);
	double* normal8 = normalVector(b8, b7, b8_h);
	glNormal3d(-normal8[0], normal8[1], -normal8[2]);
	glColor3d(0.25, 0.7, 0);
	glVertex3dv(b8);
	glVertex3dv(b8_h);
	glVertex3dv(b9_h);
	glVertex3dv(b9);
	glEnd();



	/*glBegin(GL_QUAD_STRIP);
	glVertex3dv(b1);
	glVertex3dv(b2);
	glVertex3dv(b3);
	glVertex3dv(b4);
	glVertex3dv(b5);
	glVertex3dv(b6);
	glVertex3dv(b7);
	glVertex3dv(b8);
	glVertex3dv(b9);
	glVertex3dv(b10);
	glVertex3dv(b11);
	glVertex3dv(b12);
	glVertex3dv(b13);
	glVertex3dv(b14);
	glVertex3dv(b15);
	glVertex3dv(b16);
	glVertex3dv(b17);
	glVertex3dv(b18);
	glEnd();*/

	// figure end


	//Íà÷àëî ðèñîâàíèÿ êâàäðàòèêà ñòàíêèíà
	double A[2] = { -4, -4 };
	double B[2] = { 4, -4 };
	double C[2] = { 4, 4 };
	double D[2] = { -4, 4 };

	glBindTexture(GL_TEXTURE_2D, texId);

	glColor3d(0.6, 0.6, 0.6);
	/*glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);

	glEnd();*/
	//êîíåö ðèñîâàíèÿ êâàäðàòèêà ñòàíêèíà


   //Ñîîáùåíèå ââåðõó ýêðàíà

	
	glMatrixMode(GL_PROJECTION);	//Äåëàåì àêòèâíîé ìàòðèöó ïðîåêöèé. 
	                                //(âñåê ìàòðè÷íûå îïåðàöèè, áóäóò åå âèäîèçìåíÿòü.)
	glPushMatrix();   //ñîõðàíÿåì òåêóùóþ ìàòðèöó ïðîåöèðîâàíèÿ (êîòîðàÿ îïèñûâàåò ïåðñïåêòèâíóþ ïðîåêöèþ) â ñòåê 				    
	glLoadIdentity();	  //Çàãðóæàåì åäèíè÷íóþ ìàòðèöó
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //âðóáàåì ðåæèì îðòîãîíàëüíîé ïðîåêöèè

	glMatrixMode(GL_MODELVIEW);		//ïåðåêëþ÷àåìñÿ íà ìîäåë-âüþ ìàòðèöó
	glPushMatrix();			  //ñîõðàíÿåì òåêóùóþ ìàòðèöó â ñòåê (ïîëîæåíèå êàìåðû, ôàêòè÷åñêè)
	glLoadIdentity();		  //ñáðàñûâàåì åå â äåôîëò

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //êëàññèê ìîåãî àâòîðñòâà äëÿ óäîáíîé ðàáîòû ñ ðåíäåðîì òåêñòà.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - off / onn" << std::endl;
	ss << "L - light" << std::endl;
	ss << "F - light from camera" << std::endl;
	ss << "G - move light horizontal" << std::endl;
	ss << "G+lkm move light vertical" << std::endl;
	ss << "light coordinate: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "camera coordinate: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "camera params: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //âîññòàíàâëèâàåì ìàòðèöû ïðîåêöèè è ìîäåë-âüþ îáðàòüíî èç ñòåêà.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}