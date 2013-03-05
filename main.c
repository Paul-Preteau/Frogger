#include <stdio.h>
#include <pthread.h>
#include "mymutex.h"
#include "console.h"
#include "indexed_list.h"

#define NUM_MAIN_THREADS 4
#define NUM_SCREEN_THREADS 1
#define NUM_LOG_THREADS 4

#define LOG_HEIGHT 4

static char *log_image[] = {"/======================\\","|                      |","|                      |","\\======================/"};
static char *frog_eyes[] = {"@@","<>"};
static char *frog_eyes_closed = {"--"};
static char *frog_eyes_opened = {"@@"};


static pthread_t main_threads[4];
static pthread_t log_threads[4];

int t=0;

static boolean game_running = TRUE;
pthread_mutex_t game_lock;
pthread_mutex_t frog_mutex;
pthread_mutex_t frog_cond_mutex;
int frog_dead = 0;
pthread_mutex_t kbd_mutex;
pthread_cond_t main_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t logs_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t frog_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t kbd_cond = PTHREAD_COND_INITIALIZER;

int frog_x = SCR_BOTTOM-2;//NUM_ROWS+15
int frog_x_new = SCR_BOTTOM-2;
int frog_y = SCR_WIDTH/2;
int frog_y_new = SCR_WIDTH/2;
int frog_level = 0;
boolean frog_on_log = FALSE;

int number_of_lives = 4;


void start_screen_thread();
void create_log_thread(enum LOG_TYPE type);
void *create_frog_thread();
void *create_keyboard_thread();

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

void updateNumLives(){
  mutex_lock(&screen_mutex);
  char numLives = (char)(((int)'0')+number_of_lives);
  put_string(&numLives, FROG_HOME_ROW, SCR_COL_LIVES, 1);
  mutex_unlock(&screen_mutex);
}


int main(int argc, char*argv[]) {

  game_lock = inicialize_mutex_var(); //gives us a recursive lock
  frog_mutex = inicialize_mutex_var();
  kbd_mutex = inicialize_mutex_var();
  screen_mutex = inicialize_mutex_var();

  //set default frog position
  


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

   //put number of lives on top of the screen
   updateNumLives();
   

   rc = pthread_create(&main_threads[t], NULL, create_frog_thread, NULL);
   checkResults("Failed at screen thread create with val: ", rc);
   t++;

   rc = pthread_create(&main_threads[t], NULL, create_keyboard_thread, NULL);
   checkResults("Failed at screen thread create with val: ", rc);
   t++;


   mutex_lock(&game_lock);
   while(game_running){
      rc = pthread_cond_wait(&main_cond, &game_lock);
      checkResults("Failed at wait with val: ", rc)
   }
   mutex_unlock(&game_lock);

   //join all together

   for (i=0; i<NUM_MAIN_THREADS; i++) {
    rc = pthread_join(main_threads[i], NULL);
    checkResults("pthread_join()\n", rc);
  }
   
  //destroy mutex and cond
  
  getchar();   

  screen_fini();
  exit(0);
 }
 return 0;
}

boolean checkForFrogLeft(int x, int y){
  boolean isThere = FALSE; //it isn't there

  int row = 0;
  int col = 0;


  for(row = x; row < (x+LOG_HEIGHT); row++ ){
    for(col = y; col < (y+LOG_WIDTH); col++){
      if(row == frog_x && col == frog_y && isThere == 0){
        
        isThere = TRUE;
        frog_on_log = TRUE;
        int temp = frog_y-1;

        if(temp <= SCR_RIGHT-2 && temp >= SCR_LEFT){
        frog_y_new = frog_y-1; 
        }else{
          frog_y_new = frog_y;
        }
        
      }
    }
  }

  return isThere;
}

boolean checkForFrogRight(int x, int y){
  boolean isThere = FALSE; //it isn't there

  int row = 0;
  int col = 0;


  for(row = x; row < (x+LOG_HEIGHT); row++ ){
    for(col = y; col < (y+LOG_WIDTH); col++){
      if(row == frog_x && col == frog_y && isThere == 0){
        isThere = TRUE;
        int temp = frog_y+1;
        frog_on_log = TRUE;
        if(temp <= (SCR_RIGHT-3) && temp >= (SCR_LEFT)){
        
        frog_y_new = frog_y+1; 

        }else{
    
          frog_y_new = frog_y; 

        }
      }
    }
  }

  return isThere;
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
      rc = pthread_cond_wait(&main_cond, &game_lock);
      checkResults("Failed at wait with val: ", rc)
   }
   mutex_unlock(&game_lock);
  

   //printf("%s\n", "Joining threads in Create Log");
   for (i=0; i<NUM_LOG_THREADS; ++i) {
    rc = pthread_join(log_threads[i], NULL);
    checkResults("pthread_join_logs()\n", rc);
  }


    printf("%s\n", "Returned from LOG");

  return NULL;
}

void *screen_redraw(){

  //critical section

  while(game_running){ //change to gameStillOn
    mutex_lock(&screen_mutex);
  //put_banner("Created Screen Thread");
    screen_refresh();
    mutex_unlock(&screen_mutex);
    sleep_ticks(1);
  //critical section ends
  }

  destroy_mutex_var(screen_mutex);
  
  return NULL;

} // end screen_redraw

void *top_log_draw(void *in_log){ 
  Log *curr_log=in_log;
  
  int i;

  for(i=0; i < SCR_WIDTH + LOG_WIDTH; i++){
    curr_log->y = SCR_RIGHT-i;
    mutex_lock(&screen_mutex); //this might be too much
    screen_clear_image(curr_log->x, curr_log->y+1, LOG_WIDTH, LOG_HEIGHT);
    screen_draw_image(curr_log->x, curr_log->y, log_image, LOG_HEIGHT);
    curr_log->hasFrog = checkForFrogLeft(curr_log->x, curr_log->y);
    mutex_unlock(&screen_mutex); //this might be too much 


    if(i == 27*2){ //make a constant somewhere - DISTANCE_BETWEEN_LOGS
    	//send signal back to main log thread
      curr_log->newLog = 1;
      mutex_lock(&curr_log->new_log_mutex);
      int rc = pthread_cond_broadcast(&curr_log->new_log_cond);
      checkResults("Failed to signal in log: ", rc);
      mutex_unlock(&curr_log->new_log_mutex);
    }
    
    sleep_ticks(2);
  }
 
  //I'm done, signal to delete me
  //
  curr_log->conditionMet=1;
  mutex_lock(&curr_log->log_mutex);
  int rc = pthread_cond_broadcast(&curr_log->log_cond);
  checkResults("Failed to signal in log: ", rc);
  mutex_unlock(&curr_log->log_mutex);
  return NULL;
}

void *middletop_log_draw(void *in_log){

  Log *curr_log=in_log;
  int i;

  for(i=SCR_WIDTH + LOG_WIDTH; i > -1; i--){

    mutex_lock(&screen_mutex);
    curr_log->y = SCR_RIGHT-i;
    screen_clear_image(curr_log->x, curr_log->y-1, LOG_WIDTH, LOG_HEIGHT);
    screen_draw_image(curr_log->x, curr_log->y, log_image, LOG_HEIGHT);
    curr_log->hasFrog = checkForFrogRight(curr_log->x, curr_log->y);
    mutex_unlock(&screen_mutex);

    if(i == 27*2){ //make a constant somewhere
      curr_log->newLog = 1;
      mutex_lock(&curr_log->new_log_mutex);
      int rc = pthread_cond_broadcast(&curr_log->new_log_cond);
      checkResults("Failed to signal in log: ", rc);
      mutex_unlock(&curr_log->new_log_mutex);
    }
    sleep_ticks(3);
  }
  
  curr_log->conditionMet=1;
  mutex_lock(&curr_log->log_mutex);
  int rc = pthread_cond_broadcast(&curr_log->log_cond);
  checkResults("Failed to signal in log: ", rc);
  mutex_unlock(&curr_log->log_mutex);
  return NULL;
}

void *middlebot_log_draw(void *in_log){

  Log *curr_log = in_log;  
  
  int i;
  for(i=0; i < SCR_WIDTH + LOG_WIDTH; i++){

    mutex_lock(&screen_mutex);
    curr_log->y = SCR_RIGHT-i;
    screen_clear_image(curr_log->x, curr_log->y+1, LOG_WIDTH, LOG_HEIGHT);
    screen_draw_image(curr_log->x, curr_log->y, log_image, LOG_HEIGHT);
    curr_log->hasFrog = checkForFrogLeft(curr_log->x, curr_log->y);
    mutex_unlock(&screen_mutex);


    if(i == 27*2){ //make a constant somewhere
      curr_log->newLog = 1;
      mutex_lock(&curr_log->new_log_mutex);
      int rc = pthread_cond_broadcast(&curr_log->new_log_cond);
      checkResults("Failed to signal in log: ", rc);
      mutex_unlock(&curr_log->new_log_mutex);
    }

    sleep_ticks(4);
  }
  
  curr_log->conditionMet=1;
  mutex_lock(&curr_log->log_mutex);
  int rc = pthread_cond_broadcast(&curr_log->log_cond);
  checkResults("Failed to signal in log: ", rc);
  mutex_unlock(&curr_log->log_mutex);
  return NULL;
  
}

void *bot_log_draw(void *in_log){

  Log *curr_log = in_log;

  int i;
  for(i=SCR_WIDTH + LOG_WIDTH; i > -1; i--){

    mutex_lock(&screen_mutex);
    curr_log->y = SCR_RIGHT-i;
    screen_clear_image(curr_log->x, curr_log->y-1, LOG_WIDTH, LOG_HEIGHT);
    screen_draw_image(curr_log->x, curr_log->y, log_image, LOG_HEIGHT);
    curr_log->hasFrog = checkForFrogRight(curr_log->x, curr_log->y);
    mutex_unlock(&screen_mutex);

    if(i == 27*2){ //make a constant somewhere
      curr_log->newLog = 1;
      mutex_lock(&curr_log->new_log_mutex);
      int rc = pthread_cond_broadcast(&curr_log->new_log_cond);
      checkResults("Failed to signal in log: ", rc);
      mutex_unlock(&curr_log->new_log_mutex);

    }
    sleep_ticks(5);
  }
  

  curr_log->conditionMet=1;
  mutex_lock(&curr_log->log_mutex);
  int rc = pthread_cond_broadcast(&curr_log->log_cond);
  checkResults("Failed to signal in log: ", rc);
  mutex_unlock(&curr_log->log_mutex);
  return NULL;
  
}

Log* generateDefaultLog(){
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  Log *temp; 
  temp = malloc( sizeof ( Log ) );
  temp->hasFrog = FALSE;
  temp->logID = getNextLogInId();
  temp->log_mutex = mutex;
  temp->new_log_mutex = mutex;
  pthread_cond_init(&temp->log_cond, NULL);
  pthread_cond_init(&temp->new_log_cond, NULL);
  temp->conditionMet = 0;
  temp->newLog = 0;
  temp->x = 0;
  temp->y = 0;

  return temp;
}

void *create_top_logs(){

  int rc = 0;
  pthread_t tID_curr;

  while(game_running){
  Log* top_log = generateDefaultLog();
  

  top_log->x = SCR_TOP+4;
  insert(top_log_list, top_log);

  rc = pthread_create(&tID_curr, NULL, top_log_draw, (void *)top_log);

  //lock on a new thread cond

  mutex_lock(&top_log->new_log_mutex);
  while(!top_log->newLog){
    rc = pthread_cond_wait(&top_log->new_log_cond, &top_log->new_log_mutex);
    checkResults("Failed at wait on new log with val: ", rc)
  }
  mutex_unlock(&top_log->new_log_mutex);

  //create a new thread
  create_top_logs();
  
  //lock on 'prepare to die' condition
  mutex_lock(&top_log->log_mutex);
   while(!top_log->conditionMet){
      rc = pthread_cond_wait(&top_log->log_cond, &top_log->log_mutex);
      checkResults("Failed at wait with val: ", rc)
   }
   mutex_unlock(&top_log->log_mutex);

   //clean up procedures
  removeWithId(top_log_list, top_log->logID);

  rc = pthread_join(tID_curr, NULL);
  checkResults("pthread_join()\n", rc);

  }
  return NULL;
}

void *create_middletop_logs(){
  int rc = 0;

  while(game_running){
    Log* middle_top_log = generateDefaultLog();
    pthread_t tID_curr;

    middle_top_log->x = SCR_TOP+8;
    insert(middle_top_log_list, middle_top_log);

    rc = pthread_create(&tID_curr, NULL, middletop_log_draw, (void *)middle_top_log);

  //lock on a new thread cond

    mutex_lock(&middle_top_log->new_log_mutex);
    while(!middle_top_log->newLog){
      rc = pthread_cond_wait(&middle_top_log->new_log_cond, &middle_top_log->new_log_mutex);
      checkResults("Failed at wait on new log with val: ", rc)
    }
    mutex_unlock(&middle_top_log->new_log_mutex);

  //create a new thread
    create_middletop_logs();

  //lock on 'prepare to die' condition
    mutex_lock(&middle_top_log->log_mutex);
    while(!middle_top_log->conditionMet){
      rc = pthread_cond_wait(&middle_top_log->log_cond, &middle_top_log->log_mutex);
      checkResults("Failed at wait with val: ", rc)
    }
    mutex_unlock(&middle_top_log->log_mutex);

   //clean up procedures
    removeWithId(middle_top_log_list, middle_top_log->logID);

    rc = pthread_join(tID_curr, NULL);
    checkResults("pthread_join()\n", rc);
  }
  return NULL;
}

void *create_middlebot_logs(){
  int rc = 0;

  while(game_running){
    Log* middle_bot_log = generateDefaultLog();
    pthread_t tID_curr;

    middle_bot_log->x = SCR_TOP+12;
    insert(middle_bot_log_list, middle_bot_log);

    rc = pthread_create(&tID_curr, NULL, middlebot_log_draw, (void *)middle_bot_log);

  //lock on a new thread cond

    mutex_lock(&middle_bot_log->new_log_mutex);
    while(!middle_bot_log->newLog){
      rc = pthread_cond_wait(&middle_bot_log->new_log_cond, &middle_bot_log->new_log_mutex);
      checkResults("Failed at wait on new log with val: ", rc)
    }
    mutex_unlock(&middle_bot_log->new_log_mutex);

  //create a new thread
    create_middlebot_logs();

  //lock on 'prepare to die' condition
    mutex_lock(&middle_bot_log->log_mutex);
    while(!middle_bot_log->conditionMet){
      rc = pthread_cond_wait(&middle_bot_log->log_cond, &middle_bot_log->log_mutex);
      checkResults("Failed at wait with val: ", rc)
    }
    mutex_unlock(&middle_bot_log->log_mutex);

   //clean up procedures
    removeWithId(middle_bot_log_list, middle_bot_log->logID);

    rc = pthread_join(tID_curr, NULL);
    checkResults("pthread_join()\n", rc);
  }
  return NULL;
}

void *create_bot_logs(){
  int rc = 0;

  while(game_running){
    Log* bot_log = generateDefaultLog();
    pthread_t tID_curr;

    bot_log->x = SCR_TOP+16;
    insert(bot_log_list, bot_log);

    rc = pthread_create(&tID_curr, NULL, bot_log_draw, (void *)bot_log);

  //lock on a new thread cond

    mutex_lock(&bot_log->new_log_mutex);
    while(!bot_log->newLog){
      rc = pthread_cond_wait(&bot_log->new_log_cond, &bot_log->new_log_mutex);
      checkResults("Failed at wait on new log with val: ", rc)
    }
    mutex_unlock(&bot_log->new_log_mutex);

  //create a new thread
    create_bot_logs();

  //lock on 'prepare to die' condition
    mutex_lock(&bot_log->log_mutex);
    while(!bot_log->conditionMet){
      rc = pthread_cond_wait(&bot_log->log_cond, &bot_log->log_mutex);
      checkResults("Failed at wait with val: ", rc)
    }
    mutex_unlock(&bot_log->log_mutex);

   //clean up procedures
    removeWithId(bot_log_list, bot_log->logID);

    rc = pthread_join(tID_curr, NULL);
    checkResults("pthread_join()\n", rc);
  }
  return NULL;
}




void *draw_frog(){
  //create mutex
  //
  int eyes_switch = 0;
  int counter = 0;
  int move_counter = 0;
  while(game_running){


    if(frog_on_log == FALSE && frog_level > 0 && frog_level < 5){
      //decrement number of lives left
      //
      //clear the image of where you were
      //
      number_of_lives--;
      updateNumLives();

      if(number_of_lives == 0){
        mutex_lock(&screen_mutex);
        put_banner("You loose! Press 'Q' to quit");
        mutex_unlock(&screen_mutex);

        game_running = FALSE;
        getchar();
        screen_fini();//more quitting stuff to add here

      }
      
       frog_x = SCR_BOTTOM-2;//NUM_ROWS+15
       frog_x_new = SCR_BOTTOM-2;
       frog_y = SCR_WIDTH/2;
       frog_y_new = SCR_WIDTH/2;
       frog_level = 0;
       frog_on_log = FALSE;
       pthread_cond_broadcast(&frog_cond);
      //return NULL;
    }

    if(eyes_switch == 0 && counter == 25){
      eyes_switch = 1;
      frog_eyes[0] = frog_eyes_closed;
      counter=0;
    }else if(counter == 25){
      eyes_switch = 0;
      frog_eyes[0] = frog_eyes_opened;
      counter=0;
    }

    mutex_lock(&frog_mutex);
    mutex_lock(&screen_mutex);
    screen_clear_image(frog_x, frog_y, FROG_WIDTH, FROG_HEIGHT);
    screen_draw_image(frog_x_new, frog_y_new, frog_eyes, FROG_HEIGHT);
    frog_x = frog_x_new;
    frog_y = frog_y_new;
    
    mutex_unlock(&screen_mutex);
    mutex_unlock(&frog_mutex);

    move_counter++;
    counter++;
    sleep_ticks(1);
  }
 
  destroy_mutex_var(frog_mutex);
  pthread_cond_broadcast(&frog_cond);
  frog_dead = 1;

  
  return NULL;
}

void *create_frog_thread(){
  int rc = 0;
  pthread_t frog_thread;
  frog_cond_mutex = inicialize_mutex_var();
  pthread_cond_init(&frog_cond, NULL);
  while(game_running){
     rc = pthread_create(&frog_thread, NULL, draw_frog, NULL);
    if (rc){
   printf("ERROR-return code from pthread_create() in log draw is %d\n", rc);
   exit(-1);
    }

   mutex_lock(&frog_cond_mutex);
   while(frog_dead == 0){
    rc = pthread_cond_wait(&frog_cond, &frog_cond_mutex);
    checkResults("Failed at creating a new frog: ", rc);
   }
   mutex_unlock(&frog_cond_mutex);

   rc = pthread_join(frog_thread, NULL);
   checkResults("Failed at joining frog thread: ", rc);


  }
  return NULL;
}

boolean checkFrogOnLogDown(Log* log){

  boolean isThere = FALSE; //it isn't there
  int x = log->x;
  int y = log->y;

  mutex_lock(&frog_mutex);

      if(frog_x <= x && frog_x >= x + LOG_HEIGHT+1 && frog_y >= y+LOG_WIDTH && frog_y <= y){
        isThere = TRUE;
      }
  mutex_unlock(&frog_mutex);

  return isThere;

}


boolean checkRowForFrogDown(List *list_to_check){

  boolean toReturn = FALSE;

        int i;
        Log *curr;
          for(i=0; i < size(list_to_check); i++){
            curr = get(list_to_check, i);
            if(checkFrogOnLogDown(curr) == TRUE){
              toReturn = TRUE;
            }
          }

  return toReturn;
}


boolean checkFrogOnLog(Log* log){

  boolean isThere = FALSE; //it isn't there
  int x = log->x;
  int y = log->y;

  int tempFrogX = frog_x;
  int tempFrogY = frog_y;

  mutex_lock(&frog_mutex);

      if(frog_x >= x && frog_x <= x + LOG_HEIGHT+1 && frog_y <= y+LOG_WIDTH && frog_y >= y){
        isThere = TRUE;
      }
  mutex_unlock(&frog_mutex);

  return isThere;

}


boolean checkRowForFrog(List *list_to_check){

  boolean toReturn = FALSE;

        int i;
        Log *curr;
          for(i=0; i < size(list_to_check); i++){
            curr = get(list_to_check, i);
            if(checkFrogOnLog(curr) == TRUE){
              toReturn = TRUE;
            }
          }

  return toReturn;
}

void *keyboard_listen(){
  while(game_running){

    int received_char = getchar();
    char dig = (char)received_char;

    if(dig == 'w'){
      int temp = frog_x-4;

      if(temp <= SCR_BOTTOM-2 && temp >= SCR_TOP+4){
        mutex_lock(&frog_mutex);
        frog_x_new = frog_x-4;
        mutex_unlock(&frog_mutex);

      }

      if(frog_level < 5){
        frog_level++; 

        if(frog_level == 1){
          frog_on_log = checkRowForFrog(bot_log_list);
          }else if(frog_level == 3){
            frog_on_log = checkRowForFrog(middle_top_log_list);
          }else if(frog_level == 2){
            frog_on_log = checkRowForFrog(middle_bot_log_list);
          }else if(frog_level == 4){
            frog_on_log = checkRowForFrog(top_log_list);
          }
        }
      }

    if(dig == 's'){
      int temp = frog_x+4;

      if(temp <= SCR_BOTTOM-2 && temp >= SCR_TOP+4){
        mutex_lock(&kbd_mutex);
        frog_x_new = frog_x+4;
        mutex_unlock(&kbd_mutex);

      }
      if(frog_level > 0){
        frog_level--;

        if(frog_level == 1){
          frog_on_log = checkRowForFrogDown(bot_log_list);
          }else if(frog_level == 3){
            frog_on_log = checkRowForFrogDown(middle_top_log_list);
          }else if(frog_level == 2){
            frog_on_log = checkRowForFrogDown(middle_bot_log_list);
          }else if(frog_level == 4){
            frog_on_log = checkRowForFrogDown(top_log_list);
          }
        }
      }

    if(dig == 'a'){
      int temp = frog_y-1;

      if(temp <= SCR_RIGHT-2 && temp >= SCR_LEFT){


        mutex_lock(&frog_mutex);
        frog_y_new = frog_y-1;
        mutex_unlock(&frog_mutex);
      }
    }

    if(dig == 'd'){

      int temp = frog_y+1;

      if(temp <= SCR_RIGHT-2 && temp >= SCR_LEFT){
        mutex_lock(&kbd_mutex);
        frog_y_new = frog_y+1;
        mutex_unlock(&kbd_mutex);

      }
    }

    if(dig == 'q'){
      mutex_lock(&kbd_mutex);
      put_banner("quitter... ");
      //sleep_ticks(10);
      game_running = FALSE;
      mutex_unlock(&kbd_mutex);
      pthread_cond_broadcast(&main_cond);
      
    }
  }
  return NULL;
}

void *create_keyboard_thread(){
  int rc = 0;
  pthread_t kbd_thread;
  rc = pthread_create(&kbd_thread, NULL, keyboard_listen, NULL);

  mutex_lock(&game_lock);
   while(game_running){
      rc = pthread_cond_wait(&main_cond, &game_lock);
      checkResults("Failed at wait with val: ", rc)
   }
   mutex_unlock(&game_lock);

   rc = pthread_join(kbd_thread, NULL);
    checkResults("pthread_join_keyboard()\n", rc);

  return NULL;
}



