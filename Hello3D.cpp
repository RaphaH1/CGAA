/* Hello Cube - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 * Aluno: Raphael Ferracioli
 *
 * Adaptado por Rossana Baptista Queiroz
 * para as disciplinas de Processamento Gráfico/Computação Gráfica - Unisinos
 * Modificado para incluir cubo, translação, escala e múltiplas instâncias
 */

#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Protótipos das funções
int setupShader();
int setupGeometry();

// Dimensões da janela
const GLuint WIDTH = 1000, HEIGHT = 1000;

// Código fonte do Vertex Shader
const GLchar* vertexShaderSource = "#version 450\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"uniform mat4 model;\n"
"out vec4 finalColor;\n"
"void main()\n"
"{\n"
"gl_Position = model * vec4(position, 1.0);\n"
"finalColor = vec4(color, 1.0);\n"
"}\0";

// Código fonte do Fragment Shader
const GLchar* fragmentShaderSource = "#version 450\n"
"in vec4 finalColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = finalColor;\n"
"}\n\0";

// --- Controles de rotação ---
bool rotateX = false, rotateY = false, rotateZ = false;

// --- Cubo selecionado atualmente (0 ou 1) ---
int selectedCube = 0;

// --- Estado de cada cubo: translação e escala ---
struct CubeState {
	glm::vec3 position;
	float scale;
};

// Dois cubos na cena
CubeState cubes[2] = {
	{ glm::vec3(-0.6f, 0.0f, 0.0f), 0.4f },
	{ glm::vec3( 0.6f, 0.0f, 0.0f), 0.4f }
};

// Função MAIN
int main()
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Hello Cube!", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version  = glGetString(GL_VERSION);
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	GLuint shaderID = setupShader();
	GLuint VAO      = setupGeometry();

	glUseProgram(shaderID);
	GLint modelLoc = glGetUniformLocation(shaderID, "model");

	glEnable(GL_DEPTH_TEST);

	cout << "\n=== CONTROLES ===" << endl;
	cout << "1 / 2      : selecionar cubo 1 ou 2" << endl;
	cout << "X / Y / Z  : rotacionar no eixo" << endl;
	cout << "W / S      : mover no eixo Z" << endl;
	cout << "A / D      : mover no eixo X" << endl;
	cout << "I / J      : mover no eixo Y" << endl;
	cout << "[ / ]      : diminuir / aumentar escala" << endl;
	cout << "ESC        : sair" << endl;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		float angle = (GLfloat)glfwGetTime();

		glBindVertexArray(VAO);

		// Desenha os dois cubos
		for (int i = 0; i < 2; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);

			// Translação
			model = glm::translate(model, cubes[i].position);

			// Rotação (aplicada apenas ao cubo selecionado, mas pode ser global)
			if (rotateX)
				model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
			else if (rotateY)
				model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
			else if (rotateZ)
				model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));

			// Escala uniforme
			model = glm::scale(model, glm::vec3(cubes[i].scale));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			// 36 vértices = 6 faces * 2 triângulos * 3 vértices
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glDrawArrays(GL_POINTS, 0, 36);
		}

		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	const float step  = 0.05f;
	const float sStep = 0.05f;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	// Selecionar cubo
	if (key == GLFW_KEY_1 && action == GLFW_PRESS) { selectedCube = 0; cout << "Cubo 1 selecionado" << endl; }
	if (key == GLFW_KEY_2 && action == GLFW_PRESS) { selectedCube = 1; cout << "Cubo 2 selecionado" << endl; }

	// Rotação
	if (key == GLFW_KEY_X && action == GLFW_PRESS) { rotateX = true;  rotateY = false; rotateZ = false; }
	if (key == GLFW_KEY_Y && action == GLFW_PRESS) { rotateX = false; rotateY = true;  rotateZ = false; }
	if (key == GLFW_KEY_Z && action == GLFW_PRESS) { rotateX = false; rotateY = false; rotateZ = true;  }

	// Translação — WASD para X/Z, I/J para Y
	// Aceita PRESS e REPEAT para segurar a tecla
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		if (key == GLFW_KEY_D) cubes[selectedCube].position.x += step;
		if (key == GLFW_KEY_A) cubes[selectedCube].position.x -= step;
		if (key == GLFW_KEY_I) cubes[selectedCube].position.y += step;
		if (key == GLFW_KEY_J) cubes[selectedCube].position.y -= step;
		if (key == GLFW_KEY_W) cubes[selectedCube].position.z += step;
		if (key == GLFW_KEY_S) cubes[selectedCube].position.z -= step;

		// Escala uniforme
		if (key == GLFW_KEY_RIGHT_BRACKET) // ]
			cubes[selectedCube].scale += sStep;
		if (key == GLFW_KEY_LEFT_BRACKET)  // [
			cubes[selectedCube].scale = glm::max(0.05f, cubes[selectedCube].scale - sStep);
	}
}

int setupShader()
{
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shaderProgram;
}

int setupGeometry()
{
	// Cubo: 6 faces, cada face = 2 triângulos = 6 vértices
	// Cada vértice: x, y, z, r, g, b
	// Cada face tem uma cor diferente
	GLfloat vertices[] = {
		// Face FRONTAL (z = +0.5) — vermelho
		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

		// Face TRASEIRA (z = -0.5) — verde
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,

		// Face ESQUERDA (x = -0.5) — azul
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,

		// Face DIREITA (x = +0.5) — amarelo
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,

		// Face INFERIOR (y = -0.5) — ciano
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,

		// Face SUPERIOR (y = +0.5) — magenta
		-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
	};

	GLuint VBO, VAO;

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Atributo posição (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
}
