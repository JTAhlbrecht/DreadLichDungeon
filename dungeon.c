#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

void render();
void procInput();
void createMonsters();
void blocked(int y,int x);
void combat(int y,int x);
void charCreate();
void levelup();
void spellCast();
int los(int x1,int y1,int x2,int y2);
int diagDist(int x1,int y1,int x2,int y2);
void enemyAct();
void dstairs();
void ustairs();

int playerX,playerY,running=1,menu=0,mRes=60,hRes=100,lv=1;
char state='D';

char board[50][100];
int player[10]={1,0,10,0,0,0,0,0,0,1}; //{alive=1 dead=0,XP,nextLevel,HP,Str,Int,Dex,Con,MP,Level}


int Menemies[13][9]=
{
	{10,1,2,2,1,0,0,0,8}, //Goblin - HP,Alive?,Str,Dex,Int,X coord,Y coord,ID,XP Value
	{15,1,3,3,2,0,0,1,13}, //Orc - HP,Alive?,Str,Dex,Int,X coord,Y coord,ID, XP Value
	{10,1,1,5,1,0,0,2,10}, //Rat - HP,Alive?,Str,Dex,Int,X coord,Y coord,ID, XP Value
	{10,1,1,10,1,0,0,3,15}, //Bat - HP,Alive?,Str,Dex,Int,X coord,Y coord,ID, XP Value
	{10,1,1,3,3,0,0,4,10}, //Kobold - HP,Alive?,Str,Dex,Int,X coord,Y coord,ID, XP Value
	{25,1,8,5,2,0,0,5,23}, //Hobgoblin - HP,Alive?,Str,Dex,Int,X coord,Y coord,ID, XP Value
	{15,1,3,60,7,0,0,6,30}, //Wraith - HP,Alive?,Str,Dex,Int,X coord,Y coord,ID, XP Value
	{40,1,15,7,3,0,0,7,38}, //Troll - HP,Alive?,Str,Dex,Int,X coord,Y coord,ID, XP Value
	{50,1,5,2,2,0,0,8,17}, //Skeleton - HP,Alive?,Str,Dex,Int,X coord,Y coord,ID, XP Value
	{15,1,2,7,10,0,0,9,24}, //Mage - HP,Alive?,Str,Dex,Int,X coord,Y coord,ID, XP Value
	{50,1,5,10,15,95,43,10,50}, //Necromancer - HP,Alive?,Str,Dex,Int,X coord,Y coord,ID, XP Value
	{100,1,20,15,10,9,5,11,80}, //Demon - HP,Alive?,Str,Dex,Int,X coord,Y coord,ID, XP Value
	{200,1,10,20,30,16,5,12,145}, //Lich - HP,Alive?,Str,Dex,Int,X coord,Y coord,ID, XP Value
};

int enemies[110][9]={-1};

int main()
{
	FILE *fp;
	fp=fopen("test.txt","r");

	for(int i=0;i<50;i++)
	{
		for(int ii=0;ii<100;ii++)
		{
			fscanf(fp," %c",&board[i][ii]);
		}
	}

	srand(time(0));

	playerX=1;
	playerY=1;
	createMonsters();
	charCreate();

	system("stty raw -echo");
	system("clear");

	printf("After a long war the dread lich has retreated into its dungeon, guarded by its apprentice Necromancer and its summoned Demon.\n\rThe legendary heroes are injured and you, the trainee, must delve into the dungeon and defeat the Lich once and for all.\n\n\r*Space to continue*");
	char go='n';
	while(go!=' ') scanf("%c",&go);
	system("clear");

	while(player[0]==1&&enemies[2][1]==1)
	{
		render();

		switch(state)
		{
			case 'D':
			{
				procInput();
				if(player[0]==1) enemyAct();
			} break;

			case 'C':
			{
				char input;
				scanf(" %c",&input);
				if(input=='q' || input=='Q')
				{
					system("clear");
					state='D';
				}
				else
				{
					switch(input)
					{
						case 'c':
						case 'C': menu=0; break;
						case 'e':
						case 'E': menu=1; break;
						case 's':
						case 'S': menu=2; break;
					}
				}
			} break;
		}

		if(player[1]>=player[2])
		{
			system("stty  -raw echo");
			levelup();
			system("stty raw -echo");
		}

		if(player[8]<player[5]*2) mRes-=player[5];
		if(mRes<=0)
		{
			mRes=60;
			player[8]++;
		}

		if(player[3]<player[7]*3) hRes-=player[7];
		if(hRes<=0)
		{
			hRes=100;
			player[3]+=2;
		}
	}

	system("stty -raw echo");
	system("clear");

	if(player[0]==0) printf("\e[31;1mYOU ARE DEAD\e[0m\n");
	else
	{
		if(player[0]==2) printf("You place your hands upon The Ultimate Artifact of Ultimate Power. The air around you hums with an uncontrollable energy and there is a flash of light that blinds you.\n  When your vision returns, you realize that you have the power to do ANYTHING. Using your incredible ability, you usher in a new era of peace...or an era of tyranny. The choice is yours hero(?).\n\nSECRET ENDING\n");
		else if(enemies[2][1]==0) printf("With a final anguished cry, the Dread Lich evaporates into a dark miasma. Unfortunately, you were unable to find its phylactery, so it will eventually reform.\n  However, it will be much weaker and easier to deal with in the future. You return to the surface triumphant.\n\nGOOD ENDING - Try to find the secret!\n");
	}

	return 0;
}

void render()
{
	switch(state)
	{
		case 'D':
		{
			int minY,minX,maxY,maxX;
			if(playerY-10>=-1) minY=playerY-10; else minY=-1;
			if(playerX-10>=-1) minX=playerX-10; else minX=-1;
			if(minX+20<=101) maxX=minX+20;
			else
			{
				minX=81;
				maxX=101;
			}

			if(minY+20<=51) maxY=minY+20;
			else
			{
				minY=31;
				maxY=51;
			}

			for(int y=minY;y<maxY;y++)
			{
				for(int x=minX;x<maxX;x++)
				{
					if(los(x,y,playerX,playerY)==1)
					{
						if(x==playerX && y==playerY) printf("\e[35;1m@\e[0m");
						else if(y==-1||x==-1||x==100||y==50||board[y][x]=='#') printf("\e[32;1m#\e[0m");
						else if(board[y][x]=='!') printf("\e[33;1m%c\e[0m",board[y][x]);
						else if(board[y][x]=='['||board[y][x]==']') printf("\e[34;1m%c\e[0m",board[y][x]);
						else if(board[y][x]!='.') printf("\e[31;1m%c\e[0m",board[y][x]);
						else printf("%c",board[y][x]);
					} else printf(" ");
				}
				printf("\n\r");
			}
			printf("HP: %d MP: %d Str: %d Int: %d Dex: %d Con: %d\n\r",player[3],player[8],player[4],player[5],player[6],player[7]);
			printf("Level: %d XP: %d Next Level: %d\n\r",player[9],player[1],player[2]);
			printf("Coords: %d,%d\n\r",playerX,playerY);
			printf("Press ? for info\n\r\n\r");
		} break;

		case 'C':
		{
			system("clear");
			switch(menu)
			{
				case 0: //Commands
				{
					printf("W/A/S/D for movement\n\r");
					printf("C to cast a spell\n\r");
				} break;

				case 1: //Enemies/Objects
				{
					printf("\e[31;1mG\e[0m is a goblin. One of the weakest minions of the Lich and easily dispatched.\n\r");
					printf("\e[31;1mO\e[0m is an orc. Slightly tougher than goblins, they can threatening to beginning adventurers.\n\r");
					printf("\e[31;1mR\e[0m is a rat. Slightly faster than a goblin, but weaker.\n\r");
					printf("\e[31;1mB\e[0m is a bat. Very agile, but more of a pest than a threat.\n\r");
					printf("\e[31;1mK\e[0m is a kobold. Weaker than a goblin, but faster and smarter.\n\r");
					printf("\e[31;1mH\e[0m is a hobgoblin. Very strong and robust. Approach with caution.\n\r");
					printf("\e[31;1mW\e[0m is a wraith. Tortured soul enslaved by the Lich. Immaterial and difficult to hit physcially, its shrieking cry will damage your very soul and likely critically hit. Try magic from a distance!\n\r");
					printf("\e[31;1mT\e[0m is a troll. Incredibly powerful and can take a beating, but stupid. Avoid melee combat.\n\r");
					printf("\e[31;1mS\e[0m is a skeleton. Granted incredible durability by the magics holding them together, but not terribly fast or smart.\n\r");
					printf("\e[31;1mM\e[0m is a mage. These spellcasters will fling lightning at any target within 5 squares of them. Be careful!\n\n\r");
					printf("\e[31;1mN\e[0m is the Necromancer. The Lich's apprentice guards the stairs leading to his master's lair. Will conjure powerful magic to ward off any intruders. His dark wards give him high resilience.\n\r");
					printf("\e[31;1mD\e[0m is the Demon. A powerful demon summoned by the Lich that guards the door to its lair. Overwhelmingly powerful, very fast, and capable of using magic. This dark beast will not be defeated easily, so come prepared.\n\r");
					printf("\e[31;1mL\e[0m is the Dread Lich. An evil being that has existed since the dawn of time. Will cast terrible magic to destroy its enemies and has incredibly powerful magical armor to keep it tethered to this realm. Good luck to any who face it.\n\n\r");

					printf("\e[34;1m[\e[0m is a staircase leading up. Use these to return to a previously visited floor of the Lich's dungeon.\n\r");
					printf("\e[34;1m]\e[0m is a staircase leading down. Use these to descend further into the Lich's dungeon.\n\r");
					printf("\e[32;1m#\e[0m is a wall. These block your sight and prevent you from moving through them.\n\n\r");

					printf("\e[33;1m!\e[0m is The Ultimate Artifact of Ultimate Power. The Lich has been pouring power into this for years, and nearly used it to conquer the world.\n\r  It was stopped shortly before it could use the artifact when the heroes defeated it and forced it to retreat. In its weakened state, the Lich cannot use the Artifact, and has likely hidden it nearby itself.\n\r  If the artifact could be found, it would give the wielder near limitless power.\n\n\r");
				} break;

				case 2: //Spells
				{
					printf("1) Lightning Bolt: Damages a distant enemy based on Int.\n\r");
					printf("2) Heal: Heals you for a minimum of 5 health, but will restore more based on Int.\n\r");
				} break;
			}

			printf("\n\rq to return to game\n\r");
			printf("e for enemy/object list\n\r");
			printf("s for spell list\n\r");
			printf("c for command list\n\r");
		} break;
	}
}

void charCreate()
{
	int points=16,str=1,dex=1,Int=1,con=1,choice=90;
	char proc='q';

	for(int i=0;i<3;i++)
	{
		system("clear");
		printf("Str: %d Dex: %d Int: %d Con: %d\n\r",str,dex,Int,con);
		printf("Points remaining: %d\n\r",points);

		switch(i)
		{
			case 0: printf("Please enter amount of points to add to Str: "); break;
			case 1: printf("Please enter amount of points to add to Dex: "); break;
			case 2: printf("Please enter amount of points to add to Int: "); break;
		}

		while(choice>points||choice<0) scanf("%d",&choice);
		switch(i)
		{
			case 0: str+=choice; break;
			case 1: dex+=choice; break;
			case 2: Int+=choice; break;
		}
		points-=choice;
		choice=90;
	}
	con+=points;

	system("clear");
	printf("Str: %d Dex: %d Int: %d Con: %d\n\r",str,dex,Int,con);
	printf("Do you want to proceed with these stats?\n\r",points);
	while(proc!='y'&&proc!='Y'&&proc!='n'&&proc!='N') scanf(" %c",&proc);

	switch(proc)
	{
		case 'y':
		case 'Y':
		{
			player[3]=con*3;
			player[4]=str;
			player[5]=Int;
			player[6]=dex;
			player[7]=con;
			player[8]=Int*2;
		} break;

		case 'n':
		case 'N': charCreate(); break;
	}
}

void levelup()
{
	int points=4,str=player[4],Int=player[5],dex=player[6],con=player[7],choice=90;
	char proc='q';

	for(int i=0;i<3;i++)
	{
		system("clear");
		printf("LEVEL UP! Welcome to level %d!\n\r",player[9]+1);
		printf("Str: %d Dex: %d Int: %d Con: %d\n\r",str,dex,Int,con);
		printf("Points remaining: %d\n\r",points);

		switch(i)
		{
			case 0: printf("Please enter amount of points to add to Str: "); break;
			case 1: printf("Please enter amount of points to add to Dex: "); break;
			case 2: printf("Please enter amount of points to add to Int: "); break;
		}

		while(choice>points||choice<0) scanf("%d",&choice);
		switch(i)
		{
			case 0: str+=choice; break;
			case 1: dex+=choice; break;
			case 2: Int+=choice; break;
		}
		points-=choice;
		choice=90;
	}
	con+=points;

	system("clear");
	printf("LEVEL UP!\n\r");
	printf("Str: %d Dex: %d Int: %d Con: %d\n\r",str,dex,Int,con);
	printf("Do you want to proceed with these stats?\n\r",points);
	while(proc!='y'&&proc!='Y'&&proc!='n'&&proc!='N') scanf(" %c",&proc);

	switch(proc)
	{
		case 'y':
		case 'Y':
		{
			player[3]=con*3;
			player[4]=str;
			player[5]=Int;
			player[6]=dex;
			player[7]=con;
			player[8]=Int*2;
			player[9]++;
			if(player[2]*.3<=20) player[2]+=20; else player[2]=player[2]+player[2]*.3;
			system("clear");

			if(player[1]>=player[2]) levelup();
		} break;

		case 'n':
		case 'N': levelup(); break;
	}
}

void procInput()
{
	char dir;
	scanf(" %c",&dir);

	switch(dir)
	{
		case 'w':
		case 'W':
		{
			if(playerY>0 && board[playerY-1][playerX]=='.')
			{
				playerY--;
				system("clear");
			}
			else if(playerY-1<0)
			{
				system("clear");
				break;
			}
			else  blocked(playerY-1,playerX);
		} break;

		case 's':
		case 'S':
		{
			if(playerY<49 && board[playerY+1][playerX]=='.')
			{
				playerY++;
				system("clear");
			}
			else if(playerY+1>49)
			{
				system("clear");
				break;
			}
			else blocked(playerY+1,playerX);
		} break;

		case 'a':
		case 'A':
		{
			if(playerX>0 && board[playerY][playerX-1]=='.')
			{
				playerX--;
				system("clear");
			}
			else if(playerX-1<0)
			{
				system("clear");
				break;
			}
			else blocked(playerY,playerX-1);
		} break;

		case 'd':
		case 'D':
		{
			if(playerX<99 && board[playerY][playerX+1]=='.')
			{
				playerX++;
				system("clear");
			}
			else if(playerX+1>99)
			{
				system("clear");
				break;
			}
			else blocked(playerY,playerX+1);
		} break;

		case 'q':
		case 'Q': player[0]=-1; break;

		case '/':
		case '?':
		{
			menu=0;
			state='C';
			system("clear");
		} break;

		case 'c':
		case 'C': spellCast(); break;

		default: break;
	}
}

void blocked(int y,int x)
{
	switch(board[y][x])
	{
		case '#':
		{
			if(lv==2&&playerX==9&&playerY==9)
			{
				board[9][10]='.';
				system("clear");
				printf("\e[33;1mSecret Found!\e[0m\n\r");
			}
			else system("clear");
		} break;

		case '!': player[0]=2; break;

		case ']':
		{
			dstairs();
			system("clear");
		} break;

		case '[':
		{
			ustairs();
			system("clear");
		} break;

		default:
		{
			system("clear");
			combat(y,x);
		}
	}
}

void combat(int y,int x)
{
	int tar,dam,hit;
	char name[20];

	for(int i=0;i<110;i++)
	{
		if(enemies[i][5]==x&&enemies[i][6]==y)
		{
			tar=i;
			i=111;
		}
	}

	switch(enemies[tar][7])
	{
		case 0: strcpy(name,"Goblin"); break;
		case 1: strcpy(name,"Orc"); break;
		case 2: strcpy(name,"Rat"); break;
		case 3: strcpy(name,"Bat"); break;
		case 4: strcpy(name,"Kobold"); break;
		case 5: strcpy(name,"Hobgoblin"); break;
		case 6: strcpy(name,"Wraith"); break;
		case 7: strcpy(name,"Troll"); break;
		case 8: strcpy(name,"Skeleton"); break;
		case 9: strcpy(name,"Mage"); break;
		case 10: strcpy(name,"Necromancer"); break;
		case 11: strcpy(name,"Demon"); break;
		case 12: strcpy(name,"Lich"); break;
	}

	hit=rand()%100+1;
	if(hit>(enemies[tar][3]-player[6])*2)
	{
		dam=rand()%player[4]+player[4]/2;
		if(dam==0) dam=1;
		if(rand()%100+1<=player[6])
		{
			printf("\e[31;1m*Critical Hit!*\e[0m ");
			dam=dam*2;
		}
		enemies[tar][0]-=dam;
		printf("You deal %d damage to %s.\n\r",dam,name);
	}
	else printf("You miss %s.\n\r",name);

	if(enemies[tar][0]<=0)
	{
		player[1]+=enemies[tar][8];
		enemies[tar][1]=0;
		board[enemies[tar][6]][enemies[tar][5]]='.';
		printf("%s is slain.\n\r",name);
	}
	else
	{
		hit=rand()%100+1;
		if(hit>(player[6]-enemies[tar][3])*2)
		{
			dam=rand()%enemies[tar][2]+enemies[tar][2]/2;
			if(dam==0) dam=1;
			if(rand()%100+1<=enemies[tar][3])
			{
				printf("\e[31;1m*Critical Hit!*\e[0m ");
				dam=dam*2;
			}
			player[3]-=dam;
			printf("%s deals %d damage to you.\n\r",name,dam);
			if(player[3]<=0) player[0]=0;
		}
		else printf("%s misses.\n\r",name);
	}
}

void createMonsters()
{
	int num_mon=rand()%45+60;
	int x,y,p,r;
	char mon;

	for(int i=0;i<3;i++)
	{
		enemies[i][0]=Menemies[i+10][0];
		enemies[i][1]=Menemies[i+10][1];
		enemies[i][2]=Menemies[i+10][2];
		enemies[i][3]=Menemies[i+10][3];
		enemies[i][4]=Menemies[i+10][4];
		enemies[i][5]=Menemies[i+10][5];
		enemies[i][6]=Menemies[i+10][6];
		enemies[i][7]=Menemies[i+10][7];
		enemies[i][8]=Menemies[i+10][8];
		enemies[i][9]=Menemies[i+10][9];
	}

	board[43][95]='N';

	for(int i=3;i<num_mon+3;i++)
	{
		x=rand()%99;
		y=rand()%49;

		while(board[y][x]!='.'||(x==1&&y==1))
		{
			x=rand()%99;
			y=rand()%49;
		}

		switch(rand()%10)
		{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6: mon='c'; break;
			case 7:
			case 8:
			case 9: mon='u'; break;
		}

		switch(mon)
		{
			case 'c':
			{
				r=rand()%8;
				switch(r)
				{
					case 0:
					case 1: mon='R'; break;
					case 2:
					case 3: mon='B'; break;
					case 4: mon='G'; break;
					case 5: mon='O'; break;
					case 6: mon='K'; break;
					case 7: mon='S'; break;
				}
			} break;

			case 'u':
			{
				r=rand()%4;
				switch(r)
				{
					case 0: mon='H'; break;
					case 1: mon='W'; break;
					case 2: mon='T'; break;
					case 3: mon='M'; break;
				}
			} break;
		}

		board[y][x]=mon;

		switch(mon)
		{
			case 'G': p=0; break;
			case 'O': p=1; break;
			case 'R': p=2; break;
			case 'B': p=3; break;
			case 'K': p=4; break;
			case 'H': p=5; break;
			case 'W': p=6; break;
			case 'T': p=7; break;
			case 'S': p=8; break;
			case 'M': p=9; break;
		}

		for(int ii=0;ii<9;ii++)
		{
			if(ii==5) enemies[i][ii]=x;
			else if(ii==6) enemies[i][ii]=y;
			else enemies[i][ii]=Menemies[p][ii];
		}
	}
}

void spellCast()
{
	int s=0,n,x,y;
	system("stty -raw echo");
	printf("Cast which spell? 1 or 2? 3 to cancel.\n");
	while(s<1 || s>3) scanf("%d",&s);

	switch(s)
	{
		case 1:
		{
			n=-1;
			printf("Target X coord?\n");
			while(n<0 || n>99) scanf("%d",&n);
			x=n;
			n=-1;

			printf("Target Y coord?\n");
			while(n<0 || n>49) scanf("%d",&n);
			y=n;
			system("stty raw -echo");

			if(los(x,y,playerX,playerY)==1&&board[y][x]!='#')
			{
				if(player[8]>=5)
				{
					player[8]-=5;
					system("clear");
					if(board[y][x]!='.')
					{
						int dam=0;
						for(int i=0;i<110;i++)
						{
							if(enemies[i][7]==-1) break;
							if(enemies[i][5]==x&&enemies[i][6]==y)
							{
								char name[20];
								switch(enemies[i][7])
								{
									case 0: strcpy(name,"Goblin"); break;
									case 1: strcpy(name,"Orc"); break;
									case 2: strcpy(name,"Rat"); break;
									case 3: strcpy(name,"Bat"); break;
									case 4: strcpy(name,"Kobold"); break;
									case 5: strcpy(name,"Hobgoblin"); break;
									case 6: strcpy(name,"Wraith"); break;
									case 7: strcpy(name,"Troll"); break;
									case 8: strcpy(name,"Skeleton"); break;
									case 9: strcpy(name,"Mage"); break;
									case 10: strcpy(name,"Necromancer"); break;
									case 11: strcpy(name,"Demon"); break;
									case 12: strcpy(name,"Lich"); break;
								}

								dam=rand()%player[5]+player[5]/2;
								dam-=enemies[i][4];
								if(dam<=0) dam=1;
								enemies[i][0]-=dam;
								printf("You zap %s for %d damage.\n\r",name,dam);

								if(enemies[i][0]<=0)
								{
									player[1]+=enemies[i][8];
									board[enemies[i][6]][enemies[i][5]]='.';
									printf("%s is slain.\n\r",name);
								}
								break;
							}
						}
					}
					else printf("Missed!\n\r");
				}
				else
				{
					system("clear");
					printf("No MP\n\r");
				}
			}
			else printf("Invalid coordinates");
		} break;

		case 2:
		{
			system("stty raw -echo");
			if(player[3]<player[7]*3)
			{
				if(player[8]>=8)
				{
					int h=0;
					h=5+rand()%player[5];
					if(h+player[3]>player[7]*3) h=player[7]*3-player[3];
					system("clear");
					printf("You heal %d health.\n\r",h);
					player[3]+=h;
					player[8]-=8;
				}
				else
				{
					system("clear");
					printf("No MP\n\r");
				}
			}
			else
			{
				system("clear");
				printf("Not damaged\n\r");
			}
		} break;

		case 3:
		{
			system("stty raw -echo");
			system("clear");
			render();
			procInput();
		} break;
	}
}

int los(int x1,int y1, int x2,int y2)
{
	int points[200][2]={-1};

	int N=diagDist(x1,y1,x2,y2);
	for(int i=0;i<=N;i++)
	{
		float t=N==0? 0:i/(float)N;

		int pX=x1+t*(x2-x1);
		int pY=y1+t*(y2-y1);
		points[i][1]=pX;
		points[i][0]=pY;
	}

	for(int i=0;i<100;i++)
	{
		if(points[i][0]==-1) break;
		else if(i==0&&board[points[i][0]][points[i][1]]=='#') continue;
		else if(board[points[i][0]][points[i][1]]=='#') return 0;
	}
	return 1;
}

int diagDist(int x1,int y1,int x2,int y2)
{
	int dx=abs(x2-x1);
	int dy=abs(y2-y1);
	if(dx>dy) return dx; else return dy;
}

void enemyAct()
{
	for(int i=0;i<110;i++)
	{
		if(enemies[i][0]==-1) break;
		if(los(enemies[i][5],enemies[i][6],playerX,playerY)==1&&enemies[i][7]>=9&&enemies[i][1]==1)
		{
			if(diagDist(enemies[i][5],enemies[i][6],playerX,playerY)<=5&&diagDist(enemies[i][5],enemies[i][6],playerX,playerY)>1)
			{
				char name[20];
				int dam=0;
				switch(enemies[i][7])
				{
					case 9: strcpy(name,"Mage"); break;
					case 10: strcpy(name,"Necromancer"); break;
					case 11: strcpy(name,"Demon"); break;
					case 12: strcpy(name,"Lich"); break;
				}

				if((i<3 && lv!=2)) continue;
				if(i>3&&lv>1) break;;
				dam=rand()%enemies[i][4]+enemies[i][4]/2;
				dam-=player[5];
				if(dam<=0) dam=1;
				player[3]-=dam;
				printf("%s located at %d,%d zaps you for %d damage.\n\r",name,enemies[i][5],enemies[i][6],dam);
				if(player[3]<=0) player[0]=0;
			}
		}
	}
}

void dstairs()
{
	lv++;

	for(int i=0;i<50;i++)
	{
		for(int ii=0;ii<100;ii++)
		{
			board[i][ii]=' ';
		}
	}

	FILE *fp;
	switch(lv)
	{
		case 2: fp=fopen("test2.txt","r"); break;
		case 3: fp=fopen("test3.txt","r"); break;
	}

	for(int i=0;i<12;i++)
	{
		for(int ii=0;ii<19;ii++)
		{
			fscanf(fp," %c",&board[i][ii]);
		}
	}

	if(lv==2)
	{
		if(enemies[1][1]==1) board[5][9]='D';
		board[5][16]='L';
	}

	playerX=2;
	playerY=2;
}

void ustairs()
{
	for(int i=0;i<50;i++)
	{
		for(int ii=0;ii<100;ii++)
		{
			board[i][ii]=' ';
		}
	}

	FILE *fp;
	switch(lv)
	{
		case 2: fp=fopen("test.txt","r"); break;
		case 3: fp=fopen("test2.txt","r"); break;
	}

	switch(lv)
	{
		case 3:
		{
			for(int i=0;i<12;i++)
			{
				for(int ii=0;ii<19;ii++)
				{
					fscanf(fp," %c",&board[i][ii]);
				}
			}

			for(int i=1;i<3;i++)
			{
				if(enemies[i][1]==1)
				{
					if(i==1)board[5][9]='D'; else board[5][16]='L';
				}
			}

			playerX=14;
			playerY=9;
			lv--;
		} break;

		case 2:
		{
			for(int i=0;i<50;i++)
			{
				for(int ii=0;ii<100;ii++)
				{
					fscanf(fp," %c",&board[i][ii]);
				}
			}

			for(int i=3;i<110;i++)
			{
				if(enemies[i][1]==1)
				{
					switch(enemies[i][7])
					{
						case 0: board[enemies[i][6]][enemies[i][5]]='G'; break;
						case 1: board[enemies[i][6]][enemies[i][5]]='O'; break;
						case 2: board[enemies[i][6]][enemies[i][5]]='R'; break;
						case 3: board[enemies[i][6]][enemies[i][5]]='B'; break;
						case 4: board[enemies[i][6]][enemies[i][5]]='K'; break;
						case 5: board[enemies[i][6]][enemies[i][5]]='H'; break;
						case 6: board[enemies[i][6]][enemies[i][5]]='W'; break;
						case 7: board[enemies[i][6]][enemies[i][5]]='T'; break;
						case 8: board[enemies[i][6]][enemies[i][5]]='S'; break;
						case 9: board[enemies[i][6]][enemies[i][5]]='M'; break;
					}

				}
			}

			if(board[46][96]=='.')
			{
				playerX=96;
				playerY=45;
			}
			else if(board[46][94]=='.')
			{
				playerX=94;
				playerY=45;
			}
			else playerX=95; playerY=45;

			lv--;
		} break;
	}
}