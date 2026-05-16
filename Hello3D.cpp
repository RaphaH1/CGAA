/* Hello Cube - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 * Aluno: Raphael Ferracioli
 *
 * Adaptado por Rossana Baptista Queiroz
 * para as disciplinas de Processamento Gráfico/Computação Gráfica - Unisinos
 * Modificado para incluir cubo, translação, escala e múltiplas instâncias
 */

#include <iostream>
#include <string>
#include <vector>
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
"uniform float highlight;\n"
"out vec4 finalColor;\n"
"void main()\n"
"{\n"
"gl_Position = model * vec4(position, 1.0);\n"
"vec3 c = color + vec3(highlight * 0.25);\n"
"finalColor = vec4(clamp(c, 0.0, 1.0), 1.0);\n"
"}\0";

// Código fonte do Fragment Shader
const GLchar* fragmentShaderSource = "#version 450\n"
"in vec4 finalColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = finalColor;\n"
"}\n\0";

// --- Modo de transformação e eixo ativo ---
enum class TransformMode { NONE, ROTATE, TRANSLATE, SCALE };
enum class Axis { NONE, X, Y, Z };

TransformMode currentMode = TransformMode::NONE;
Axis          currentAxis = Axis::NONE;

// --- Struct que representa um objeto 3D na cena ---
struct Object3D {
	GLuint    VAO       = 0;
	int       numVerts  = 0;
	glm::vec3 position  = glm::vec3(0.0f);
	glm::vec3 scale     = glm::vec3(1.0f);
	float     rotAngleX = 0.0f;
	float     rotAngleY = 0.0f;
	float     rotAngleZ = 0.0f;

	glm::mat4 modelMatrix() const {
		glm::mat4 m = glm::mat4(1.0f);
		m = glm::translate(m, position);
		m = glm::rotate(m, rotAngleX, glm::vec3(1.0f, 0.0f, 0.0f));
		m = glm::rotate(m, rotAngleY, glm::vec3(0.0f, 1.0f, 0.0f));
		m = glm::rotate(m, rotAngleZ, glm::vec3(0.0f, 0.0f, 1.0f));
		m = glm::scale(m, scale);
		return m;
	}
};

// --- Objetos na cena e índice do selecionado ---
vector<Object3D> objects;
int selectedIndex = 0;

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

	// --- Criação dos objetos na cena ---
	{
		Object3D obj;
		obj.VAO      = VAO;
		obj.numVerts = 36;
		obj.position = glm::vec3(-0.55f, 0.0f, 0.0f);
		obj.scale    = glm::vec3(0.4f);
		objects.push_back(obj);
	}
	{
		Object3D obj;
		obj.VAO      = VAO;
		obj.numVerts = 36;
		obj.position = glm::vec3( 0.55f, 0.0f, 0.0f);
		obj.scale    = glm::vec3(0.4f);
		objects.push_back(obj);
	}
	{
		Object3D obj;
		obj.VAO      = VAO;
		obj.numVerts = 36;
		obj.position = glm::vec3(0.0f, 0.55f, 0.0f);
		obj.scale    = glm::vec3(0.25f);
		objects.push_back(obj);
	}

	glUseProgram(shaderID);
	GLint modelLoc     = glGetUniformLocation(shaderID, "model");
	GLint highlightLoc = glGetUniformLocation(shaderID, "highlight");

	glEnable(GL_DEPTH_TEST);

	cout << "\n=== CONTROLES ===" << endl;
	cout << "TAB        : selecionar proximo objeto" << endl;
	cout << "R          : modo Rotacao   -> X, Y ou Z -> setas" << endl;
	cout << "T          : modo Translacao -> X, Y ou Z -> setas" << endl;
	cout << "S          : modo Escala    -> X, Y ou Z -> setas (sem eixo = uniforme)" << endl;
	cout << "ESC        : sair" << endl;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		for (int i = 0; i < (int)objects.size(); i++)
		{
			glBindVertexArray(objects[i].VAO);

			glm::mat4 model = objects[i].modelMatrix();
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			// Destaque visual para o objeto selecionado
			glUniform1f(highlightLoc, (i == selectedIndex) ? 1.0f : 0.0f);

			// 36 vértices = 6 faces * 2 triângulos * 3 vértices
			glDrawArrays(GL_TRIANGLES, 0, objects[i].numVerts);
			glDrawArrays(GL_POINTS,    0, objects[i].numVerts);
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
	const float rStep = glm::radians(5.0f);

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	// Selecionar próximo objeto (TAB cicla pelo vector)
	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
	{
		selectedIndex = (selectedIndex + 1) % (int)objects.size();
		currentMode = TransformMode::NONE;
		currentAxis = Axis::NONE;
		cout << "Objeto " << selectedIndex << " selecionado" << endl;
	}

	// Seleção de modo de transformação
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_R) { currentMode = TransformMode::ROTATE;    currentAxis = Axis::NONE; cout << "[Modo: ROTACAO]    -> X, Y ou Z" << endl; }
		if (key == GLFW_KEY_T) { currentMode = TransformMode::TRANSLATE; currentAxis = Axis::NONE; cout << "[Modo: TRANSLACAO] -> X, Y ou Z" << endl; }
		if (key == GLFW_KEY_S) { currentMode = TransformMode::SCALE;     currentAxis = Axis::NONE; cout << "[Modo: ESCALA]     -> X, Y ou Z (sem eixo = uniforme)" << endl; }

		// Seleção de eixo
		if (key == GLFW_KEY_X) { currentAxis = Axis::X; cout << "  eixo X" << endl; }
		if (key == GLFW_KEY_Y) { currentAxis = Axis::Y; cout << "  eixo Y" << endl; }
		if (key == GLFW_KEY_Z) { currentAxis = Axis::Z; cout << "  eixo Z" << endl; }
	}

	// Aplicação da transformação (PRESS e REPEAT para segurar a tecla)
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		int sign = 0;
		if (key == GLFW_KEY_UP   || key == GLFW_KEY_W) sign = +1;
		if (key == GLFW_KEY_DOWN || key == GLFW_KEY_S) sign = -1;

		if (sign != 0 && currentMode != TransformMode::NONE)
		{
			Object3D& obj = objects[selectedIndex];

			if (currentMode == TransformMode::ROTATE)
			{
				float delta = sign * rStep;
				if      (currentAxis == Axis::X) obj.rotAngleX += delta;
				else if (currentAxis == Axis::Y) obj.rotAngleY += delta;
				else if (currentAxis == Axis::Z) obj.rotAngleZ += delta;
				else                             obj.rotAngleY += delta;
			}
			else if (currentMode == TransformMode::TRANSLATE)
			{
				float delta = sign * step;
				if      (currentAxis == Axis::X) obj.position.x += delta;
				else if (currentAxis == Axis::Y) obj.position.y += delta;
				else if (currentAxis == Axis::Z) obj.position.z += delta;
				else                             obj.position.x += delta;
			}
			else if (currentMode == TransformMode::SCALE)
			{
				float delta = sign * sStep;
				if      (currentAxis == Axis::X) obj.scale.x = glm::max(0.05f, obj.scale.x + delta);
				else if (currentAxis == Axis::Y) obj.scale.y = glm::max(0.05f, obj.scale.y + delta);
				else if (currentAxis == Axis::Z) obj.scale.z = glm::max(0.05f, obj.scale.z + delta);
				else {
					// Escala uniforme
					obj.scale.x = glm::max(0.05f, obj.scale.x + delta);
					obj.scale.y = glm::max(0.05f, obj.scale.y + delta);
					obj.scale.z = glm::max(0.05f, obj.scale.z + delta);
				}
			}
		}
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
