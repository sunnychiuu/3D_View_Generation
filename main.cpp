#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include <unistd.h>
#include <GL/glew.h>

#include "controls.hpp"
#include "TRIModel.h"
#include "Shader.h"
#include "Transform.h"
#include "Light.h"
#include "utils.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLFW/glfw3.h>
#else
#include <GL/glu.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#endif


enum{PHONG}; // enumerate the shading styles
#define SHADER_NUM 1 // determine number of shaders

using namespace glm;
using namespace std;
typedef unsigned char UCHAR;

vector<TRIModel> models; // triangle model list

bool offscreen = 1;
int W = 600, H  = 600;

void initOffscreen();
vector<vec3> read_eyes_pos(string eyes_file);
void saveImage(string path);

void loadModel(const char* filename){
	//TRIModel model;
	OFFModel model;
	model.loadFromFile(filename);
	models.push_back(model);
}

int main(int argc, char **argv){
    string offFile(argv[1]);  //input filename
    string outDir(argv[2]);   //output directory name
    string offname = removeExtension(basename(offFile));

    GLFWwindow* window;
    if (!glfwInit())
        exit(EXIT_FAILURE);
    window = glfwCreateWindow(600/2, 600/2, "Demo", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
	if(glewInit() != GLEW_OK)
		cout << "Failed to initial GLEW"<<endl;

    if (offscreen)
        initOffscreen();

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);


	// Read the models
	//loadModel("models/balls.tri");
	//loadModel("m100.off");
	loadModel(offFile.c_str());

	// Create and compile our GLSL program from the shaders
	Shader shader[SHADER_NUM];
	shader[PHONG].CreateProgram("vs.vertexshader", "fs.fragmentshader");
	//shader[PHONG].CreateProgram("gauraud_test.vertexshader", "gauraud_test.fragmentshader");
	// get associated handle for uniform variables and attribute variables in the shader program
	string univars[13]={"PVM", "V", "M", "normat", "ligposwld", "ligcolor",
            "uMaterialShininess",
            "uAmbientColor",
            "uPointLightingLocation",
            "uPointLightingSpecularColor",
            "uPointLightingDiffuseColor",
            "uForeColor",
            "uBackColor" };
	string attrvars[3]={"vertposmdl", "vertnormdl", "vertcolor"};
	for(int s = 0; s < SHADER_NUM; ++s){
        for(int i = 0; i < 13; ++i) {
			shader[s].GetUniformLocation(univars[i]);
        }
		for(int i = 0; i < 3; ++i)
			shader[s].GetAttributeLocation(attrvars[i]);
	}


	// Create vertex buffer objects and bind them to attribute handle
	VBO vertbuf(3, GL_FLOAT), colorbuf(3, GL_FLOAT), normalbuf(3, GL_FLOAT), backcolorbuf(3, GL_FLOAT);
	int numvert = 0;
	for(size_t i = 0; i < models.size(); ++i)
		numvert += models[i].vertices.size();
    cout << numvert << endl;
	GLsizeiptr reqmem = numvert * sizeof(vec3);
	vertbuf.Alloc(reqmem);
	colorbuf.Alloc(reqmem);
    backcolorbuf.Alloc(reqmem);
	normalbuf.Alloc(reqmem);
	for(size_t i = 0; i < models.size(); ++i){
		GLsizeiptr memsize = models[i].vertices.size() * sizeof(vec3);
		vertbuf.Append(memsize, &(models[i].vertices[0]));
		colorbuf.Append(memsize, &(models[i].forecolors[0]));
		normalbuf.Append(memsize, &(models[i].normals[0]));
        backcolorbuf.Append(memsize, &(models[i].backcolors[0]));
	}
	for(int s = 0; s < SHADER_NUM; ++s){
		shader[s].BindVBO(&vertbuf, "vertposmdl");
		shader[s].BindVBO(&colorbuf,"vertcolor");
		shader[s].BindVBO(&normalbuf, "vertnormdl");
		shader[s].BindVBO(&backcolorbuf, "uBackColor");
	}

	// Create Lights, and bind them to uniform vector handle
	Light lig(vec3(4, -4, 4), vec3(1.0, 1.0, 1.0));
	for(int s = 0; s < SHADER_NUM; ++s){
		shader[s].BindVector(&(lig.pos), "ligposwld");
		shader[s].BindVector(&(lig.color), "ligcolor");
	}

    // But I don't use the lighting above, instead create a new one
    float materialShininess = 3;
    vec3 ambientColor(0.8, 0.8, 0.8);
    //vec3 pointLightingLocation(30.0, 30.0, 40.0);
    vec3 pointLightingLocation(30.0, 30.0, 40.0);
    vec3 pointLightingSpecularColor(0.7, 0.7, 0.7);
    vec3 pointLightingDiffuseColor(2.8, 2.8, 2.8);
	for(int s = 0; s < SHADER_NUM; ++s){
		shader[s].BindFloat(materialShininess, "uMaterialShininess");
		shader[s].BindVector(&(ambientColor), "uAmbientColor");
		shader[s].BindVector(&(pointLightingLocation), "uPointLightingLocation");
		shader[s].BindVector(&(pointLightingSpecularColor), "uPointLightingSpecularColor");
		shader[s].BindVector(&(pointLightingDiffuseColor), "uPointLightingDiffuseColor");
	}

	// Create Transform, and bind them to uniform matrix handle

	Transform transform;
	for(int s = 0; s < SHADER_NUM; ++s){
		shader[s].BindMatrix(&(transform.pvm), "PVM");
		shader[s].BindMatrix(&(transform.modelmat), "M");
		shader[s].BindMatrix(&(transform.viewmat), "V");
		shader[s].BindMatrix(&(transform.normat), "normat");
	}


    vector<vec3> eyes = read_eyes_pos("./eyes.conf");

    // drawScene:
    for (int i = 0; i < eyes.size(); i++) {
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        vec3 eye = eyes[i];
        eye = normalize(eye) * 1.5f;
        pointLightingLocation = normalize(eye) *300.0f;

		transform.SetProjectionMatrix(0.0);
		transform.SetViewMatrix(eye, vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 0.0));
		transform.SetModelMatrix(1.0, models[0].center, vec3(0.0, 0.0, 0.0) , vec3(0.0, 0.0, 0.0));
		transform.UpdatePVM();
		transform.UpdateNormalMatrix();

		shader[PHONG].Draw(GL_TRIANGLES, vertbuf.offset[0] / sizeof(vec3), models[0].vertices.size());


        // must save before swap buffer
        string outImg = outDir + "/" + offname + "." + to_string(i) + ".png";
        saveImage(outImg);

        glfwSwapBuffers(window);

        //sleep(1);
    }





#if 0
	//the viewing matrix (camera setting) is currently fixed
	//you can also change it in the render loop
	//transform.SetViewMatrix(vec3(0, 0, 6.0), vec3(0.0, 0.0, -40.0), vec3(0.0, 1.0, 0.0));
	// start to render

	vec3 rot(0.0, 0.0, 0.0);

	CtrlParam ctrlparyyam;

    while (!glfwWindowShouldClose(window))
    {
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// the projection may changing
#if 1
		// update transformation according to keyboard/mouse control, see control.hpp/cpp
		CatchEvent(window);
		UpdateCtrlParams(ctrlparam);
		transform.SetProjectionMatrix(ctrlparam.dfov);
		transform.SetViewMatrix(vec3(0, 0, 6.0), vec3(0.0, 0.0, -40.0), vec3(0.0, 1.0, 0.0), ctrlparam.rotview);
		transform.SetModelMatrix(ctrlparam.size, models[0].center, ctrlparam.trans, ctrlparam.rot);
#endif
#if 0
        transform.SetProjectionMatrix(0);
		transform.SetModelMatrix(0.002739, models[0].center, vec3(0.8, -0.7, 2.79), vec3(0, rot.y, 0) );
#endif
		transform.UpdatePVM();
		transform.UpdateNormalMatrix();


		shader[PHONG].Draw(GL_TRIANGLES, vertbuf.offset[0] / sizeof(vec3), models[0].vertices.size());

        glfwSwapBuffers(window);
        glfwPollEvents();

        rot.y += 0.3;
    }

#endif

    glfwDestroyWindow(window);
    glfwTerminate();

	return 0;
}



void initGL() {

}

void initOffscreen() {
    // render on memory
    GLuint fbo, rboColor, rboDepth;

    // Color renderbuffer.
    glGenRenderbuffers(1,&rboColor);
    glBindRenderbuffer(GL_RENDERBUFFER, rboColor);
    // Set storage for currently bound renderbuffer.
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, W, H);


    // DEPTH
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(
        GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, W, H
    );


    // Framebuffer
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER,fbo);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rboColor);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch(status) {
        case GL_FRAMEBUFFER_COMPLETE:
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            cout << "Unsupported framebuffer." << endl;
            exit(0);
    }


    // Set to write to the framebuffer.
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,fbo);

    // Tell glReadPixels where to read from.
    //glReadBuffer(GL_COLOR_ATTACHMENT0);


}

vector<vec3> read_eyes_pos(string eyes_file) {
    FILE *f = fopen(eyes_file.c_str(), "r");
    if (!f) {
        cerr << "eyesfile " << eyes_file << " not found!" << endl;
        exit(-1);
    }

    char line[100];
    vector<vec3> eyes;
    vec3 eye;

    DEBUG_MSG("eyes:");
    while(std::fscanf(f,"%f,%f,%f", &eye.x, &eye.y, &eye.z) != EOF) {
        eyes.push_back(eye);
        DEBUG_MSG("(" << eye.x << "," << eye.y << "," << eye.z << ") ");
    }
    fclose(f);

    return eyes;
}

void saveImage(string path) {
    int arrLen = W * H * 3;
    GLbyte* colorArr = new GLbyte[ arrLen ];

    if (offscreen) glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0, 0, W, H, GL_RGB, GL_UNSIGNED_BYTE, colorArr);

    cv::Mat img, grey;
    std::vector<cv::Mat> imgPlanes(3);
    img.create(H, W, CV_8UC3);
    cv::split(img, imgPlanes);

    int count = 0;
    for(int i = 0; i < H; i ++) {
        UCHAR* plane0Ptr = imgPlanes[0].ptr<UCHAR>(i);
        UCHAR* plane1Ptr = imgPlanes[1].ptr<UCHAR>(i);
        UCHAR* plane2Ptr = imgPlanes[2].ptr<UCHAR>(i);
        for(int j = 0; j < W; j ++) {
            int k = 3 * (i * W + j);
            // BGR to RGB
            plane2Ptr[j] = colorArr[k];
            plane1Ptr[j] = colorArr[k+1];
            plane0Ptr[j] = colorArr[k+2];
            count ++;
        }
    }

    cv::merge(imgPlanes, img);
    cv::flip(img, img ,0); // !!!
    cv::cvtColor(img, grey, CV_BGR2GRAY);

    printf("path: %s\n", path.c_str());
    imwrite(path.c_str(), grey);

    img.release();
    grey.release();
    delete [] colorArr;
}
