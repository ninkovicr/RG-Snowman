#include<GL/glut.h>
#include<math.h>
#include<stdio.h>
#include<stdbool.h>
#include<time.h>

#define TIMER_ID 1
#define TIMER_INTERVAL 45
#define SNOWFLAKE_MAX 10000
#define SNOWFLAKE_ID 2
#define TIMER_SNOWFLAKE 1000

/* Callback f-je. */
static void on_display(void);
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_timer(int value);
static void snowflakeTimer(int value);

void drawSnowMan(); /* F-ja za crtanje sneska. */
void drawSnowflake(); /* F-ja za crtanje pahuljica. */
void drawText(char *string, float x, float y, float z); /* F-ja za ispisivanje teksta. */
void end(); /*F-ja za kraj igrice. */
void drawLine();
/* F-ja za proveru da li Snesko i kugla dodiruju. 
 *Argumenti f-je: x koordinata kugle, z koordinata kugle, poluprecnik kugle, x koordinata Sneska, z koordinata Sneska, poluprecnik tela Sneska. */
bool collision(float bx, float bz, float rBoll, float xSnow, float zSnow, float rSnow);


int points = 0; /* Promenljiva za broj poena. */
int lives = 3; /* Promenljiva za 3 zivota, koja se smanjuje kada Snesko pokupi vatrenu kuglu. */
static float vx, vz; /* Koordinate za kretanje. */
float bx, bz; /* Koordinate bele kugle (grudve). */
float kx, kz; /* Koordinate vatrene kugle. */ 
/* Promenljiva za kreiranje grudvi i vatrenih kugli. */
bool create_boll = true; 
bool create_boll_fire = true;
int snowflakeId = 0;

static int animation_ongoing;
static int animation_snowflake;

/* Struktuka Pahulje - poluprecnik i koordinate. */
typedef struct{
    float x;
    float y;
    float z;
    bool alive;
}SNOWFLAKE;
    

SNOWFLAKE snowFlakes[SNOWFLAKE_MAX];

/*Struktuka za Sneska - koordinate Sneska. */
typedef struct{
    float xKor;
    float zKor;
}SNOWMAN;

SNOWMAN* snowMan;

/* Koeficijenti za osvetljenje. */
GLfloat ambient_coeffs[] = {0,0,0,1};
GLfloat diffuse_coeffs[] = {0.9,0.9,0.9,1};
GLfloat specular_coeffs[] = {1,1,1,1};

GLfloat diffuse_coeffs_polygon[] = {0,1,1,1};
GLfloat diffuse_coeffs_mesh[] = {1,1,1,1};
GLfloat diffuse_coeffs_boll[] = {0.7,0.7,1,1};
GLfloat diffuse_coeffs_fire[] = {1,0.5,0,1};


int main(int argc, char** argv){
    
    /* Inicijalizuje se GLUT */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    /* Pravi se prozor */
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(200, 200);
    glutCreateWindow(argv[0]);
    
    /* Callback funkcije */
    glutKeyboardFunc(on_keyboard);
    glutReshapeFunc(on_reshape);
    glutDisplayFunc(on_display);

    /* OpenGL inicijalizacija */
    glClearColor(0.8, 0.8, 0.8, 0);
    glEnable(GL_DEPTH_TEST);
    glLineWidth(3);
    glEnable(GL_NORMALIZE);
    
    /* Inicijalne vrednosti */
    snowMan = malloc(sizeof(SNOWMAN));
    snowMan->xKor = -0.1;
    snowMan->zKor = -0.15;     
    
    vx = 0;
    vz = 0.0085;
    
    int seed = time(NULL);
    srand(seed);
    
    /* Random pozicije za pahulje. */
    for(int i=0; i<SNOWFLAKE_MAX; i++){
        //snowFlakes[i].r = 0.02;
        snowFlakes[i].x = 2.0*(float)rand()/RAND_MAX - 1.0;
        snowFlakes[i].y = 1.5;
        snowFlakes[i].z = 2.0*(float)rand()/RAND_MAX - 1.0;
        snowFlakes[i].alive = false;
        //printf("%f %f %f\n", snowFlakes[i].x, snowFlakes[i].y, snowFlakes[i].z);
    }
    
    animation_ongoing = 0;
    /* animation_snowflake pocinje od 1 da bi se pravilo od 1. pahulje. */
    animation_snowflake = 1;
    glutTimerFunc(TIMER_SNOWFLAKE, snowflakeTimer, SNOWFLAKE_ID);
    
    glutMainLoop();
    
    return 0;
}

static void on_reshape(int width, int height)
{
    /* Podesava se viewport. */
    glViewport(0, 0, width, height);

    /* Podesava se projekcija. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float) width / height, 1, 50);
}

static void on_keyboard(unsigned char key, int x, int y){
    switch(key){
        /* Izlaz */
        case 27:
            exit(0);
            break;
            
        /* Pauza */
        case 'p':
        case 'P':
            animation_ongoing = 0;
            break;
            
        /* Nastavlja se */
        case 'g':
        case 'G':
            if(!animation_ongoing){
                snowMan->xKor = -0.1;
                snowMan->zKor = -0.15;     
                
                vx = 0;
                vz = 0.0085;
                
                points = 0;
                lives = 3;
                
                animation_ongoing = 1;
                on_timer(TIMER_ID);
            }
            break;
        /* Komande za kretanje levo, desno, napred i nazad */
        case 'a':
                vx = -0.0085;
                vz = 0;
            break;
        case 'd':
                vx = 0.0085;
                vz = 0;
            break;
        case 's':
                vx = 0;
                vz = 0.0085;
            break;
        case 'w':
                vx = 0;
                vz = -0.0085;
            break;
    }
}

static void on_timer(int value){
    if(value != TIMER_ID)
        return;

    if(animation_ongoing == 0)
        return;
    
    snowMan->xKor += vx;
    snowMan->zKor += vz;
    
    for(int i=0; i<SNOWFLAKE_MAX; i++)
        if(snowFlakes[i].alive)
            snowFlakes[i].y -= 0.02;
    
    glutPostRedisplay();

    if(animation_ongoing)
        glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
}

static void snowflakeTimer(int value){
    if(SNOWFLAKE_ID != value)
        return;
    
    snowflakeId ++;
    
    snowFlakes[snowflakeId].alive = true;
    
    if(animation_snowflake)
        glutTimerFunc(TIMER_SNOWFLAKE+100, snowflakeTimer, SNOWFLAKE_ID);
}

static void on_display(void)
{
    /* Pozicija svetla (direktno svetlo) */
    GLfloat light_position[] = {1,1,1,0};
    /* Ambijentalna boja svetlosti */
    GLfloat light_ambient[] = {0.1,0.1,0.1,1};
    /* Difuzna boja svetlosti */
    GLfloat light_diffuse[] = {0.7,0.7,0.7,1};
    /* Spekularna boja svetlosti */
    GLfloat light_specular[] = {0.9,0.9,0.9,1};
    
    /* Koeficijent glastkosti materijala */
    GLfloat shininess = 20;
    
    /* Brise se prethodni sadrzaj prozora. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Podesava se tacka pogleda. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
            -0.55, 1.8, 1.5,
            0, 0, 0,
            0, 1, 0);

    glRotatef(-20,0,1,0);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
  
    /* Crtanje pahuljica. */
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs_boll);
    for(int i=1; i<=snowflakeId; i++){
        glPushMatrix();
        glTranslatef(snowFlakes[i].x, snowFlakes[i].y, snowFlakes[i].z);
        glScalef(0.1, 0.1, 0.1);
        drawSnowflake();
        glPopMatrix();
    }
    /* Crtanje sneska */
    glEnable(GL_COLOR_MATERIAL);
    glPushMatrix();
        glTranslatef(snowMan->xKor,0,snowMan->zKor);       
        
        if(points >= 0 && points < 5)
            glScalef(0.3, 0.3, 0.3);
        else if(points >= 5 && points < 10)
            glScalef(0.35, 0.35, 0.35);
        else if (points >= 10 && points < 15)
            glScalef(0.4, 0.4, 0.4);
        else 
            glScalef(0.45, 0.45, 0.45);
        
        drawSnowMan();
    glPopMatrix();
    glDisable(GL_COLOR_MATERIAL);
    
        
    /* Iscrtavanje mreze po kojoj se snesko krece. */    
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs_polygon);
    glBegin(GL_POLYGON);
        glVertex3f(1, 0, 1);
        glVertex3f(1, 0, -1);
        glVertex3f(-1, 0, -1);
        glVertex3f(-1, 0, 1);
    glEnd();
    
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs_mesh);
    glBegin(GL_LINES);
        for(int i=-5;i<6;i++){
            glVertex3f(i/5.0, 0, 1);
            glVertex3f(i/5.0, 0, -1);
            glVertex3f(-1, 0, i/5.0);
            glVertex3f(1, 0, i/5.0);
        }
    glEnd();
    
    
    /* Kada snesko ispadne iz mreze po kojoj se krece kraj igre. */
    if(snowMan->xKor > 1 || snowMan->xKor < -1)
        end();
    else if(snowMan->zKor > 1 || snowMan->zKor < -1)
        end();
    
    
     /* Nove grudve */
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs_boll);
    
    if(create_boll){
        create_boll = false;
        int r = rand() % 18;
        bx = (r - 9)/10.0*1.05 + 0.05;
        r = rand() % 18;
        bz = (r - 9)/10.0*1.05 + 0.05;
     }
    
    glPushMatrix();
        glTranslatef(bx, 0.1, bz);
        glutSolidSphere(0.05,20,20);
    glPopMatrix();
    
    /* Nove vatrene kugle */
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs_fire);
    
    if(create_boll_fire){
        create_boll_fire = false;
        int r = rand() % 18;
        kx = (r - 9)/10.0*1.05 + 0.05;
        r = rand() % 18;
        kz = (r - 9)/10.0*1.05 + 0.05;
        
        if(kx == bx && kz == bz){
            int r = rand() % 18;
            kx = (r - 9)/10.0*1.05 + 0.05;
            r = rand() % 18;
            kz = (r - 9)/10.0*1.05 + 0.05;
            }
        }
    
    glPushMatrix();
        glTranslatef(kx, 0.1, kz);
        glutSolidSphere(0.05,20,20);
    glPopMatrix();

        /* Kada snesko pokupi grudvu */
    if(collision(bx, bz, 0.05, snowMan->xKor, snowMan->zKor, 0.15)){
        points ++;
        create_boll = true;
    } 
    
    /* Kada snesko pokupi vrelu kuglu */
    if(collision(kx, kz, 0.05, snowMan->xKor, snowMan->zKor, 0.15)){
        points --;
        lives --;
        create_boll_fire = true;
    }
    
    if(lives == 0 || points < 0)
        end();
    
    /* Ispisivanje poena */
    char string[20];
    sprintf(string, "POINTS %d", points);
    drawText(string,0.5,1,0);
    
    /* Ispisivanje zivota. */
    sprintf(string, "LIVES 3 : %d", lives);
    drawText(string, -0.7,1,0);
    
    /* Nova slika se salje na ekran. */
    glutSwapBuffers();
}

void drawLine(){
    glColor3f(0,0,1);
    glBegin(GL_LINES);
        glVertex3f(0, -0.5, 0);
        glVertex3f(0, 0.5, 0);
    glEnd();
}

void drawSnowMan(){
    glColor3f(1.0, 1.0, 1.0);

    glPushMatrix();
    /* Crta telo */
    glTranslatef(0.0 ,0.75, 0.0);
    glutSolidSphere(0.5,20,20);

    /* Crta glavu */
    glTranslatef(0.0, 0.75, 0.0);
    glutSolidSphere(0.25,20,20);

    /* Crta oci */
    glPushMatrix();
    glColor3f(0.0,0.0,0.0);
    glTranslatef(0.05, 0.10, 0.18);
    glutSolidSphere(0.05,10,10);
    glTranslatef(-0.1, 0.0, 0.0);
    glutSolidSphere(0.05,10,10);
    glPopMatrix();

    /* Crta nos */
    glColor3f(1.0, 0.5 , 0.5);
    glutSolidCone(0.08,0.5,10,2);    

    glPopMatrix();
}

void drawSnowflake(){
    /* Crtanje vertikalne linije sa dve sfere na krajevima. */
    /*  |  */
    glPushMatrix();
    drawLine();
    
    glTranslatef(0,0.45,0);
    glutWireSphere(0.05, 5, 5);
    glTranslatef(0,-0.45,0);
    
    glTranslatef(0,-0.45,0);
    glutWireSphere(0.05, 5, 5);
    glTranslatef(0,+0.45,0);
    
    glPopMatrix();
    
    
    /* Crtanje horizontalne linije sa dve sfere na krajevima. */
    /*  --  */
    glPushMatrix();
    
    glRotatef(-45,0,0,1);
    drawLine();
    
    glTranslatef(0,0.45,0);
    glutWireSphere(0.05, 5, 5);
    glTranslatef(0,-0.45,0);

    glTranslatef(0,-0.45,0);
    glutWireSphere(0.05, 5, 5);
    glTranslatef(0,+0.45,0);
    
    glPopMatrix();
    
    
    /* Crtanje kose linije sa dve sfere na krajevima. */
    /*  /  */
    glPushMatrix();
    
    glRotatef(45,0,0,1);
    drawLine();
    
    glTranslatef(0,0.45,0);
    glutWireSphere(0.05, 5, 5);
    glTranslatef(0,-0.45,0);
    
    glTranslatef(0,-0.45,0);
    glutWireSphere(0.05, 5, 5);
    glTranslatef(0,0.45,0);
    
    glPopMatrix();
    
    
    /* Crtanje kose linije sa dve sfere na krajevima. */
    /*  \  */
    glPushMatrix();
    
    glRotatef(90, 0,0,1);
    drawLine();
    
    glTranslatef(0,-0.45,0);
    glutWireSphere(0.05, 5, 5);
    glTranslatef(0,0.45,0);
    
    glTranslatef(0,0.45,0);
    glutWireSphere(0.05, 5, 5);
    glTranslatef(0,-0.45,0);
    
    glPopMatrix();
}

void drawText(char *string,float x,float y,float z) 
{  
    char *s;
    glRasterPos3f(x, y,z);
    for (s=string; *s != '\0'; s++) 
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *s);
}

void end(){
    animation_ongoing = 0;
    char string[20];
    sprintf(string, "END");
    drawText(string,0,1,0);
}

bool collision(float bx, float bz, float rBoll, float xSnow, float zSnow, float rSnow){
    float d = sqrt((bx-xSnow)*(bx-xSnow) + 0.1*0.1 + (bz-zSnow)*(bz-zSnow));
    
    /* Proverava rastojanje izmedju Sneska i kugle. */
    if(d < (rBoll + rSnow))
        return true;

    return false;
}
