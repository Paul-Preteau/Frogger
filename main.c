#include <stdio.h>
#include <pthread.h>
#include "mymutex.h"
#include "console.h"
#include "indexed_list.h"

#define NUM_MAIN_THREADS 4
#define NUM_SCREEN_THREADS 1
#define NUM_TOP_LOG_THREADS 1

#define LOG_HEIGHT 4

static char *log_image[] = {"/======================\\","|                      |","|                      |","\\======================/"};
static char *frog_eyes[] = {"@@","<>"};
static char *frog_eyes_closed[] = {"--", "<>"};


static pthread_t threads[100000];
static pthread_t main_threads[4];
static pthread_t log_threads[4];
static pthread_t 
int t=0;

static boolean game_running = TRUE;
pthread_mutex_t game_lock;
pthread_cond_t main_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t logs_cond = PTHREAD_COND_INITIALIZER;

int eyes_switch = 0;
int frog_x = SCR_BOTTOM-2;//NUM_ROWS+15
int frog_x_new = SCR_BOTTOM-2;
int frog_y = SCR_WIDTH/2;
int frog_y_new = SCR_WIDTH/2;


void start_screen_thread();
void create_log_thread(enum LOG_TYPE type);
void create_frog_thread();
void create_keyboard_thread();

pthread_mutexattr_t attributes;
pthread_mutex_t top_list_mutex;
pthread_mutex_t screen_mutex;

static int logInId = 0;

int getNextLogInId(){
  return logInId++;
}

List *top_log_list;
List *middle_top_log_list;
List *middle_bot_log_list;
List *bot_log_list;


void *create_logs();
void *screen_redraw();
void *create_top_logs();
void *create_middletop_logs();
void *create_middlebot_logs();
void *create_bot_logs();


int main(int argc, char*argv[]) {

  game_lock = inicialize_mutex_var(); //gives us a recursive lock


  top_log_list = construct();
  assert(top_log_list != NULL);

  middle_top_log_list = construct();
  assert(middle_top_log_list != NULL);

  middle_bot_log_list = construct();
  assert(middle_bot_log_list != NULL);

  bot_log_list = construct();
  assert(bot_log_list != NULL);

  int t = 0;
  int rc = 0;
  int i = 0; 


  if (screen_init(40, 80)) {

   rc = pthread_create(&main_threads[t], NULL, create_logs, NULL);
   checkResults("Failed at log thread create with val: ", rc);
   t++;

   rc = pthread_create(&main_threads[t], NULL, screen_redraw, NULL);
   checkResults("Failed at screen thread create with val: ", rc);
   t++;


   mutex_lock(&game_lock);
   while(game_running){
      rc = pthread_cond_wait(&main_cond, &game_lock);
      checkResults("Failed at wait with val: ", rc)
   }
   mutex_unlock(&game_lock);

   //join all together

   printf("%s\n", "Joining threads in Main");
   for (i=0; i<NUM_MAIN_THREADS; ++i) {
    rc = pthread_join(main_threads[i], NULL);
    checkResults("pthread_join()\n", rc);
  }
   
  //destroy mutex and cond
   

  //screen_fini();
 }
 return 0;
}

void *create_logs(){

  //create 4 different log threads. 
  //has to manage 
  int t = 0;
  int rc = 0;
  int i = 0;
  
  rc = pthread_create(&log_threads[t], NULL, create_top_logs, NULL);
  checkResults("Failed at top log thread create with val: ", rc);
  t++;

  rc = pthread_create(&log_threads[t], NULL, create_middletop_logs, NULL);
  checkResults("Failed at top log thread create with val: ", rc);
  t++;

  rc = pthread_create(&log_threads[t], NULL, create_middlebot_logs, NULL);
  checkResults("Failed at top log thread create with val: ", rc);
  t++;

  rc = pthread_create(&log_threads[t], NULL, create_bot_logs, NULL);
  checkResults("Failed at top log thread create with val: ", rc);
  t++;

  mutex_lock(&game_lock);
   while(game_running){
      rc = pthread_cond_wait(&logs_cond, &game_lock);
      checkResults("Failed at wait with val: ", rc)
   }
   mutex_unlock(&game_lock);
  

   printf("%s\n", "Joining threads in Create Log");
   for (i=0; i<NUM_MAIN_THREADS; ++i) {
    rc = pthread_join(main_threads[i], NULL);
    checkResults("pthread_join()\n", rc);
  }
  //set up condition variables
  //go to sleep
  //wake up when a thread dies

  //join them together


}


int checkForFrogLeft(int x, int y){
  int isThere = 0; //it isn't there

  int row = 0;
  int col = 0;


  for(row = x; row < (x+LOG_HEIGHT); row++ ){
    for(col = y; col < (y+LOG_WIDTH); col++){
      if(row == frog_x && col == frog_y && isThere == 0){

        isThere = 1;
        //exit(1);
        int temp = frog_y-1;

        if(temp <= SCR_RIGHT-2 && temp >= SCR_LEFT){
          frog_y_new = frog_y-1; 
        //create_frog_thread();
        }else{
          frog_y_new = frog_y;
          //create_frog_thread();
        }
        
        //start_screen_thread();
      }
    }
  }



  return isThere;
}

int checkForFrogRight(int x, int y){
  int isThere = 0; //it isn't there

  int row = 0;
  int col = 0;


  for(row = x; row < (x+LOG_HEIGHT); row++ ){
    for(col = y; col < (y+LOG_WIDTH); col++){
      if(row == frog_x && col == frog_y && isThere == 0){
        isThere = 1;
        int temp = frog_y+1;
        if(temp <= (SCR_RIGHT-3) && temp >= (SCR_LEFT)){
          frog_y_new = frog_y+1; 
        //create_frog_thread();
        }else{
          frog_y_new = frog_y; 
          //create_frog_thread();
        }
        //start_screen_thread();
      }
    }
  }


  return isThere;
}


void *screen_redraw(){

  //create a mutex

  screen_mutex = inicialize_mutex_var();

  //critical section

  while(1){ //change to gameStillOn
    mutex_lock(&screen_mutex);
  //put_banner("Created Screen Thread");
    screen_refresh();
    mutex_unlock(&screen_mutex);
    sleep_ticks(1);
  //critical section ends
  }

  destroy_mutex_var(screen_mutex);
  
  pthread_exit(NULL);//kill a refresh thread

} // end thrfunc

void *top_log_draw(void *in_log){ 
  Log *curr_log=in_log;
  
  int i;

  for(i=0; i < SCR_WIDTH + LOG_WIDTH; i++){


    //lock_list( top_log_list );
    mutex_lock(&curr_log->log_mutex);
    mutex_lock(&screen_mutex); //this might be too much
    screen_clear_image(curr_log->x, SCR_RIGHT-i+1, LOG_WIDTH, LOG_HEIGHT);
    screen_draw_image(curr_log->x, SCR_RIGHT-i, log_image, LOG_HEIGHT);
    if(curr_log->hasFrog == TRUE){
    	//lock frog mutex
    	frog_y = frog_y++;
    	//unlock frog mutex
    }
    //int result = checkForFrogLeft(SCR_TOP+4, SCR_RIGHT-i);
    mutex_unlock(&screen_mutex); //this might be too much 
    mutex_unlock(&curr_log->log_mutex);
    //unlock_list( top_log_list );


    if(i == 27*2){ //make a constant somewhere - DISTANCE_BETWEEN_LOGS
    	//send signal back to main log thread
      enum LOG_TYPE logB = TOP; 
      create_log_thread(logB);
    }
    
    sleep_ticks(2);
  }
  removeWithId(top_log_list, curr_log->logID);
  //I'm done, pass the pointer of the struct back to creator
  //
  pthread_exit(NULL);//kill a log thread
  
}

void *middletop_log_draw(void *in_log){

  Log *curr_log=in_log;
  
  int i = 0;

  for(i=SCR_WIDTH + LOG_WIDTH; i > -1; i--){

    mutex_lock(&curr_log->log_mutex);
    mutex_lock(&screen_mutex);

    screen_clear_image(SCR_TOP+8, SCR_RIGHT-i-1, LOG_WIDTH, LOG_HEIGHT);
    screen_draw_image(SCR_TOP+8, SCR_RIGHT-i, log_image, LOG_HEIGHT);
    
    mutex_unlock(&screen_mutex);
    mutex_unlock(&curr_log->log_mutex);

    if(i == 27*2){ //make a constant somewhere
      enum LOG_TYPE logB = MIDDLE_TOP; 
      create_log_thread(logB);
    }
    sleep_ticks(3);
  }
  
  removeWithId(middle_top_log_list, curr_log->logID);
  pthread_exit(NULL);//kill a log thread
  
}

void *middlebot_log_draw(void *in_log){

  Log *curr_log = in_log;  
  
  int i;
  for(i=0; i < SCR_WIDTH + LOG_WIDTH; i++){

    lock_list(middle_bot_log_list);
    mutex_lock(&screen_mutex);
    
    screen_clear_image(SCR_TOP+12, SCR_RIGHT-i+1, LOG_WIDTH, LOG_HEIGHT);
    screen_draw_image(SCR_TOP+12, SCR_RIGHT-i, log_image, LOG_HEIGHT);
    
    mutex_unlock(&screen_mutex);
    unlock_list(middle_bot_log_list);


    if(i == 27*2){ //make a constant somewhere
      enum LOG_TYPE logB = MIDDLE_BOT; 
      create_log_thread(logB);
    }

    sleep_ticks(4);
  }
  
  //destroy_mutex_var(middlebot_mutex);
  //
  removeWithId(middle_bot_log_list, curr_log->logID);
  pthread_exit(NULL);//kill a log thread
  
}

void *bot_log_draw(void *in_log){

  Log *curr_log = in_log;



  int i = 0;
  for(i=SCR_WIDTH + LOG_WIDTH; i > -1; i--){
    mutex_lock(&curr_log->log_mutex);
    mutex_lock(&screen_mutex);
    screen_clear_image(SCR_TOP+16, SCR_RIGHT-i-1, LOG_WIDTH, LOG_HEIGHT);
    screen_draw_image(SCR_TOP+16, SCR_RIGHT-i, log_image, LOG_HEIGHT);
    mutex_unlock(&screen_mutex);
    mutex_unlock(&curr_log->log_mutex);
    
    

    if(i == 27*2){ //make a constant somewhere
      enum LOG_TYPE logB = BOT; 
      create_log_thread(logB);
    }
    sleep_ticks(5);
  }
  

  removeWithId(bot_log_list, curr_log->logID);
  pthread_exit(NULL);//kill a log thread
  
}

Log* generateDefaultLog(){
  int rc = 0;

  pthread_mutex_t mutex;     
  pthread_mutex_init(&mutex, NULL);
  Log *temp; 
  temp = malloc( sizeof ( Log ) );
  temp->hasFrog = FALSE;
  temp->logID = getNextLogInId();
  temp->log_mutex = mutex;
  temp->x = 0;
  temp->y = 0;

  return temp;
}

void *create_top_logs(){



  Log* top_log = generateDefaultLog();

  temp->x = SCR_TOP+4;
  insert(top_log_list, temp);

  rc = pthread_create(&threads[t], NULL, top_log_draw, (void *)temp);


}


void create_log_thread(enum LOG_TYPE type){

  int rc = 0;

  pthread_mutex_t mutex;     
  pthread_mutex_init(&mutex, NULL);
  Log *temp; 
  temp = malloc( sizeof ( Log ) );
  temp->hasFrog = FALSE;
  temp->logID = getNextLogInId();
  temp->log_mutex = mutex;
  temp->x = SCR_TOP+4;
  temp->y = 0;

  for(t=0; t<NUM_TOP_LOG_THREADS; t++){

    if(type == TOP){
      temp->x = SCR_TOP+4;
      insert(top_log_list, temp);
      rc = pthread_create(&threads[t], NULL, top_log_draw, (void *)temp);
    }else if(type == MIDDLE_TOP){
      temp->x = SCR_TOP+8;
      insert(middle_top_log_list, temp);
      rc = pthread_create(&threads[t], NULL, middletop_log_draw, (void *) temp);
    }else if(type == MIDDLE_BOT){
      temp->x = SCR_TOP+12;
      temp->y = 0;
      insert(middle_bot_log_list, temp);
      rc = pthread_create(&threads[t], NULL, middlebot_log_draw, (void*) temp);
    }else if(type == BOT){
      temp->x = SCR_TOP+16;
      temp->y = 0;
      insert(bot_log_list, temp);
      rc = pthread_create(&threads[t], NULL, bot_log_draw, (void*) temp);
    }   
    if (rc){
     printf("ERROR-return code from pthread_create() in log draw is %d\n", rc);
     exit(-1);
   }
   }// for
 }



 void *create_frog(){
  //create mutex
  //
  //pthread_mutex_t frog_mutex = inicialize_mutex_var();


  //mutex_lock(&frog_mutex);
  screen_clear_image(frog_x, frog_y, FROG_WIDTH, FROG_HEIGHT);
  screen_draw_image(frog_x_new, frog_y_new, frog_eyes, FROG_HEIGHT);
  frog_x = frog_x_new;
  frog_y = frog_y_new;

  //mutex_unlock(&frog_mutex);

  //sleep_ticks(25);
  //
  //destroy_mutex_var(frog_mutex);
  pthread_exit(NULL);//kill a frog thread
}

void create_frog_thread(){
  int rc = 0;
  rc = pthread_create(&threads[t], NULL, create_frog, NULL);
  if (rc){
   printf("ERROR-return code from pthread_create() in log draw is %d\n", rc);
   exit(-1);
 }
}


void *keyboard_listen(){

  //pthread_mutex_t kbd_mutex = inicialize_mutex_var();


  while(1){
     //mutex_lock(&kbd_mutex);

    int received_char = getchar();
    char dig = (char)received_char;

    if(dig == 'w'){
      int temp = frog_x-4;

      if(temp <= SCR_BOTTOM-2 && temp >= SCR_TOP+4){
      //mutex_lock(kbd_mutex);
        frog_x_new = frog_x-4;
      //mutex_unlock(kbd_mutex);
        create_frog_thread();
      }    
    }

    if(dig == 's'){
      int temp = frog_x+4;

      if(temp <= SCR_BOTTOM-2 && temp >= SCR_TOP+4){

    //mutex_lock(kbd_mutex);
        frog_x_new = frog_x+4;
    //mutex_unlock(kbd_mutex);
        create_frog_thread();
      }
    }

    if(dig == 'a'){
      int temp = frog_y-1;

      if(temp <= SCR_RIGHT-2 && temp >= SCR_LEFT){


    //mutex_lock(kbd_mutex);
        frog_y_new = frog_y-1;
    //mutex_unlock(kbd_mutex);
        create_frog_thread();
      }
    }

    if(dig == 'd'){

      int temp = frog_y+1;

      if(temp <= SCR_RIGHT-2 && temp >= SCR_LEFT){
    //mutex_lock(kbd_mutex);
        frog_y_new = frog_y+1;
    //mutex_unlock(kbd_mutex);
        create_frog_thread();
      }
    }

    if(dig == 'q'){
    //mutex_lock(kbd_mutex);
      put_banner("quitter... ");
      sleep_ticks(40);
      getchar();
      screen_fini();
    //mutex_unlock(kbd_mutex);
      exit(1);
    }
    //zmutex_unlock(&kbd_mutex);
  }
  
  
  pthread_exit(NULL);
}

void create_keyboard_thread(){
  int rc = 0;

  rc = pthread_create(&threads[t], NULL, keyboard_listen, NULL);
}



