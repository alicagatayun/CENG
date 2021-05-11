#include <stdio.h>
#include <unistd.h>
#include "message.h"
#include <stdlib.h>
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

int health;
int damage;
int defence;
int range;

int mhDistance(coordinate a, coordinate b) {
  return abs(a.x - b.x) + abs(a.y - b.y);
}

int main(int argc , char*argv[])
{
  int a,b,c,d;
   
   health 	= atoi(argv[1]);
   damage 	= atoi(argv[2]);
   defence 	= atoi(argv[3]);
   range 	= atoi(argv[4]);
  monster_response mr;//servere giden mesajjj
  monster_message sm;//serverden gelen mesajjj
  int i =1;
   int n;
  while(1) 
  {
 	
 	if(i > 1)
 	{
 		read(STDIN_FILENO, &sm, sizeof(monster_message));
 		if(sm.game_over == false)
	    {
	    	health = health - max(0,sm.damage-defence);

	    	if(health <= 0)
	    	{
	    		mr.mr_type = mr_dead;
	    		write(STDOUT_FILENO, &mr, sizeof(mr));
	    		//Ben öldüm ben bittim
	    	}
	    	else
	    	{
	    		int min_distance = mhDistance(sm.new_position, sm.player_coordinate);
				if(range >= min_distance)
				{
					mr.mr_type = mr_attack;
					mr.mr_content.attack = damage;
					write(STDOUT_FILENO, &mr, sizeof(mr));
					
				}
				else
				{
					//Means get closer to the player
					mr.mr_type = mr_move;
					coordinate tempCoordinate= sm.new_position;
					coordinate bestCoordinate;
					coordinate monsterCurrentCoordinate;
					monsterCurrentCoordinate = sm.new_position;
					int local_min_distance;
					int best_distance =99;	

					//For the UP
					tempCoordinate.y +=1;
					local_min_distance= mhDistance(tempCoordinate, sm.player_coordinate);
					if(local_min_distance < best_distance)
					{
						bestCoordinate.x = monsterCurrentCoordinate.x;
						bestCoordinate.y = monsterCurrentCoordinate.y + 1;
						best_distance = local_min_distance;
					}

					//For the upper right
					tempCoordinate.x +=1;
					local_min_distance= mhDistance(tempCoordinate, sm.player_coordinate);
					
					if(local_min_distance < best_distance)
					{
						bestCoordinate.x = monsterCurrentCoordinate.x + 1;
						bestCoordinate.y = monsterCurrentCoordinate.y + 1;
						best_distance = local_min_distance;
					}
					//Right
					tempCoordinate = sm.new_position;
					tempCoordinate.x +=1;
					local_min_distance= mhDistance(tempCoordinate, sm.player_coordinate);
					
					if(local_min_distance < best_distance)
					{
						bestCoordinate.x = monsterCurrentCoordinate.x + 1;
						bestCoordinate.y = monsterCurrentCoordinate.y;
						best_distance = local_min_distance;
					}

					//bottomright

					tempCoordinate = sm.new_position;
					tempCoordinate.x +=1;
					tempCoordinate.y -=1;
					local_min_distance= mhDistance(tempCoordinate, sm.player_coordinate);
					
					if(local_min_distance < best_distance)
					{
						bestCoordinate.x = monsterCurrentCoordinate.x + 1;
						bestCoordinate.y = monsterCurrentCoordinate.y - 1;
						best_distance = local_min_distance;
					}
					//,down,bottom-left,left,upper-left

					tempCoordinate = sm.new_position;					
					tempCoordinate.y -=1;
					local_min_distance= mhDistance(tempCoordinate, sm.player_coordinate);
					
					if(local_min_distance < best_distance)
					{
						bestCoordinate.x = monsterCurrentCoordinate.x ;
						bestCoordinate.y = monsterCurrentCoordinate.y - 1;
						best_distance = local_min_distance;
					}

					//bottom-left
					tempCoordinate = sm.new_position;					
					tempCoordinate.y -=1;
					tempCoordinate.x -=1;
					local_min_distance= mhDistance(tempCoordinate, sm.player_coordinate);
					
					if(local_min_distance < best_distance)
					{
						bestCoordinate.x = monsterCurrentCoordinate.x - 1;
						bestCoordinate.y = monsterCurrentCoordinate.y - 1;
						best_distance = local_min_distance;
					}

					//left
					tempCoordinate = sm.new_position;					
					
					tempCoordinate.x -=1;
					local_min_distance= mhDistance(tempCoordinate, sm.player_coordinate);
					
					if(local_min_distance < best_distance)
					{
						bestCoordinate.x = monsterCurrentCoordinate.x - 1;
						bestCoordinate.y = monsterCurrentCoordinate.y;
						best_distance = local_min_distance;
					}

					//upper-left
					tempCoordinate = sm.new_position;					
					
					tempCoordinate.x -=1;
					tempCoordinate.y +=1;
					local_min_distance= mhDistance(tempCoordinate, sm.player_coordinate);
					
					if(local_min_distance < best_distance)
					{
						bestCoordinate.x = monsterCurrentCoordinate.x - 1;
						bestCoordinate.y = monsterCurrentCoordinate.y + 1;
						best_distance = local_min_distance;
					}

					mr.mr_content.move_to = bestCoordinate;
					write(STDOUT_FILENO, &mr, sizeof(monster_response));
				}



	    	}
	    	//Monster's aim is getting closer to the player and attack it
	    	
	    	//FillMessage(&mr, sm.pos, move_count, possible_moves);
	    }
	    else
	    {
	   		//Oyun bitti----
	    }
 	}
 	else
 	{
		mr.mr_type = mr_ready;//mr_content may be any value but initializing to 0 is advised
  		//It is only sent before receiving any message from the game world.  		
  		write(STDOUT_FILENO, &mr, sizeof(mr)); //Send Ready Response
  		
 	}
  	i++;
	    
	    
  }
  
  //usleep(10000*(1+rand()%9));
  return 0;
}