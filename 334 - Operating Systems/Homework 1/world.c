# include <fcntl.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

# include "message.h"
# include "logging.h"

#define CLIENT_FD 0
#define SERVER_FD 1
#define PIPE(fd) socketpair(AF_UNIX, SOCK_STREAM, PF_UNIX, fd)
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
enum ClientStatus
{
    ALIVE = 0,
    DEAD = 1,
};
typedef struct {
  coordinate pos;
char** argvs;
enum ClientStatus client_status;
pid_t pid;
int pipe_fd[2];
} ClientPlayer;
 
typedef struct {
  coordinate pos;
char* symbol;
char** argvs;
enum ClientStatus client_status;
pid_t pid;
int pipe_fd[2];
} ClientMonster;
int attacked[MONSTER_LIMIT];

 int GameRound;
int map_width;
int map_height;
char* x_positionOfTheDoor;
char* y_positionOfTheDoor;
int playerX;
int playerY;
int numberOfMonsters;
char** map;
char* strler;
ClientPlayer player;
ClientMonster* monsters;
int MonsterAttack;
int nfds;
coordinate* ordered;
int total;
bool game_over;
void ReadInputs()
{
    scanf("%d%d", &map_width, &map_height);
    //map = (char**)malloc(sizeof(char*) * map_height);
    /*for (int i = 0; i < map_height; i++) {
	   // map[i] = (char*)malloc(sizeof(char) * map_width);
	    memset(map[i], ' ', map_width);
	}*/

    scanf("%ms%ms", &x_positionOfTheDoor, &y_positionOfTheDoor);
    //map[x_positionOfTheDoor][y_positionOfTheDoor] ='+';

  
    // PLAYER 
    char* arg1;
    char* arg2;
    char* arg3;
    player.argvs = (char**)calloc(7, sizeof(char*));
    scanf("%d%d", &playerX, &playerY);
    //map[playerX][playerY] ='+';
    scanf("%ms", &strler);


    player.pos.x = playerX;//Player positonunu 2yere tanımladk hayırlısı..
    player.pos.y = playerY;
    player.client_status = ALIVE;
    scanf("%ms%ms%ms", &arg1, &arg2, &arg3);
    player.argvs[0] = strler;
    player.argvs[1] = x_positionOfTheDoor;
    player.argvs[2] = y_positionOfTheDoor;
    player.argvs[3] = arg1;
    player.argvs[4] = arg2;
    player.argvs[5] = arg3;
    player.argvs[6] = NULL;
    //printf("%s %s %s %s %s %s\n",strler,player.argvs[0],player.argvs[1],player.argvs[2],player.argvs[3],player.argvs[4]);	


    //# OF MONSTER
    scanf("%d", &numberOfMonsters);


    // MOSTERS
    monsters = (ClientMonster*)malloc(sizeof(ClientMonster) * numberOfMonsters);
    for (int i = 0; i < numberOfMonsters; ++i)
    {
        monsters[i].argvs = (char**)calloc(6, sizeof(char*));
        char* arg1;
        char* arg2;
        char* arg3;
        char* arg4;
        //COOOOL


        // s 3 5 5 5 1 2
        scanf("%ms%ms%d%d%ms%ms%ms%ms", &strler, &monsters[i].symbol, &monsters[i].pos.x, &monsters[i].pos.y, &arg1, &arg2,
            &arg3, &arg4);


        monsters[i].client_status = ALIVE;// bURAYA Bİ BAK

        monsters[i].argvs[0] = strler;
        monsters[i].argvs[1] = arg1;
        monsters[i].argvs[2] = arg2;
        monsters[i].argvs[3] = arg3;
        monsters[i].argvs[4] = arg4;
        monsters[i].argvs[5] = NULL;
        //Farklı monster adı olursa kısmına bak
    }
}
int CheckMovementForPlayer(int x , int y)
{
	int doorx = atoi(x_positionOfTheDoor);
    int doory = atoi(y_positionOfTheDoor);
	if(x > map_width || y > map_height)
    {return 0;}
    if (x == doorx && y == doory)
    {player.pos.x = x;player.pos.y=y; return 2;}
	for (int i = 0; i < total; ++i)
    {
        if (monsters[i].client_status == ALIVE)
        {
            if (x == monsters[i].pos.x && y == monsters[i].pos.y)
            {
                return 0;
            }
        }
    }
    return 1;

}

int CheckMovement(int x, int y)
{//out of the room boundries control etmemiz de gerekiyorü

    int doorx = atoi(x_positionOfTheDoor);
    int doory = atoi(y_positionOfTheDoor);
    if (x == player.pos.x && y == player.pos.y)
    {
        return 0;
    }
    if (x == doorx && y == doory)
    {
        return 0;
    }    
    //6 / 8
    if(x > map_width || y > map_height)
    	{return 0;}

    if (x == 0)
    {
        for (int i = 0; i < map_height; ++i)
        {
            if (y == i) { return 0; }
        }
    }
    if (y == 0)
    {
        for (int i = 0; i < map_height; ++i)
        {
            if (x == i) { return 0; }
        }
    }

    for (int i = 0; i < total; ++i)
    {
        if (monsters[i].client_status == ALIVE)
        {
            if (x == monsters[i].pos.x && y == monsters[i].pos.y)
            {
                return 0;
            }


        }
    }

    return 1;
}
void PrintMap()
{
    map_info mapping;

    mapping.map_width = map_width;
    mapping.map_height = map_height;
    coordinate door;
    int x = atoi(x_positionOfTheDoor);
    int y = atoi(y_positionOfTheDoor);
    //printf("%d   %s",x , x_positionOfTheDoor);

    door.x = x;
    door.y = y;
    mapping.door = door;

    coordinate _player;
    _player.x = player.pos.x;
    _player.y = player.pos.y;
    mapping.player = _player;



    mapping.alive_monster_count = total;
    coordinate monster_coordinates[MONSTER_LIMIT];
    char monster_types[MONSTER_LIMIT];
    for (int i = 0; i < total; ++i)
    {
        if (monsters[i].client_status == ALIVE)
        {
            char a = *monsters[i].symbol;
            monster_types[i] = a;
            //printf("Yaşayan Monsterlar => %c\n",a);
        }
        else
        {
            //monster_types[i]=NULL;
        }
    }
    for (int i = 0; i < total; ++i)
    {
        if (monsters[i].client_status == ALIVE)
        {
            monster_coordinates[i].x = monsters[i].pos.x;
            monster_coordinates[i].y = monsters[i].pos.y;
        }

    }

    for (int i = 0; i < total; ++i)
    {
        if (monsters[i].client_status == ALIVE)
        {
            mapping.monster_coordinates[i] = monster_coordinates[i];
            mapping.monster_types[i] = monster_types[i];
        }

    }


    print_map(&mapping);

}
    //free(monster_types);}
void SortTheCoordinates()
{


    total = 0;
    for (int i = 0; i < numberOfMonsters; ++i)
    {
        if (monsters[i].client_status == ALIVE)
            total++;
    }
  


    for (int i = 0; i < numberOfMonsters; ++i)
    {
    	for (int k = 0; k < numberOfMonsters - i - 1; ++k)
        {
            if (monsters[k].client_status == DEAD)
            {
                ClientMonster tempMonster;
                tempMonster = monsters[k];
                monsters[k] = monsters[k + 1];
                monsters[k + 1] = tempMonster;
               //	printf("%s\n", monsters[k+1].symbol);
            }
        }
    }
	

    for (int i = 0; i < total - 1; ++i)
    {
        for (int k = 0; k < total - i - 1; ++k)
        {
            if (monsters[k].pos.x > monsters[k + 1].pos.x)
            {
                ClientMonster tempMonster;
                tempMonster = monsters[k];
                monsters[k] = monsters[k + 1];
                monsters[k + 1] = tempMonster;
                

            }
            else if(monsters[k].pos.x == monsters[k + 1].pos.x)
            {
            	if (monsters[k].pos.y > monsters[k + 1].pos.y)
            	{
            		ClientMonster tempMonster;
	                tempMonster = monsters[k];
	                monsters[k] = monsters[k + 1];
	                monsters[k + 1] = tempMonster;
            	}
            }
        }
    }}
void InformMonsters(int idx)
{

    ClientMonster * const monster = monsters + idx;
    monster_message SM;
    SM.new_position.x = monster->pos.x;
    SM.new_position.y = monster->pos.y;
    SM.damage = attacked[idx];

    SM.player_coordinate.x = player.pos.x;
    SM.player_coordinate.y = player.pos.y;

    SM.game_over = game_over;


    write(monster->pipe_fd[SERVER_FD], &SM, sizeof(monster_message));
}
void Kill(ClientMonster* client)
{
    close(client->pipe_fd[SERVER_FD]);
    kill(client->pid, SIGTERM);
    waitpid(client->pid, NULL, 0);
    client->client_status = DEAD;
   // printf("Ölen Monster =>  %s\n",client->symbol );
}
void KillPlayer(ClientPlayer client)
{
    close(client.pipe_fd[SERVER_FD]);
    kill(client.pid, SIGTERM);
    waitpid(client.pid, NULL, 0);
    client.client_status = DEAD;
    //printf("Ölen Player =>  %s\n",client.symbol );
}
void InformPlayer()
{
    ClientPlayer const SinglePlayer = player;
    //The game world sends player_message struct to the player process and receives player_response struct in response.
    player_message SM;
    if (GameRound == 0)
    {
        total = numberOfMonsters;
    }
    else
    {
        SortTheCoordinates();
    }

    SM.total_damage = MonsterAttack;///SIRA BURDA
	SM.alive_monster_count = total;
    SM.new_position.x = player.pos.x;
    SM.new_position.y = player.pos.y;
    SM.game_over = game_over;

    for (int i = 0; i < total; ++i)
    {
        if (monsters[i].client_status == ALIVE)
        {
            SM.monster_coordinates[i] = monsters[i].pos;
            //printf("(%d , %d)\n", monsters[i].pos.x, monsters[i].pos.y);
        }

    }


    write(player.pipe_fd[SERVER_FD], &SM, sizeof(player_message));
}
void InitializeClients()
{
    for (int i = 0; i < numberOfMonsters; i++)
    {
        PIPE(monsters[i].pipe_fd);
        monsters[i].pid = fork();
        if (!monsters[i].pid)
        {
            dup2(monsters[i].pipe_fd[CLIENT_FD], STDIN_FILENO);
            dup2(monsters[i].pipe_fd[CLIENT_FD], STDOUT_FILENO);
            close(monsters[i].pipe_fd[SERVER_FD]);
            execv(monsters[i].argvs[0], monsters[i].argvs);
        }
        close(monsters[i].pipe_fd[CLIENT_FD]);
        nfds = max(nfds, monsters[i].pipe_fd[SERVER_FD] + 1);

    
        monster_response mr;
        read(monsters[i].pipe_fd[SERVER_FD], &mr, sizeof(monster_response));
        close(monsters[i].pipe_fd[CLIENT_FD]);
        

    }
    PIPE(player.pipe_fd);
    player.pid = fork();
    if (!player.pid)
    {
        dup2(player.pipe_fd[CLIENT_FD], STDIN_FILENO);
        dup2(player.pipe_fd[CLIENT_FD], STDOUT_FILENO);
        close(player.pipe_fd[SERVER_FD]);
        execv(player.argvs[0], player.argvs);// şu isimleri dinamik yapalım
    }
    close(player.pipe_fd[CLIENT_FD]);
    nfds = max(nfds, player.pipe_fd[SERVER_FD] + 1);

    player_response pr;   
    read(player.pipe_fd[SERVER_FD], &pr, sizeof(player_response));
   
}
void GameLoop()
{
    int p = 1;
    int remaining_monster = numberOfMonsters;
	fd_set read_fds;
	int k = 0;
	int playerCounter = 0;
	int playerKacis;
	if(player.argvs[5] != 0)
	 playerKacis = atoi(player.argvs[5]);

	while (1)
	{
		for (int i = 0; i < MONSTER_LIMIT; ++i)
		{
			attacked[i]=0;
		}
		playerCounter++;
		if(playerKacis != 0)
		{
			if(playerKacis == playerCounter)
			{
				game_over_status PlayerLeft;
				game_over = true;		
				PlayerLeft = go_left;
				
				PrintMap();
				print_game_over(PlayerLeft);
				break;
			}
		}
	    FD_ZERO(&read_fds);
	    for (int i = 0; i < total; ++i)
	    {
	        if (monsters[i].client_status == ALIVE)
	        {
	            FD_SET(monsters[i].pipe_fd[SERVER_FD], &read_fds);
	        }
	    }
	    if (player.client_status == ALIVE)
	    {
	        FD_SET(player.pipe_fd[SERVER_FD], &read_fds);
	    }	    

	    InformPlayer();	   

	    player_response pr;
	    read(player.pipe_fd[SERVER_FD], &pr, sizeof(player_response));

	    if (pr.pr_type == pr_attack)
	    {
	        //printf("Attack PR\n");
	        for (int i = 0; i < total; ++i)
	        {
	            attacked[i] = pr.pr_content.attacked[i];
	           // printf("%d\n", attacked[i]);
	        }
	    }
	    else if (pr.pr_type == pr_move)
	    {
	        //printf("Move PR\n");
	        int rejectOrAccept = CheckMovementForPlayer(pr.pr_content.move_to.x, pr.pr_content.move_to.y);
	       // printf("PR Wants to go : (%d , %d)\n",pr.pr_content.move_to.x,pr.pr_content.move_to.y);
	        if (rejectOrAccept == 1)
	        {
	            player.pos.x = pr.pr_content.move_to.x;
	            player.pos.y = pr.pr_content.move_to.y;
	        }
	        else if(rejectOrAccept == 2)
	        {
	        	game_over_status PlayerReachedToTheDoor;
				game_over = true;		
				PlayerReachedToTheDoor = go_reached;
				
				PrintMap();
				print_game_over(PlayerReachedToTheDoor);
				break;
	        }
	        
	    }
	    else if (pr.pr_type == pr_dead)
	    {
	        //printf("Dead PR\n");
	        game_over_status PlayerDies;
			game_over = true;		
			PlayerDies = go_died;
			
			PrintMap();
			print_game_over(PlayerDies);
			break;
	    }
	    else
	    {
	       // printf("Non-Ready PR");
	    }
	  

	    //Monsterlara tek tek mail atıyoruz sonra cevaplarını bekliyoruz
	    for (int i = 0; i < total; ++i)
	    {
	        //printf("Monster First For\n");
	        InformMonsters(i);
	    }
	    MonsterAttack = 0;
	    SortTheCoordinates();//hem x e hem y ye göre sıralayacuk
	    for (int i = 0; i < total; ++i)
	    {
	        //printf("Monster Second For\n");

	        monster_response mr;
	        read(monsters[i].pipe_fd[SERVER_FD], &mr, sizeof(mr));

	        if (mr.mr_type == mr_attack)
	        {
	            //printf("Attack MR\n");

	            MonsterAttack  += mr.mr_content.attack;
	            //printf("%dth monster attacks %d\n",i, MonsterAttack );
	        }
	        // ilk x sırası küçükten büyüğe eğer x ler eşitse y sırası küçükten büyüğe
	        else if (mr.mr_type == mr_move)
	        {
	            //printf("%dth MR Wants to go from (%d , %d) to (%d , %d) \n",i,
	            	///monsters[i].pos.x,monsters[i].pos.y,mr.mr_content.move_to.x,mr.mr_content.move_to.y);

	            int rejectOrAccept = CheckMovement(mr.mr_content.move_to.x, mr.mr_content.move_to.y);
	            if (rejectOrAccept == 1)
	            {
	                monsters[i].pos.x = mr.mr_content.move_to.x;
	                monsters[i].pos.y = mr.mr_content.move_to.y;
	            }
	        }
	        else if (mr.mr_type == mr_dead)
	        {
	            //printf("Dead MR -> %s\n",monsters[i].symbol);

	            Kill(monsters + i);
	            //printf("Dead MR -> %s\n",monsters[i+1].symbol);
	        }
	        else
	        {
	            //printf("Non-Ready");
	        }
	    }
	    SortTheCoordinates();
		PrintMap();

		if(total == 0)
		{
			game_over_status NoMonsterAlive;
			game_over = true;		
			NoMonsterAlive = go_survived;
			print_game_over(NoMonsterAlive);
			break;
		}
		GameRound++;
	 }}
void Finito(){
	KillPlayer(player);
	for (int i = 0; i < numberOfMonsters; ++i)
	{
		if(monsters[i].client_status == ALIVE)
		{
			Kill(monsters+i);
		}
	}
	
	free(monsters);
}
int main()
{
	GameRound=0;
    ReadInputs();
    InitializeClients();
    total = numberOfMonsters;
    //WaitForReadySignals()	
    PrintMap();	
    GameLoop();
    InformPlayer();
    for (int i = 0; i < total; ++i)
    {
    	if(monsters[i].client_status == ALIVE)
    		InformMonsters(i);
    }
    //SortTheCoordinates();
    Finito();
    return 0;
}