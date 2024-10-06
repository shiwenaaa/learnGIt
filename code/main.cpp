/*
 *        Computer Graphics Course - Shenzhen University
 *    Mid-term Assignment - Tetris implementation sample code
 * ============================================================
 *
 * - 本代码仅仅是参考代码，具体要求请参考作业说明，按照顺序逐步完成。
 * - 关于配置OpenGL开发环境、编译运行，请参考第一周实验课程相关文档。
 *
 * - 已实现功能如下：
 * - 1) 绘制棋盘格和‘L’型方块
 * - 2) 键盘左/右/下键控制方块的移动，上键旋转方块
 * ============================================================
 * -本实验实现的功能如下：
 * - 1) 绘制‘J’、‘Z’等形状的方块
 * - 2) 随机生成方块并赋上不同的颜色
 * - 3) 方块的自动向下移动
 * - 4) 方块之间、方块与边界之间的碰撞检测
 * - 5) 棋盘格中每一行填充满之后自动消除
 * - 6) 完成restart()函数使得按r键游戏重启
 * - 7) 完成按空格键方块迅速下落
 * - 8) 添加游戏向导，在控制面板输出游戏规则，进行游戏难度设置
 *      ！！！！！注意要输入游戏难度等级才能启动游戏！！！！！！！！
 * - 9) 添加了计分板，利用文件保存和读取历史最高分
 * - 10) 添加了游戏结束的判断，即没有空间再产生新的方块
 * - 11) 进行游戏难度梯度设置，每增加200分下落速度加1，但最多不会超过一秒5格
 * - 12) 增加了S键游戏暂停功能，完善了按键逻辑
 */

#include "Angel.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
#include <fstream>

using namespace std;

int starttime;			// 控制方块向下移动时间
int rotation = 0;		// 控制当前窗口中的方块旋转
glm::vec2 tile[4];			// 表示当前窗口中的方块
bool gameover = false;	// 游戏结束控制变量
int xsize = 400;		// 窗口大小（尽量不要变动窗口大小！）
int ysize = 720;
int shapenum = 0;       //方块类型
float speed = -1;		//初始下落速度为每隔一秒下降一个格子
int score = 0;			//分数
int maxscore = 0;		//历史最高分
int myrank = 1;			//游戏难度初始为1
bool stop = false;
int colornum = 0;		//颜色

// 单个网格大小
int tile_width = 33;

// 网格布大小
const int board_width = 10;
const int board_height = 20;

// 网格三角面片的顶点数量
const int points_num = board_height * board_width * 6;

// 我们用画直线的方法绘制网格
// 包含竖线 board_width+1 条
// 包含横线 board_height+1 条
// 一条线2个顶点坐标
// 网格线的数量
const int board_line_num =  (board_width + 1) + (board_height + 1);
bool checkvalid(glm::vec2 cellpos);
bool movetile(glm::vec2 direction);

void saveMaxScore() {	//保存历史最高分
	std::ofstream outFile("data.txt");
	if (outFile) {
		outFile << maxscore;
	}
	outFile.close();
}
void loadMaxScore() {	//读取历史最高分
	std::ifstream inFile("data.txt");
	if (inFile) {
		inFile >> maxscore;
	}
	inFile.close();
}

// 一个二维数组表示所有可能出现的方块和方向。
glm::vec2 allRotationsLshape[7][4][4] ={
	//1：O型方块
{  {glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(0, -1), glm::vec2(-1,-1)},
   {glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(0, -1), glm::vec2(-1,-1)},   
   {glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(0, -1), glm::vec2(-1,-1)},    
   {glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(0, -1), glm::vec2(-1,-1)}  },

   //2:I型方块
{	{glm::vec2(-2, 0), glm::vec2(-1,0), glm::vec2(1, 0), glm::vec2(0,0)},	
	{glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(0,-1), glm::vec2(0, -2)},   
	{glm::vec2(-2, 0), glm::vec2(-1,0), glm::vec2(1, 0), glm::vec2(0,0)},     
	{glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(0,-1), glm::vec2(0, -2)}  },

   //3:S型方块
{	{glm::vec2(0, 0), glm::vec2(0,-1), glm::vec2(-1, -1), glm::vec2(1,0)},	
	{glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(1,0), glm::vec2(1, -1)},    
	{glm::vec2(0, 0), glm::vec2(0,-1), glm::vec2(-1, -1), glm::vec2(1,0)},    
	{glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(1,0), glm::vec2(1, -1)}  },

   //4:Z型方块
{	{glm::vec2(-1, 0), glm::vec2(0,0), glm::vec2(0, -1), glm::vec2(1,-1)},	
	{glm::vec2(0, 0), glm::vec2(0, -1), glm::vec2(1,0), glm::vec2(1, 1)},    
	{glm::vec2(-1, 0), glm::vec2(0,0), glm::vec2(0, -1), glm::vec2(1,-1)},  
	{glm::vec2(0, 0), glm::vec2(0, -1), glm::vec2(1,0), glm::vec2(1, 1)}  },

   //5:L型方块
{	{glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(1, 0), glm::vec2(-1,-1)},	
	{glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(0,-1), glm::vec2(1, -1)},     
	{glm::vec2(1, 1), glm::vec2(-1,0), glm::vec2(0, 0), glm::vec2(1,  0)},   
	{glm::vec2(-1,1), glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(0, -1)}  },

   //6:J型方块
{	{glm::vec2(-1, 0), glm::vec2(0,0), glm::vec2(1, 0), glm::vec2(1,-1)},	
	{glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(0,-1), glm::vec2(1, 1)},  
	{glm::vec2(-1, 0), glm::vec2(0,0), glm::vec2(1, 0), glm::vec2(-1,  1)},     
	{glm::vec2(-1,-1), glm::vec2(0, -1), glm::vec2(0, 0), glm::vec2(0, 1)} },

   //7:T型方块
{	{glm::vec2(-1, 0), glm::vec2(0,0), glm::vec2(1, 0), glm::vec2(0,-1)},	
	{glm::vec2(0, -1), glm::vec2(0, 0), glm::vec2(0,1), glm::vec2(1, 0)},    
	{glm::vec2(-1, 0), glm::vec2(0,0), glm::vec2(1, 0), glm::vec2(0,  1)},      
	{glm::vec2(-1,0), glm::vec2(0, -1), glm::vec2(0, 0), glm::vec2(0, 1)}  },

};

// 绘制窗口的颜色变量
glm::vec4 orange = glm::vec4(1.0, 0.5, 0.0, 1.0);
glm::vec4 white  = glm::vec4(1.0, 1.0, 1.0, 1.0);
glm::vec4 black  = glm::vec4(0.0, 0.0, 0.0, 1.0);
glm::vec4 red = glm::vec4(1.0, 0.0, 0.0, 1.0);
glm::vec4 green = glm::vec4(0.0, 1.0, 0.0, 1.0);
glm::vec4 blue = glm::vec4(0.0, 0.0, 1.0, 1.0);
glm::vec4 yellow = glm::vec4(1.0, 1.0, 0.0, 1.0);
glm::vec4 purple = glm::vec4(0.5, 0.5, 1.0, 1.0);
glm::vec4 pink = glm::vec4(1.0, 0.54, 0.98, 1.0);
glm::vec4 colors[] = { orange,red,green,blue,yellow,purple,pink };


// 当前方块的位置（以棋盘格的左下角为原点的坐标系）
glm::vec2 tilepos = glm::vec2(5, 19);

// 布尔数组表示棋盘格的某位置是否被方块填充，即board[x][y] = true表示(x,y)处格子被填充。
// （以棋盘格的左下角为原点的坐标系）
bool board[board_width][board_height];

// 当棋盘格某些位置被方块填充之后，记录这些位置上被填充的颜色
glm::vec4 board_colours[points_num];

GLuint locxsize;
GLuint locysize;

GLuint vao[3];
GLuint vbo[6];

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// 修改棋盘格在pos位置的颜色为colour，并且更新对应的VBO
void changecellcolour(glm::vec2 pos, glm::vec4 colour)
{
	// 每个格子是个正方形，包含两个三角形，总共6个定点，并在特定的位置赋上适当的颜色
	for (int i = 0; i < 6; i++)
		board_colours[(int)( 6 * ( board_width*pos.y + pos.x) + i)] = colour;

	glm::vec4 newcolours[6] = {colour, colour, colour, colour, colour, colour};

	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);

	// 计算偏移量，在适当的位置赋上颜色
	int offset = 6 * sizeof(glm::vec4) * (int)( board_width * pos.y + pos.x);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(newcolours), newcolours);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// 当前方块移动或者旋转时，更新VBO
void updatetile()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);

	// 每个方块包含四个格子
	for (int i = 0; i < 4; i++)
	{
		// 计算格子的坐标值
		GLfloat x = tilepos.x + tile[i].x;
		GLfloat y = tilepos.y + tile[i].y;

		glm::vec4 p1 = glm::vec4(tile_width + (x * tile_width), tile_width + (y * tile_width), .4, 1);
		glm::vec4 p2 = glm::vec4(tile_width + (x * tile_width), tile_width*2 + (y * tile_width), .4, 1);
		glm::vec4 p3 = glm::vec4(tile_width*2 + (x * tile_width), tile_width + (y * tile_width), .4, 1);
		glm::vec4 p4 = glm::vec4(tile_width*2 + (x * tile_width), tile_width*2 + (y * tile_width), .4, 1);

		// 每个格子包含两个三角形，所以有6个顶点坐标
		glm::vec4 newpoints[6] = {p1, p2, p3, p2, p3, p4};
		glBufferSubData(GL_ARRAY_BUFFER, i*6*sizeof(glm::vec4), 6*sizeof(glm::vec4), newpoints);
	}
	glBindVertexArray(0);
	
}

// 设置当前方块为下一个即将出现的方块。在游戏开始的时候调用来创建一个初始的方块，
// 在游戏结束的时候判断，没有足够的空间来生成新的方块。
void newtile()
{
	int flag = 0;
	for (int i = 4; i < 7; i++) {
		for (int j = 19; j < 20; j++) {	//中间最后一行没有空格
			if (board[i][j] == true)
				flag = 1;
		}
	}
	// 将新方块放于棋盘格的最上行中间位置
	tilepos = glm::vec2(5 , 19);

	srand(static_cast<unsigned int>(time(0)));//设置随机种子
	shapenum = rand() % 7;	//随机产生类型
	rotation = rand() % 4;	//随机产生方向
	for (int i = 0; i < 4; i++)
	{
		//cout << shapenum << "  " << rotation << endl;
		//tile[i] = allRotationsLshape[shapenum][rotation][i];
		tile[i] = allRotationsLshape[shapenum][rotation][i];
		//检测是否有位置放置新方块
	if (!checkvalid(tile[i] + tilepos) && flag == 1) {
		gameover = true;
		system("cls");
		cout << "游戏结束！"<<endl;
		if (score > maxscore) {
			
			cout << "恭喜你打破游戏记录!" << endl;
			cout << left << setw(15) << "历史最高分为：" << maxscore << endl;
			maxscore = score;
			saveMaxScore();
		}
		cout << left << setw(15) << "你的分数为：" << score << endl;
		cout << left << setw(15) << "游戏等级为：" << myrank << endl;
		cout << left << setw(15) << "最高分为：" << maxscore << endl;
		cout << "--------------------------------------" << endl;
		cout << "请按q/Esc键退出游戏或按r键重新开始游戏" << endl;
		return;
	}
	}

	updatetile();

	colornum = rand() % 7;
	// 给新方块赋上颜色
	glm::vec4 newcolours[24];
	for (int i = 0; i < 24; i++)
		//newcolours[i] = colors[shapenum];
		newcolours[i] = colors[colornum];

	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

// 游戏和OpenGL初始化
void init()
{
	gameover = false;
	stop = false;	
	//游戏向导
	cout << "欢迎来到俄罗斯方块游戏！" << endl;
	cout << "游戏规则：调整下落方块使其能成功放置，填充满一行可消除获得分数，放置不下则游戏结束" << endl;
	cout << "按键说明：" << left << setw(30) << "使用上键旋转方块" << "使用左或右键移动方块\n"
		<< setw(10) << "" << setw(30) << "使用下键加速方块下落" << "使用空格键下落方块至底部\n"
		<< setw(10) << "" << setw(30) << "使用q键或Esc键退出游戏" << "使用r键重新开始游戏\n" 
		<< setw(10) << "" << setw(30) << "使用s键暂停或继续游戏\n" << endl;
	cout << "游戏等级有1、2、3三种选择，等级越高游戏越难，请选择你的游戏等级：";
	cin >> myrank;
	speed -= myrank - 1;//根据选择的游戏等级设置下落速度
	cout << "游戏开始！" << endl;
	cout << " ------------------------" << endl;
	cout << left << setw(15) << "|游戏等级：" << setw(10) << myrank << "|" << endl;
	cout << left << setw(15) << "|我的分数：" << setw(10) << score << "|" << endl;
	cout << left << setw(15) << "|历史最高分：" << setw(10) << maxscore << "|" << endl;
	cout << " ------------------------" << endl;

	// 初始化棋盘格，这里用画直线的方法绘制网格
	// 包含竖线 board_width+1 条
	// 包含横线 board_height+1 条
	// 一条线2个顶点坐标，并且每个顶点一个颜色值
	
	glm::vec4 gridpoints[board_line_num * 2];
	glm::vec4 gridcolours[board_line_num * 2];

	// 绘制网格线
	// 纵向线
	for (int i = 0; i < (board_width+1); i++)
	{
		gridpoints[2*i] = glm::vec4((tile_width + (tile_width * i)), tile_width, 0, 1);
		gridpoints[2*i + 1] = glm::vec4((tile_width + (tile_width * i)), (board_height+1) * tile_width, 0, 1);
	}

	// 水平线
	for (int i = 0; i < (board_height+1); i++)
	{
		gridpoints[ 2*(board_width+1) + 2*i ] = glm::vec4(tile_width, (tile_width + (tile_width * i)), 0, 1);
		gridpoints[ 2*(board_width+1) + 2*i + 1 ] = glm::vec4((board_width+1) * tile_width, (tile_width + (tile_width * i)), 0, 1);
	}

	// 将所有线赋成白色
	for (int i = 0; i < (board_line_num * 2); i++)
		gridcolours[i] = white;

	// 初始化棋盘格，并将没有被填充的格子设置成黑色
	glm::vec4 boardpoints[points_num];
	for (int i = 0; i < points_num; i++)
		board_colours[i] = black;

	// 对每个格子，初始化6个顶点，表示两个三角形，绘制一个正方形格子
	for (int i = 0; i < board_height; i++)
		for (int j = 0; j < board_width; j++)
		{
			glm::vec4 p1 = glm::vec4(tile_width + (j * tile_width), tile_width + (i * tile_width), .5, 1);
			glm::vec4 p2 = glm::vec4(tile_width + (j * tile_width), tile_width*2 + (i * tile_width), .5, 1);
			glm::vec4 p3 = glm::vec4(tile_width*2 + (j * tile_width), tile_width + (i * tile_width), .5, 1);
			glm::vec4 p4 = glm::vec4(tile_width*2 + (j * tile_width), tile_width*2 + (i * tile_width), .5, 1);
			boardpoints[ 6 * ( board_width * i + j ) + 0 ] = p1;
			boardpoints[ 6 * ( board_width * i + j ) + 1 ] = p2;
			boardpoints[ 6 * ( board_width * i + j ) + 2 ] = p3;
			boardpoints[ 6 * ( board_width * i + j ) + 3 ] = p2;
			boardpoints[ 6 * ( board_width * i + j ) + 4 ] = p3;
			boardpoints[ 6 * ( board_width * i + j ) + 5 ] = p4;
		}

	// 将棋盘格所有位置的填充与否都设置为false（没有被填充）
	for (int i = 0; i < board_width; i++)
		for (int j = 0; j < board_height; j++)
			board[i][j] = false;

	// 载入着色器
	std::string vshader, fshader;
	vshader = "shaders/vshader.glsl";
	fshader = "shaders/fshader.glsl";
	GLuint program = InitShader(vshader.c_str(), fshader.c_str());
	glUseProgram(program);

	locxsize = glGetUniformLocation(program, "xsize");
	locysize = glGetUniformLocation(program, "ysize");

	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	GLuint vColor = glGetAttribLocation(program, "vColor");

	
	glGenVertexArrays(3, &vao[0]);
	glBindVertexArray(vao[0]);		// 棋盘格顶点
	
	glGenBuffers(2, vbo);

	// 棋盘格顶点位置
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, (board_line_num * 2) * sizeof(glm::vec4), gridpoints, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// 棋盘格顶点颜色
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, (board_line_num * 2) * sizeof(glm::vec4), gridcolours, GL_STATIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);

	
	glBindVertexArray(vao[1]);		// 棋盘格每个格子

	glGenBuffers(2, &vbo[2]);

	// 棋盘格每个格子顶点位置
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, points_num*sizeof(glm::vec4), boardpoints, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// 棋盘格每个格子顶点颜色
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, points_num*sizeof(glm::vec4), board_colours, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);

	
	glBindVertexArray(vao[2]);		// 当前方块

	glGenBuffers(2, &vbo[4]);

	// 当前方块顶点位置
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, 24*sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// 当前方块顶点颜色
	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferData(GL_ARRAY_BUFFER, 24*sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);

	
	glBindVertexArray(0);

	glClearColor(0, 0, 0, 0);

	glClear(GL_COLOR_BUFFER_BIT);

	// 游戏初始化
	newtile();
	//starttime = glutGet(GLUT_ELAPSED_TIME);
}

// 检查在cellpos位置的格子是否被填充或者是否在棋盘格的边界范围内
bool checkvalid(glm::vec2 cellpos)
{
	if((cellpos.x >=0) && (cellpos.x < board_width) && (cellpos.y >= 0) && (cellpos.y < board_height) 
		&&(board[(int)cellpos.x][(int)cellpos.y]==false))
		return true;
	else
		return false;
}

// 在棋盘上有足够空间的情况下旋转当前方块
void rotate()
{
	// 计算得到下一个旋转方向
	int nextrotation = (rotation + 1) % 4;

	// 检查当前旋转之后的位置的有效性
	if (checkvalid((allRotationsLshape[shapenum][nextrotation][0]) + tilepos)
		&& checkvalid((allRotationsLshape[shapenum][nextrotation][1]) + tilepos)
		&& checkvalid((allRotationsLshape[shapenum][nextrotation][2]) + tilepos)
		&& checkvalid((allRotationsLshape[shapenum][nextrotation][3]) + tilepos))
	{
		// 更新旋转，将当前方块设置为旋转之后的方块
		rotation = nextrotation;
		for (int i = 0; i < 4; i++)
			tile[i] = allRotationsLshape[shapenum][rotation][i];

		updatetile();
	}
}


void showmyscore() {
	system("cls");
	cout << "欢迎来到俄罗斯方块游戏！" << endl;
	cout << "游戏规则：调整下落方块使其能成功放置，填充满一行可消除获得分数，放置不下则游戏结束" << endl;
	cout << "按键说明：" << left << setw(30) << "使用上键旋转方块" << "使用左或右键移动方块\n"
		<< setw(10) << "" << setw(30) << "使用下键加速方块下落" << "使用空格键下落方块至底部\n"
		<< setw(10) << "" << setw(30) << "使用q键或Esc键退出游戏" << "使用r键重新开始游戏\n" 
		<< setw(10) << "" << setw(30) << "使用s键暂停或继续游戏\n" << endl << endl << endl;
	
	cout << " ------------------------" << endl;
	cout << left << setw(15) << "|游戏等级：" << setw(10) << myrank << "|" << endl;
	cout << left << setw(15) << "|我的分数：" << setw(10) << score << "|" << endl;
	cout << left << setw(15) << "|历史最高分：" << setw(10) << maxscore << "|" << endl;
	cout << " ------------------------" << endl;
}



// 检查棋盘格在row行有没有被填充满
void checkfullrow(int row)
{
	int flag = 1;	//标志位，为1表示被填满
	for (int i = 0; i < board_width; i++) {
		if (board[i][row] == false) {	//该行仍有格子
			flag = 0;
		}
	}
	if (flag == 1) {
		score += 5;
		if (score % 200 == 0 && speed >= -5) {
			speed -= 1;
		}

		showmyscore();

		//cout << "row:"<<row<<"满了"<<endl;
		for (int i = 0; i < board_width; i++) {
			changecellcolour(glm::vec2(i, row), black);	//颜色全部变黑
			board[i][row] = false;	//格子状态修改为未填满	
		}
		for (int i = row + 1; i < board_height; i++) {
			for (int k = 0; k < 4 && i < row + 4; k++) {//如果本方块有在上一行，要把方块纵坐标减1避免漏判
				int y = (tile[k] + tilepos).y;	
				if (y == i) {
					tile[k].y -= 1;
					//cout << "有我在！！" << endl;
				}
			}
			for (int j = 0; j < board_width; j++) {
				if (board[j][i] == true) {	//如果上一行有方格，则需要往下掉
					
					board[j][i] = false;	//将上一行清空
					changecellcolour(glm::vec2(j, i - 1), board_colours[(int)(6 * (board_width * glm::vec2(j, i).y + glm::vec2(j, i).x))]);
					//设置为上一行格子颜色
					changecellcolour(glm::vec2(j, i), black);
					board[j][i-1] = true;
				}
			}
		}
	}
}

// 放置当前方块，并且更新棋盘格对应位置顶点的颜色VBO
void settile()
{
	while (movetile(glm::vec2(0, -1))) {
	};	//确保底下没有未填满的空格

	// 每个格子
	for (int i = 0; i < 4; i++)
	{
		// 获取格子在棋盘格上的坐标
		int x = (tile[i] + tilepos).x;
		int y = (tile[i] + tilepos).y;
		// 将格子对应在棋盘格上的位置设置为填充
		board[x][y] = true;
		// 并将相应位置的颜色修改
		//changecellcolour(glm::vec2(x, y), colors[shapenum]);
		changecellcolour(glm::vec2(x, y), colors[colornum]);

	}

	//每放置完一个方块都要判断该行是否填满
	for (int i = 0; i < 4; i++) {
		int y = (tile[i] + tilepos).y;
		checkfullrow(y);
	}
}

// 给定位置(x,y)，移动方块。有效的移动值为(-1,0)，(1,0)，(0,-1)，分别对应于向
// 左，向下和向右移动。如果移动成功，返回值为true，反之为false
bool movetile(glm::vec2 direction)
{
	// 计算移动之后的方块的位置坐标
	glm::vec2 newtilepos[4];
	for (int i = 0; i < 4; i++)
		newtilepos[i] = tile[i] + tilepos + direction;

	// 检查移动之后的有效性
	if (checkvalid(newtilepos[0])
		&& checkvalid(newtilepos[1])
		&& checkvalid(newtilepos[2])
		&& checkvalid(newtilepos[3]))
		{
			// 有效：移动该方块
			tilepos.x = tilepos.x + direction.x;
			tilepos.y = tilepos.y + direction.y;

			updatetile();

			return true;
		}

	return false;
}

// 重新启动游戏
void restart()
{
	system("cls");	//清楚终端信息
	cout << "提示：游戏已重新开始!"<<endl;	//输出重启提示
	init();	//重新启动
}

// 游戏渲染部分
void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glUniform1i(locxsize, xsize);
	glUniform1i(locysize, ysize);

	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, points_num); // 绘制棋盘格 (width * height * 2 个三角形)
	glBindVertexArray(vao[2]);
	glDrawArrays(GL_TRIANGLES, 0, 24);	 // 绘制当前方块 (8 个三角形)
	glBindVertexArray(vao[0]);
	glDrawArrays(GL_LINES, 0, board_line_num * 2 );		 // 绘制棋盘格的线

}

// 在窗口被拉伸的时候，控制棋盘格的大小，使之保持固定的比例。
void reshape(GLsizei w, GLsizei h)
{
	xsize = w;
	ysize = h;
	glViewport(0, 0, w, h);
}

// 键盘响应事件中的特殊按键响应
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	switch (key) {
		case GLFW_KEY_ESCAPE:
			if (action == GLFW_PRESS) {
				exit(EXIT_SUCCESS);
				break;
			}
			else
			{
				break;
			}
		case GLFW_KEY_Q:
			if (action == GLFW_PRESS) {
				exit(EXIT_SUCCESS);
				break;
		}
			else
		{
			break;
		}

		case GLFW_KEY_R:
			if (action == GLFW_PRESS) {
				restart();
				break;
			}
			else
			{
				break;
			}

		case GLFW_KEY_S:
			if (action == GLFW_PRESS && stop == false && gameover == false) {
				stop = true;
				//cout << "游戏暂停了" << endl;
				break;
			}
			else if (action == GLFW_PRESS && stop == true && gameover == false) {
				stop = false;
				//cout << "游戏继续了" << endl;
				break;
			}
			else
			{
				break;
			}
	}

	if(!gameover&&!stop)
	{
		switch(key)
		{	
			// 控制方块的移动方向，更改形态
			case GLFW_KEY_UP:	// 向上按键旋转方块
				if (action == GLFW_PRESS || action == GLFW_REPEAT)
				{
					rotate();
					break;
				}
				else
				{
					break;
				}
			case GLFW_KEY_DOWN: // 向下按键移动方块
				if (action == GLFW_PRESS || action == GLFW_REPEAT){
					if (!movetile(glm::vec2(0, -1)))
					{
						settile();
						newtile();
						break;
					}
					else
					{
						break;
				}
				}
			case GLFW_KEY_LEFT:  // 向左按键移动方块
				if (action == GLFW_PRESS || action == GLFW_REPEAT){
					movetile(glm::vec2(-1, 0));
					break;
				}
				else
				{
					break;
				}
			case GLFW_KEY_RIGHT: // 向右按键移动方块
				if (action == GLFW_PRESS || action == GLFW_REPEAT){
					movetile(glm::vec2(1, 0));
					break;
				}
				else
				{
					break;
				}
			case GLFW_KEY_SPACE: // 按空格键迅速降落方块
				if (action == GLFW_PRESS || action == GLFW_REPEAT) {
					while (movetile(glm::vec2(0, -1)))
					{
					}
					settile();
					newtile();
					break;
				}
		}
	}
	
}

int main(int argc, char **argv)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
	#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	// 创建窗口。
#pragma execution_character_set("utf-8");
	GLFWwindow* window = glfwCreateWindow(500, 900, "2022150198_梁文诗_期中大作业", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
	
	loadMaxScore();
	
	init();
	starttime = clock();
	while (!glfwWindowShouldClose(window))
    { 
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();	

		//方块自动向下移动
		int nowtime = clock();
		if (nowtime - starttime >= 1000 && gameover == false && stop == false) {
			if (!movetile(glm::vec2(0, speed))) {
				settile();
				newtile();
			}
			starttime = nowtime;
		}

    }

    glfwTerminate();
    return 0;
}
