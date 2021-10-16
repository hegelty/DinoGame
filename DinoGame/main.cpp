#pragma warning(disable:4996)
#pragma warning(disable:6031)

#include <conio.h>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <windows.h>

using namespace std;

#define cls system("cls");

#define CONSOLE_COLS 150
#define CONSOLE_LINES 30
#define DINO_SIZE_X 24
#define DINO_SIZE_Y 10
#define TREE_SIZE_X 7
#define TREE_SIZE_Y 6
#define TALL_TREE_SIZE_X 7
#define TALL_TREE_SIZE_Y 9
#define CLOUD_SIZE_X 10
#define CLOUD_SIZE_Y 3
#define GRAVITY 60
#define ACCELERATION 35

typedef struct score_st
{
	int id;
	int score;
} score_st;

int cmp(const void* a, const void* b) { return (*(score_st*)b).score - (*(score_st*)a).score; }

double Dino_acceleration = 0, Dino_y = 0;
int Ms_per_tick = 100, Speed = 3;
int Tree_x = -1, Tree_left = 0, Tall_tree_x = 143, Tall_tree_left = 137, High_cloud_x = 143, high_cloud_left = 450, Low_cloud_x = 143, Low_cloud_left = 220;
int Score = 0;
int Score_data_cnt;
bool Jumping = false, Lying = false;
char Ground[CONSOLE_COLS];
score_st Score_arr[10000];

void game();

void load_score()
{
	FILE* score_data = fopen("./score.txt", "r");
	fscanf(score_data, "%d", &Score_data_cnt);
	for(int i=0;i<Score_data_cnt;i++)
	{
		fscanf(score_data, "%d %d", &Score_arr[i].id, &Score_arr[i].score);
	}
	fclose(score_data);
}

void save_score()
{
	FILE* score_data = fopen("./score.txt", "w");
	fprintf(score_data, "%d\n", Score_data_cnt + 1);
	for(int i=0;i<Score_data_cnt+1;i++)
	{
		fprintf(score_data, "%d %d\n", Score_arr[i].id, Score_arr[i].score);
	}
	fclose(score_data);
}

void goto_xy(int x, int y)
{
	COORD pos = { x,y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void set_console()
{
	system(("mode con:cols=" + to_string(CONSOLE_COLS) + " lines=" + to_string(CONSOLE_LINES)).c_str());
	system("title Dinosour Game by UNREVR");
}

void draw_ground()
{
	goto_xy(0, CONSOLE_LINES - 1);
	printf("%s", Ground);
}

void draw_dino(int dino_x, int dino_y, int dino_stat)
{
	char dino_img[3][DINO_SIZE_Y][DINO_SIZE_X] = {
							{
								"",
								"             #######",
								"            ## ######",
								"            ######",
								"  #        #########",
								"  ##    ##########",
								"  #############",
								"    ##########",
								"       #    ##",
								"       ##    ##"
							},
							{
								"",
								"             #######",
								"            ## ######",
								"            ######",
								"  #        #########",
								"  ##    ##########",
								"  #############",
								"    ##########",
								"       ##   #",
								"        ##  ##"
							},
							{
								"",
								"",
								"",
								"",
								"             ##########",
								"  #        ###### #####",
								"  ##    ############",
								"  ###################",
								"    ############",
								"        ### ###"
							}
	};
	for (int i = 0; i < DINO_SIZE_Y; i++)
	{
		goto_xy(dino_x, dino_y + i);
		printf("%s\n", dino_img[dino_stat][i]);
	}
}

void draw_tree(int tree_x)
{
	char tree_img[][TREE_SIZE_X] = {
				"   @  ",
				"  @@@ ",
				" @@@@@",
				" @@@@@",
				"   @  ",
				"   @  "
	};
	for (int i = 0; i < TREE_SIZE_Y; i++)
	{
		goto_xy(tree_x, CONSOLE_LINES - TREE_SIZE_Y - 1 + i);
		printf("%s\n", tree_img[i]);
	}
}

void draw_tall_tree(int tree_x)
{
	char tree_img[][TALL_TREE_SIZE_X] = {
		"  @",
		" @@@",
		" @@@",
		"@@@@@",
		"@@@@@",
		"@@@@@",
		"  @  ",
		"  @  ",
		"  @  "
	};
	for(int i=0;i<TALL_TREE_SIZE_Y;i++)
	{
		goto_xy(tree_x, CONSOLE_LINES - TALL_TREE_SIZE_Y - 1 + i);
		printf("%s\n", tree_img[i]);
	}
}

void draw_high_cloud(int cloud_x)
{
	char cloud_img[][CLOUD_SIZE_X] = {
		"  %%% %%",
		" %%%%%%%%",
		"  %%%%%"
	};
	for(int i=0;i<CLOUD_SIZE_Y;i++)
	{
		goto_xy(cloud_x, CONSOLE_LINES - CLOUD_SIZE_Y - 3 - DINO_SIZE_Y + i);
		printf("%s\n", cloud_img[i]);
	}
}

void draw_low_cloud(int cloud_x)
{
	char cloud_img[][CLOUD_SIZE_X] = {
		"  %%% %%",
		" %%%%%%%%",
		"  %%%%%"
	};
	for (int i = 0; i < CLOUD_SIZE_Y; i++)
	{
		goto_xy(cloud_x, CONSOLE_LINES - CLOUD_SIZE_Y + 1 - DINO_SIZE_Y + i);
		printf("%s\n", cloud_img[i]);
	}
}

void generate_hurdle()
{
	//tree
	if (Tree_x < 9)
	{
		srand(time(0));
		Tree_x = CONSOLE_COLS - 7;
		Tree_left = rand() % 15;
	}
	else if (Tree_left == 0)
	{
		Tree_x -= Speed;
		draw_tree(Tree_x);
	}
	else Tree_left--;

	//tall tree
	if (Tall_tree_x < 9)
	{
		srand(time(0));
		Tall_tree_x = CONSOLE_COLS - 7;
		Tall_tree_left = rand() % 33;
	}
	else if (Tall_tree_left == 0)
	{
		Tall_tree_x -= Speed;
		draw_tall_tree(Tall_tree_x);
	}
	else Tall_tree_left--;

	//high cloud
	if (High_cloud_x < 9)
	{
		srand(time(0));
		High_cloud_x = CONSOLE_COLS - 7;
		high_cloud_left = rand() % 97;
	}
	else if (high_cloud_left == 0)
	{
		High_cloud_x -= Speed;
		draw_high_cloud(High_cloud_x);
	}
	else high_cloud_left--;

	//low cloud
	if (Low_cloud_x < 9)
	{
		srand(time(0));
		Low_cloud_x = CONSOLE_COLS - 7;
		Low_cloud_left = rand() % 73;
	}
	else if (Low_cloud_left == 0)
	{
		Low_cloud_x -= Speed;
		draw_low_cloud(Low_cloud_x);
	}
	else Low_cloud_left--;
}

void draw_score(int score)
{
	goto_xy(1, 0);
	printf("SCORE : %d", score);
}

void show_scoreboard()
{
	cls;
	char scoreboard_image[][62] = {
		"-------------------------------------------------------------",
		"|                                                           |",
		"|   ###   ###  ###  ####  #### ####   ###   ##  ####  ###   |",
		"|  #     #    #   # #   # #    #   # #   # #  # #   # #  #  |",
		"|   ###  #    #   # ####  ###  ####  #   # #### ####  #  #  |",
		"|      # #    #   # # #   #    #   # #   # #  # # #   #  #  |",
		"|  ####   ###  ###  #  ## #### ####   ###  #  # #  ## ###   |",
		"|                                                           |",
		"|          |  Student ID  |   Name   |   Score   |          |",
		"|          ---------------------------------------          |",
		"|        1 |              |          |           |          |",
		"|        2 |              |          |           |          |",
		"|        3 |              |          |           |          |",
		"|        4 |              |          |           |          |",
		"|        5 |              |          |           |          |",
		"|        6 |              |          |           |          |",
		"|        7 |              |          |           |          |",
		"|        8 |              |          |           |          |",
		"|        9 |              |          |           |          |",
		"|       10 |              |          |           |          |",
		"|          ---------------------------------------          |",
		"|                                                           |",
		"|                 Press Any Key to Return                   |",
		"-------------------------------------------------------------"
	};

	for (int i = 0; i < 24; i++)
	{
		goto_xy(43, 3+i);
		printf("%s\n", scoreboard_image[i]);
	}

	for(int i=0;i<Score_data_cnt+1;i++)
	{
		goto_xy(60, 13 + i);
		printf("%d", Score_arr[i].id);
		/*
		goto_xy(72, 13 + i);
		printf("%s", Score_arr[i].name);
		*/
		int score_len = (int)(log10(Score_arr[i].score));
		goto_xy(81 + (11-score_len)/2, 13 + i);
		printf("%d", Score_arr[i].score);
	}
	while (1)
	{
		Sleep(50);
		if(_kbhit()) return;
	}
}

void game_over(int score)
{
	cls;
	char over_image[][63] = {
		"--------------------------------------------------------------",
		"|                                                            |",
		"|   ####   ###   #   #  #####     ###   #   #  #####  ####   |",
		"|  #      #   #  ## ##  #        #   #  #   #  #      #   #  |",
		"|  #  ##  #####  # # #  ####     #   #  #   #  ####   ####   |",
		"|  #   #  #   #  #   #  #        #   #   # #   #      # #    |",
		"|   ####  #   #  #   #  #####     ###     #    #####  #  ##  |",
		"|                                                            |",
		"|                                                            |",
		"|                                                            |",
		"|                   Student ID :                             |",
		"|                         Name :                             |",
		"|                                                            |",
		"--------------------------------------------------------------"
	};

	for(int i=0;i<14;i++)
	{
		goto_xy(44, 6 + i);
		printf("%s\n", over_image[i]);
	}
	
	int score_len = (int)log10(score);
	goto_xy((CONSOLE_COLS - score_len - 18) / 2, 14);
	printf("Your Score : %d", score);

	string student_id, name;
	goto_xy(77, 16);
	scanf("%d", &Score_arr[Score_data_cnt + 1].id);

	Score_arr[Score_data_cnt + 1].score = score;

	//qsort(Score_arr, Score_data_cnt + 1, sizeof(score_st), cmp);
	//save_score();
	show_scoreboard();
}

int start_menu()
{
	while (1)
	{
		cls;
		draw_ground();
		draw_dino(5, CONSOLE_LINES - DINO_SIZE_Y - 1, 0);
		draw_tree(50);
		draw_tall_tree(110);
		draw_high_cloud(130);
		char start_image[][60] = {
			"-----------------------------------------------------------",
			"|                                                         |",
			"|      #### #     ##   #  #       ####  # #   ##  #       |",
			"|      #    ###  #  #  #  #          # ## #  #  # #       |",
			"|      #    #    #  #  #  #####      #  # #  #  # #       |",
			"|      #### #     ##   #    #        #  # #   ##  #       |",
			"|                         #####                           |",
			"|                                               ###       |",
			"|                                               # #       |",
			"|                                               ###       |",
			"|                  Press SPACE To Start                   |",
			"|                s : ScoreBoard | x : exit                |",
			"|                                                         |",
			"-----------------------------------------------------------"
		};

		for (int i = 0; i < 14; i++)
		{
			goto_xy(45, 6 + i);
			printf("%s\n", start_image[i]);
		}
		goto_xy(60, CONSOLE_LINES - 10);
		printf("SPACE/UP : JUMP & DOWN : SLIDE");
		while (1)
		{
			if (_kbhit() != 0)
			{
				int key = _getch();
				if (key == 32)
				{
					game();
					game_over(Score);
					Score = 0;
					break;
				}
				else if (key == 's' || key == 'S')
				{
					show_scoreboard();
					break;
				}
				else if (key == 'x' || key == 'X')
				{
					return -1;
				}
			}
		}
	}
}

bool crash()
{
	//tree
	if ((Tree_x == 7 || Tree_x == 8) && Dino_y <= TREE_SIZE_Y - 2) return true;
	if (Tree_x >= 9 && Tree_x <= 16 && Dino_y <= TREE_SIZE_Y) return true;
	if (Tree_x > 16 && Tree_x <= 20 && Dino_y <= TREE_SIZE_Y - 2) return true;

	//tall tree
	if ((Tall_tree_x == 7 || Tall_tree_x == 8) && Dino_y <= TALL_TREE_SIZE_Y - 2) return true;
	if (Tall_tree_x >= 9 && Tall_tree_x <= 16 && Dino_y <= TALL_TREE_SIZE_Y) return true;
	if (Tall_tree_x > 16 && Tall_tree_x <= 20 && Dino_y <= TALL_TREE_SIZE_Y - 2) return true;

	//high cloud
	if ((High_cloud_x >= 16 && High_cloud_x <= 24) && Dino_y >= 4) return true;

	//low cloud
	if ((Low_cloud_x >= 16 && Low_cloud_x <= 24) && !Lying) return true;

	return false;
}

void game()
{
	int dino_stat = 0;
	Tree_x = CONSOLE_COLS - 5;
	while (true)
	{
		char key = 0;
		Score += 1;

		if (Score == 200) Speed = 4;
		if (Score == 600) Speed = 5;
		if (Score == 1200) Speed = 7;

		if (Jumping)
		{
			Dino_y += Dino_acceleration * Ms_per_tick / 1000;
			Dino_acceleration -= GRAVITY * Ms_per_tick / 1000;
			if (Dino_y < 1)
			{
				Dino_acceleration = 0;
				Jumping = false;
				Dino_y = 0;
			}
		}
		else if (GetAsyncKeyState(VK_DOWN))
		{
			Lying = true;
			dino_stat = 2;
		}
		else
		{
			Lying = false;
			if (dino_stat == 2) dino_stat = 0;
			else dino_stat = !dino_stat;
		}

		if (_kbhit() != 0)
		{
			key = _getch();
		}

		if (GetAsyncKeyState(VK_UP)) key = ' ';

		if (key == 32 && Jumping == false)
		{
			Jumping = true;
			Dino_acceleration = ACCELERATION;
		}

		cls;

		generate_hurdle();
		draw_ground();
		draw_score(Score);
		draw_dino(5, CONSOLE_LINES - DINO_SIZE_Y - 1 - (int)ceil(Dino_y), dino_stat);

		if (crash()) break;

		Sleep(Ms_per_tick);
	}
	return;
}

int main()
{
	for (int i = 0; i < CONSOLE_COLS - 2; i++) Ground[i] = '-';
	set_console();
	load_score();
	while (true)
	{
		if(start_menu() == -1) break;
	}
	return 0;
}