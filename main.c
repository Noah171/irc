#include <gtk/gtk.h>
#include "irc.h"
#include <pthread.h>

#define WINDOWHEIGHT 960
#define WINDOWWIDTH 480
#define LABEL_BUFFER_MAX 100

enum send_button_clicked_args{ARG_LABEL=0};

static void activate(GtkApplication* app,
		     gpointer user_data);
/* Begin callback declarations */
static void cb_send_button_clicked(GtkWidget *button,
				gpointer callback_data);
static void cb_update_char_box(GtkWidget* vertical_box,
			    gpointer callback_data);
static void cb_thread_quit(GtkWidget* window,
			   gpointer callback_data);
/* End callback declaration */
static void *recv_loop(void* args);
/*
  TODO

  o Scroll window adds a scroll bar to an element, it's not its own separate element. SO
      vertical_box will be the new chat recieving widget, which will be the child of 
      scroll_window, which is a scroll window and will therefore have a scroll bar.

 */

static void activate(GtkApplication* app,
		   gpointer user_data){
  

  GtkWidget * window;
  GtkWidget * scroll_window;
  GtkWidget * grid;
  GtkWidget * button;
  GtkWidget * vertical_box;
  GtkWidget * entry;
  GtkEntryBuffer * buff;

  char * buffer = malloc(LABEL_BUFFER_MAX * sizeof(char));
  pthread_t * thread;
  void * args = malloc(sizeof(void*) * 2);
  ((int **)args)[0] = user_data; // Sends a pointer to the socketfd
  ((char **)args)[1] = buffer; // sends the buffer's address to the receive thread
  
  // Initialize the first window of the GTK application
  grid = gtk_grid_new();
  button = gtk_button_new_with_label("Send");
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  scroll_window = gtk_scrolled_window_new(NULL,NULL);
  vertical_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5); // Make it a vertical box with
  // 5 pixels of spacing
  buff = gtk_entry_buffer_new("Enter message ...",18);
  entry = gtk_entry_new_with_buffer(buff); // Text entry box

  /* Set up the window */
  gtk_window_set_title(GTK_WINDOW(window), "DeliveryClient");
  gtk_window_set_default_size(GTK_WINDOW(window), WINDOWHEIGHT,WINDOWWIDTH);
  gtk_window_set_resizable(GTK_WINDOW(window),0);
  g_signal_connect(G_OBJECT(window), "destroy",
		   G_CALLBACK(cb_thread_quit), (void*)thread);

  /* Sets up the window's grid */
  gtk_grid_set_column_homogeneous(GTK_GRID(grid), 1);
  gtk_grid_set_row_homogeneous(GTK_GRID(grid), 1);

  /* connects the send_button_clicked to the button */
  g_signal_connect(G_OBJECT(button), "clicked",
		   G_CALLBACK(cb_send_button_clicked), (void*) vertical_box);

  /* Connect the vertical box to the loop which receives connection */
  /* g_signal_connect(G_OBJECT(vertical_box), "destroy", G_CALLBACK(recv_loop), */
  /* (void*) vertical_box); */
  /* Make the vertical box a scroll window */
  gtk_container_add(GTK_CONTAINER(scroll_window), vertical_box);
  
  /* Attach children to parent grid, order of numbers is
     <LeftDistance, TopDistance, Width, Height */
  gtk_grid_attach(GTK_GRID(grid), button, 4,3,1,1);
  gtk_grid_attach(GTK_GRID(grid), scroll_window, 0,0,4,3);
  gtk_grid_attach(GTK_GRID(grid), entry,0,3,4,1); 
  
  /* Attach the grid to the window */
  gtk_container_add(GTK_CONTAINER(window), grid);

  g_signal_new("message-recvd",
	       G_TYPE_OBJECT,
	       G_SIGNAL_RUN_FIRST,
	       0,NULL,NULL,
	       g_cclosure_marshal_VOID__POINTER,
	       G_TYPE_NONE, 1, G_TYPE_POINTER);
	       

  
  pthread_create(thread, NULL, recv_loop, (void*) args); 
  g_signal_connect(G_OBJECT(vertical_box), "message-recvd",
		   G_CALLBACK(cb_update_char_box), buffer);
  
  gtk_widget_show_all(window);
  gtk_main();
}

int main(int argc, char * argv[]){
  
  int sockfd = irc_init("185.30.166.37");
  GtkApplication * app = gtk_application_new("id.ccd", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(G_APPLICATION(app), "activate", G_CALLBACK(activate), &sockfd);
  int status = g_application_run(G_APPLICATION(app), argc, argv);
      
  g_object_unref(app);
  
  return status;
}

// IRC receive thread main loop
static void* recv_loop(void* args){

  /* char * buffer = ((char**) args)[1]; */
  /* int * sockfd_address = ((int**) args)[0]; // ONLY ADDRESSES are sent as args */
  /* int sockfd = *sockfd_address; */
  for(int label_count = 0; ; ++label_count){b
  /*   /\* Wait to receive *\/ */
    printf("|");
  /*   irc_recv(sockfd, buffer, LABEL_BUFFER_MAX); */
  /*   /\* Then send signal *\/ */   
  }
  
  return NULL;
}

// Callback function for clicking the send button
static void cb_send_button_clicked(GtkWidget* button, gpointer callback_data){
  
  
}

static void cb_update_char_box(GtkWidget * verical_box,
			       gpointer callback_data){
  
}

static void cb_thread_quit(GtkWidget* window,
			   gpointer callback_data){
  pthread_t * thread = callback_data;
  pthread_cancel(*thread);
  gtk_main_quit();
}
