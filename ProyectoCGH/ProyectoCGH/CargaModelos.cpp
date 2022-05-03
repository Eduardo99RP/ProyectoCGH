/*
Semestre 2022-2
Proyecto
*/
//
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include <irrklang\irrKlang.h>

//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

//materiales
Material Material_brillante;
Material Material_opaco;


Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;


//texturas de modelos
Texture Lago;
Texture TCasa_1;
Texture TArbol_1;
Texture isla_float;
Texture caminoo;
Texture lampara;
Texture TCasa_2;
Texture TMountain;
Texture TNube1;

//modelos
Model Casa_1;
Model Casa_2;
Model lago;
Model Arbol_1;
Model Islad_1;
Model Montana_1;
Model Mountain;

Model Paloma_1;
Model isla_flotante;
Model camino;
Model lamp;
Model Nube1;
Model Cheems;

//sr zanahoria
Model Zanahoria_Cuerpo;
Model Zanahoria_Brazo_Izq;
Model Zanahoria_Brazo_Der;
Model Zanahoria_Pierna;
Model Zanahoria_Hoja_1;
Model Zanahoria_Hoja_2;


Skybox skybox;
Skybox skybox_dia;
Skybox skybox_noche;

//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;


// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";
using namespace irrklang;

void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3,

	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	1000.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 1000.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		1000.0f, 1000.0f,	0.0f, -1.0f, 0.0f
	};

	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);
	
	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}




int main()
{

	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 1.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	isla_float = Texture("Textures/isla_flotante.png");
	isla_float.LoadTextureA();

	TCasa_1 = Texture("Textures/Casa_1.png");
	TCasa_1.LoadTextureA();
	TCasa_2 = Texture("Textures/VoxelHouse_02_tex");
	TCasa_2.LoadTextureA();

	caminoo = Texture("Textures/camino.png");
	caminoo.LoadTextureA();

	lampara = Texture("Textures/lamp.png");
	lampara.LoadTextureA();

	TMountain = Texture("Textures/mountain.png");
	TMountain.LoadTextureA();

	TNube1 = Texture("Textures/nube1.png");
	TNube1.LoadTextureA();

	Casa_1 = Model();
	Casa_1.LoadModel("Models/Casa_1.fbx");
	Casa_2 = Model();
	Casa_2.LoadModel("Models/casa_2.obj");

	camino = Model();
	camino.LoadModel("Models/camino.obj");

	lago = Model();
	lago.LoadModel("Models/Island v.2.obj");

	Arbol_1 = Model();
	Arbol_1.LoadModel("Models/Arboles/Arbol_1.obj");

	Islad_1 = Model();
	Islad_1.LoadModel("Models/Terrenos/Islad_2.obj");

	Paloma_1 = Model();
	Paloma_1.LoadModel("Models/Animales/Paloma_1.3ds");

	Montana_1 = Model();
	Montana_1.LoadModel("Models/Terrenos/Montana_1.obj");

	Mountain = Model();
	Mountain.LoadModel("Models/mountain.obj");

	isla_flotante = Model();
	isla_flotante.LoadModel("Models/Terrenos/isla_flotante.obj");

	Cheems = Model();
	Cheems.LoadModel("Models/Personajes/Cheems_blender.obj");

	lamp = Model();
	lamp.LoadModel("Models/lamp.obj");

	Nube1 = Model();
	Nube1.LoadModel("Models/nube1.obj");



	//Sr zanahoria
	Zanahoria_Cuerpo = Model();
	Zanahoria_Cuerpo.LoadModel("Models/Sr_Zanahoria/zanahoria_cuerpo.obj");

	Zanahoria_Brazo_Izq = Model();
	Zanahoria_Brazo_Izq.LoadModel("Models/Sr_Zanahoria/zanahoria_brazo_izq.obj");

	Zanahoria_Brazo_Der = Model();
	Zanahoria_Brazo_Der.LoadModel("Models/Sr_Zanahoria/zanahoria_brazo_der.obj");

	Zanahoria_Pierna = Model();
	Zanahoria_Pierna.LoadModel("Models/Sr_Zanahoria/zanahoria_pierna.obj");

	Zanahoria_Hoja_1 = Model();
	Zanahoria_Hoja_1.LoadModel("Models/Sr_Zanahoria/zanahoria_Hoja_1 .obj");

	Zanahoria_Hoja_2 = Model();
	Zanahoria_Hoja_2.LoadModel("Models/Sr_Zanahoria/zanahoria_Hoja_2 .obj");


	/*************/

	//luz 
	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);
	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.8f, 0.8f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		2.0f, 4.0f,
		2.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;



	unsigned int spotLightCount = 0;

	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		10.0f);
	spotLightCount++;

	spotLights[1] = SpotLight(0.0f, 0.66f, 0.89f,
		2.0f, 4.0f,
		-400.0f, 70.0f, 370.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;

	spotLights[2] = SpotLight(0.0f, 0.66f, 0.89f,
		2.0f, 4.0f,
		-400.0f, 70.0f, 480.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;

	spotLights[3] = SpotLight(0.0f, 0.66f, 0.89f,
		2.0f, 4.0f,
		50.0f, 70.0f, 370.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;

	spotLights[4] = SpotLight(0.0f, 0.66f, 0.89f,
		2.0f, 4.0f,
		50.0f, 70.0f, 480.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/right_day.tga");
	skyboxFaces.push_back("Textures/Skybox/left_day.tga");

	skyboxFaces.push_back("Textures/Skybox/down_day.tga");
	skyboxFaces.push_back("Textures/Skybox/up_day.tga");

	skyboxFaces.push_back("Textures/Skybox/front_day.tga");
	skyboxFaces.push_back("Textures/Skybox/back_day.tga");



	skybox_dia = Skybox(skyboxFaces);


	skyboxFaces.clear();
	skyboxFaces.push_back("Textures/Skybox/right.tga");
	skyboxFaces.push_back("Textures/Skybox/left.tga");

	skyboxFaces.push_back("Textures/Skybox/down.tga");
	skyboxFaces.push_back("Textures/Skybox/up.tga");

	skyboxFaces.push_back("Textures/Skybox/front.tga");
	skyboxFaces.push_back("Textures/Skybox/back.tga");



	skybox_noche = Skybox(skyboxFaces);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	skybox = skybox_dia;
	int cont_tiempo_dia_noche = 1;
	bool dia = false;


	//***************************************************************//
	// inicie el motor de sonido con los parámetros predeterminados
	ISoundEngine* audio = createIrrKlangDevice();

	if (!audio)
		return 0; // por si hay algun error en el audio

	//audio->play2D("breakout.mp3", true); //Reproducce el audio en ciclo

	audio->play2D("Minecraftc.mp3", true); //Reproducce el audio en ciclo



	//***************************************************************//

	float Zanahoria_Pierna_mov = 0.0f;
	float Zanahoria_Brazo_Izq_mov = 0.0f;
	bool estado = true;
	bool estado_1 = false;

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		if (estado == true  ) {
			
			if (Zanahoria_Pierna_mov > 30) {
				estado_1 = true;
				estado = false;
			}
			else {
				//Zanahoria_Pierna_mov = mainWindow.getmuevez();
				Zanahoria_Brazo_Izq_mov += 0.8 * deltaTime;
			
			}
		
		}
		if (estado_1 == true  ) {

			if (Zanahoria_Pierna_mov < -30) {
				estado_1 = false;
				estado = true;
			}
			else {
				//Zanahoria_Pierna_mov = mainWindow.getmuevez();
				Zanahoria_Brazo_Izq_mov -= 0.8 * deltaTime;


			}

		}

		Zanahoria_Pierna_mov = mainWindow.getmuevez()*2.0f;


		if (cont_tiempo_dia_noche % 12000 == 0) {
			if (dia) {

				mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
					0.8f, 0.8f,
					0.0f, 0.0f, -1.0f);
				skybox = skybox_dia;



			}
			else {
				mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
					0.2f, 0.2f,
					0.0f, 0.0f, -1.0f);
				skybox = skybox_noche;


			}
			dia = !dia; //para hace cambio si es 1 se volvera 0, si es 0 se volvera 1
		}

		cont_tiempo_dia_noche += 1;


		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);


		// luz ligada a la cámara de tipo flash
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(0.3608f, 0.5882f, 0.1804f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1000.0f, 4.0f, 1000.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		pisoTexture.UseTexture();
		meshList[2]->RenderMesh();




		//Agrgeado de modelos 


		//Casa_1
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-100.0f, -1.0f, 600.0f));
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

		TCasa_1.UseTexture();
		Casa_1.RenderModel();

		//Casa_2
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-200.0f, -23.0f, 250.0f));
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

		TCasa_2.UseTexture();
		Casa_2.RenderModel();

		//Camino
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-300.0f, 3.0f, 300.0f));
		model = glm::scale(model, glm::vec3(15.0f, 20.0f, 20.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

		caminoo.UseTexture();
		camino.RenderModel();

		//lampara1
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-400.0f, -4.0f, 350.0f));
		model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		lampara.UseTexture();
		lamp.RenderModel();

		//lampara2
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-400.0f, -4.0f, 500.0f));
		model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
		model = glm::rotate(model, -180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		lampara.UseTexture();
		lamp.RenderModel();

		//lampara3
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(50.0f, -4.0f, 350.0f));
		model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		lampara.UseTexture();
		lamp.RenderModel();

		//lampara4
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(50.0f, -3.0f, 500.0f));
		model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
		model = glm::rotate(model, -180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		lampara.UseTexture();
		lamp.RenderModel();

		//Lago_2
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(600.0f, 50.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Lago.UseTexture();
		lago.RenderModel();

		//Arbol_1 - 1

		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, -40.0f));
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		TArbol_1.UseTexture();
		Arbol_1.RenderModel();

		//islad_1
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 60.0f, -1000.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Islad_1.RenderModel();

		//Isla flotante
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 60.0f, -1000.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		isla_float.UseTexture();
		isla_flotante.RenderModel();

		//Paloma_1-falla la textura
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, -80.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Paloma_1.RenderModel();

		//Montaña 1
		model = glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::translate(model, glm::vec3(-1200.0f, 200.0f, -80.0f));
		model = glm::scale(model, glm::vec3(40.0f, 40.0f, 40.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//Montana_1.RenderModel();

		//Montaña Modelo 2
		model = glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::translate(model, glm::vec3(-800.0f, -120.0f, -1500.0f));
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		TMountain.UseTexture();
		Mountain.RenderModel();


		//Nube1
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(50.0f, 350.0f, -1600.0f));
		model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		TNube1.UseTexture();
		Nube1.RenderModel();

		//

		//cheems
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 10.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Cheems.RenderModel();


		//sr zanahoria 

		/*************************************************************************/
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevez()*0.5, 15.0f, 37.0f + mainWindow.getmuevex()*0.5));

		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Zanahoria_Cuerpo.RenderModel();

		glm::mat4 SrZanahoria(1.0);
		SrZanahoria = model;
		model = modelaux;

		model = modelaux = SrZanahoria;

		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		modelaux = model;
		model = glm::translate(model, glm::vec3(2.5f, -2.0f, 0.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Zanahoria_Brazo_Izq.RenderModel();

		model = modelaux = SrZanahoria;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(-2.5f, 0.0f, 0.0f));
		modelaux = model;

		model = glm::rotate(model, Zanahoria_Brazo_Izq_mov* toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Zanahoria_Brazo_Der.RenderModel();

		model = modelaux = SrZanahoria;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(1.00f, -4.00f, -0.8f));
		modelaux = model;

		model = glm::rotate(model, Zanahoria_Pierna_mov * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Zanahoria_Pierna.RenderModel();

		model = modelaux = SrZanahoria;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(-1.0f, -4.00f, -0.8f));
		modelaux = model;

		model = glm::rotate(model, -Zanahoria_Pierna_mov * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Zanahoria_Pierna.RenderModel();
		/*************************************************************************/


		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
