#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

// Function declarators
void create_filename(char *, const char **, int, char **);
void create_room(char **, const char **, int, int, int);
void connect_rooms(char **, const char **,int **, int, int);
void get_user_choice(char *, char **, char *);
void play_game(char *, const char *, const char *);
void delete_connections(int **);
int ** allocate_and_init_connections();
void delete_filename_pointers(char **, int);
char ** create_filename_pointers(int);
void choose_random_rooms(const char **, const char **, int[], int, int);
void startup(char *, const char **, char **, int **, int *);

/******************************************************************************
 * void startup(char *, const char *, char **, int ** int *)
 *
 * function that sets up the game directory for use
 *****************************************************************************/
void startup(char * directoryname, const char ** chosen_roomnames, char ** filenames, int ** connections, int * start_end){
	// safely create a directory if it doesnt exits
	struct stat st = {0};
	// set up the directory name as mankesg.rooms.pid
	memset(directoryname, 0, sizeof(directoryname));
	strcpy(directoryname, "mankesg.rooms.");
	char * end = directoryname + strlen(directoryname);
	end += sprintf(end, "%ld", (long)getpid());
	if (stat(directoryname, &st) == -1) {
		// create this directory if it doesn't exist
		mkdir(directoryname, 0700);
	}
	// for each room in the chosen roomnames, create a corresponding filename (mankesg.rooms.pid/GONNA)
	int i = 0;
	for(;i < 7; i++){
		create_filename(directoryname, chosen_roomnames, i, filenames);
	}
	// randomly assign a start and end room
	srand((unsigned)time(NULL));
	int start_room = rand() % 7;
	int end_room = rand() % 7;
	start_end[0] = start_room;
	// make sure that they aren't the same room
	while (start_room == end_room){
		end_room = rand() % 7;
	}
	start_end[1] = end_room;
	// for each file name, create a corresponding file
	i = 0;
	for(; i < 7; i++){
		create_room(filenames, chosen_roomnames, i, start_room, end_room);
	}
	// connect the rooms together and give them contents
	connect_rooms(filenames, chosen_roomnames, connections, start_room, end_room);
}

/******************************************************************************
 * void connect_rooms(char **, const char **,int **, int, int)
 *
 * Connects the rooms together and gives them their contents.
 *****************************************************************************/
void connect_rooms(char ** filenames, const char ** chosen_roomnames, int ** connections,  int start_room, int end_room){
	// seed the random number generator
	srand((unsigned)time(NULL));
	// for each room, randomly assign a random number of connections
	int i = 0;
	for(; i < 7; i++){
		int num_connections = (rand() % 4) + 3;
		int new_connection;
		while(num_connections > 0){
			// while we have more connections to go
			new_connection = rand() % 7;
			// generate a new connection. if this conneciton does not exist
			if ((new_connection != i) &&
				(connections[i][new_connection] == 0)){
				// decrement the number of connections and assign the connection to
				// the current file and the connection file
				num_connections--;
				connections[i][new_connection] = 1;
				connections[new_connection][i] = 2;
			}// if the connection does exist, we count this connection and decrement num connections
			else if((new_connection != i)
					&& (connections[i][new_connection] == 2)){
				connections[i][new_connection] = 1;
				num_connections--;
			}
		}
	}
	// once we have assigned connections, we print them to the contents of the file
	// for each file
	i = 0;
	for (; i < 7; i++){
		FILE * fp;
		fp = fopen(filenames[i], "a+");
		int k = 0;
		int j = 0;
		for (; j < 7; j++){
			if (connections[i][j] != 0){
				fprintf(fp, "CONNECTION %d: %s\n",k+1, chosen_roomnames[j]);
				k++;
			}
		}
		if(i == start_room){
			fprintf(fp, "ROOM TYPE: START_ROOM\n");
		}
		else if (i == end_room){
			fprintf(fp, "ROOM TYPE: END_ROOM\n");			
		}
		else{
			fprintf(fp, "ROOM TYPE: MID_ROOM\n");
		}
		fclose(fp);
	}
}

/******************************************************************************
 *
 *
 *
 *****************************************************************************/
void create_filename(char * directoryname, const char ** chosen_roomnames, int i, char ** filenames){
	memset(filenames[i], 0, sizeof(filenames[i]));
	strcpy(filenames[i], directoryname);
	char * end = filenames[i] + strlen(filenames[i]);
	end += sprintf(end, "/");
	end += sprintf(end, "%s", chosen_roomnames[i]);
	//printf("%s\n", filenames[i]);
}

/******************************************************************************
 *
 *
 *
 *****************************************************************************/
void create_room(char ** filenames, const char ** chosen_roomnames, int i, int start_room, int end_room){
	FILE * fp;
	fp = fopen(filenames[i], "wb");
	if (!fp){
		printf("File pointer invalid\n");
	}
	fprintf(fp, "ROOM NAME: %s\n", chosen_roomnames[i]);
	fclose(fp);
}

/******************************************************************************
 *
 *
 *
 *****************************************************************************/
void choose_random_rooms(const char ** roomnames, const char ** chosen_roomnames, int chosen_indices[], int num_rooms, int num_to_choose){
	srand((unsigned)time(NULL));
	int rooms_to_fill = num_to_choose;
	int i;
	while (rooms_to_fill > 0){
		i = rand() % 10;
		if(chosen_indices[i] == 0){
			chosen_indices[i] = 1;
			rooms_to_fill--;
		}
	}
	int j = 0;
	i = 0;
	for (; i < num_rooms; i++){
		if (chosen_indices[i] == 1){
			chosen_roomnames[j] = roomnames[i];
			j++;
		}
	}
}

/******************************************************************************
 *
 *
 *
 *****************************************************************************/
char ** create_filename_pointers(int num_files){
	char** x;

	x = malloc(num_files * sizeof(char*));
	int i = 0;
	for (; i < num_files; i++) {
		x[i] = malloc(100 * sizeof(char));
	}
	return x;
}

/******************************************************************************
 *
 *
 *
 *****************************************************************************/
void delete_filename_pointers(char ** x, int num_files){
  int i = 0;
	for (; i < num_files; i++) {
		free(x[i]);
	}
	free(x);
}

/******************************************************************************
 *
 *
 *
 *****************************************************************************/
int ** allocate_and_init_connections(){
	int ** connections = malloc(7 * sizeof(int*));
	int i = 0;
	for (; i < 7; i++){
		connections[i] = malloc(7 * sizeof(int));
	}
	i = 0;
	for (; i < 7; i++){
	  int j = 0;
		for(; j < 7; j++){
			connections[i][j] = 0;
		}
	}
	return connections;
}

/******************************************************************************
 *
 *
 *
 *****************************************************************************/
void delete_connections(int ** connections){
  int i = 0;
	for (; i < 7; i++){
		free(connections[i]);
	}
	free(connections);
}

/******************************************************************************
 *
 *
 *
 *****************************************************************************/
void get_user_choice(char * current_room, char ** connection_rooms, char * user_choice){
	int matches = 0;
	int choice = 0;
	while(matches == 0){
		printf("CURRENT LOCATION: %s\n", current_room);
		int count = 0;
		printf("POSSIBLE CONNECTIONS: ");
		while(count < 6 && connection_rooms[count][0] != 0){
			count++;
		}
		int i = 0;
		for (; i < count; i++){
			if(i < count - 1){
				printf("%s, ", connection_rooms[i]);
			}
			else if (i == (count -1)){
				printf("%s.\n", connection_rooms[i]);
			}
		}
		printf("WHERE TO? > ");
		fgets(user_choice, 100, stdin);
		user_choice[strlen(user_choice) - 1] = 0;
		i = 0;
		for (; i < count; i++){
			if(strcmp(user_choice, connection_rooms[i]) == 0){
				choice = i;
				matches = 1;
			}
		}
		if (matches == 0){
			printf("\nHUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
		}
	}
}

/******************************************************************************
 *
 *
 *
 *****************************************************************************/
void play_game(char * directoryname, const char * start_room, const char * end_room){
	char * current_room = malloc(100 * sizeof(char));
	memset(current_room, 0, sizeof(current_room));
	strcpy(current_room, start_room);
	char * filename = malloc(100* sizeof(char));
	char * user_choice = malloc(100 * sizeof(char));
	char ** visited_rooms = create_filename_pointers(100);
	int i = 0;
	for (; i < 100; i++){
		memset(visited_rooms[i], 0, sizeof(visited_rooms[i]));
	}
	int num_rooms_visited = 0;
	while(strcmp(current_room, end_room) != 0){
		memset(filename, 0, sizeof(filename));
		strcpy(filename, directoryname);
		char * end = filename + strlen(filename);
		end += sprintf(end, "/%s", current_room);
		FILE *file = fopen(filename, "r");
		int count = 0;
		if ( file != NULL ){
			char line[256];
			memset(line, 0, sizeof(line));
			char ** connection_rooms = create_filename_pointers(6);
			i = 0;
			for (; i < 6; i++){
				memset(connection_rooms[i], 0, sizeof(connection_rooms[i]));
			}
			int count = 0;
			while (fgets(line, sizeof line, file) != NULL){
				char new_buff[100];
				memset(new_buff, 0, sizeof(new_buff));
				strncpy(new_buff, line, 10);
				if (strcmp(new_buff, "ROOM NAME:") == 0){
					// this is the first line of the file
					// print the room name
					strncpy(current_room, line+11, 10);
					current_room[strlen(current_room)-1] = 0;
					//printf("CURRENT LOCATION: %s\n", current_room);
				}
				else if(strcmp(new_buff, "ROOM TYPE:") == 0){
					// this the last line of the file
				}
				else{
					// this is a connection line
					strncpy(connection_rooms[count], line + 14, 10);
					connection_rooms[count][strlen(connection_rooms[count])-1] = 0;
					count++;
				}	
			}
			// TODO: Get choice from user, and somehow get the room type
		    get_user_choice(current_room, connection_rooms, user_choice);
			memset(current_room, 0, sizeof(current_room));
			strcpy(current_room, user_choice);
			strcpy(visited_rooms[num_rooms_visited], user_choice);
			num_rooms_visited++;
			delete_filename_pointers(connection_rooms, 6);
			fclose(file);
			
		}
	}
	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS: \n", num_rooms_visited);
	i = 0;
	for(; i < num_rooms_visited; i++){
		printf("%s\n", visited_rooms[i]);
	}
	
	delete_filename_pointers(visited_rooms, 100);
	free(user_choice);
	free(filename);
	free(current_room);
}


int main(){
	int ** connections = allocate_and_init_connections();
	int * start_end = malloc(2*sizeof(int));
	char directoryname[100];
	const char * roomnames[10];
	char ** filenames = create_filename_pointers(7);
	roomnames[0] = "never";
	roomnames[1] = "gonna";
	roomnames[2] = "give";
	roomnames[3] = "you";
	roomnames[4] = "up";
	roomnames[5] = "NEVER";
	roomnames[6] = "GONNA";
	roomnames[7] = "let";
	roomnames[8] = "YOU";
	roomnames[9] = "down";
	const char * chosen_roomnames[7];
	int chosen_indices[] = {0,0,0,0,0,0,0,0,0,0};
	choose_random_rooms(roomnames, chosen_roomnames, chosen_indices, 10, 7);
	startup(directoryname, chosen_roomnames, filenames, connections, start_end);
	play_game(directoryname, chosen_roomnames[start_end[0]], chosen_roomnames[start_end[1]]);
	free(start_end);
	delete_connections(connections);
	delete_filename_pointers(filenames, 7);
	exit(0);
}
