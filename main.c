#include <gtk/gtk.h>
#include "irc.h"
#include <pthread.h>

#define WINDOWHEIGHT 960
#define WINDOWWIDTH 480
#define LABEL_BUFFER_MAX 1000

/* Begin callback declarations */
static void
cb_send_button_clicked(GtkWidget *button,
		       gpointer callback_data);
/* End callback declaration */
static void *
recv_loop(void* args);
static gboolean
update_box(gpointer sick_data);
int
main(int argc, char * argv[]){
  
  int sockfd = irc_init("185.30.166.37"); // This is freenode's ip
  
  GtkWidget * window;
  GtkWidget * scroll_window;
  GtkWidget * grid;
  GtkWidget * button;
  GtkWidget * vertical_box;
  GtkWidget * entry;
  
  size_t buff_len = LABEL_BUFFER_MAX * sizeof(char);
  void * socket_thread_arguments = malloc(sizeof(void*) * 4);
  void * cbargs_button = malloc(sizeof(void*) * 4);
  const char * buff_msg = "Enter message ...";
  
  gtk_init(&argc, &argv);
        
  // Initialize the first window of the GTK application
  grid = gtk_grid_new();
  button = gtk_button_new_with_label("Send");
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  scroll_window = gtk_scrolled_window_new(NULL, NULL);
  vertical_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5); // Make it a vertical box with
  // 5 pixels of spacing
  entry = gtk_entry_new(); // Text entry box
  gtk_entry_set_text(GTK_ENTRY(entry), buff_msg);
  /* Set up the window */
  gtk_window_set_title(GTK_WINDOW(window), "DeliveryClient");
  gtk_window_set_default_size(GTK_WINDOW(window),
			      WINDOWHEIGHT,WINDOWWIDTH);
  
  gtk_window_set_resizable(GTK_WINDOW(window),0);
  g_signal_connect(G_OBJECT(window), "destroy",
		   G_CALLBACK(gtk_main_quit), NULL);

  /* Sets up the window's grid */
  gtk_grid_set_column_homogeneous(GTK_GRID(grid), 1);
  gtk_grid_set_row_homogeneous(GTK_GRID(grid), 1);

  ((int **)cbargs_button)[0] = &sockfd;
  ((GtkWidget **) cbargs_button)[1] = entry;
  ((GtkWidget **) cbargs_button)[2] = vertical_box;
  ((GtkWidget **) cbargs_button)[3] = window;
    
  /* connects the send_button_clicked to the button */
  g_signal_connect(G_OBJECT(button), "clicked",
		   G_CALLBACK(cb_send_button_clicked),
		   cbargs_button);

  /* Make the vertical box a scroll window */
  gtk_container_add(GTK_CONTAINER(scroll_window), vertical_box);
  /* Attach children to parent grid, order of numbers is
     <LeftDistance, TopDistance, Width, Height */
  gtk_grid_attach(GTK_GRID(grid), button, 4,3,1,1);
  gtk_grid_attach(GTK_GRID(grid), scroll_window, 0,0,4,3);
  gtk_grid_attach(GTK_GRID(grid), entry,0,3,4,1); 
  
  /* Attach the grid to the window */
  gtk_container_add(GTK_CONTAINER(window), grid);
  
  /* connects the previously declared signal to the updating function */

  ((int**) socket_thread_arguments)[0] = &sockfd; // Sends a pointer to the socketfd
  ((GtkWidget **) socket_thread_arguments)[1] = vertical_box;
  ((size_t *) socket_thread_arguments)[2] = buff_len;
  ((GtkWidget **) socket_thread_arguments)[3] = window;
  
  g_thread_new("thr_socket", recv_loop, socket_thread_arguments);
	       
  gtk_widget_show_all(window);
  gtk_main();

  free(socket_thread_arguments);
  free(cbargs_button);
  close(sockfd);
  return 0;
}

// IRC receive thread main loop
static void *
recv_loop (void* args) {

  int sockfd = *((int **) args)[0]; // ((int **) args)[0] is a pointer to sockfd, so
  // dereference as such
  GtkWidget * vertical_box = ((GtkWidget **) args)[1];
  size_t buff_len = ((size_t *) args)[2];
  GtkWidget * window = ((GtkWidget **) args)[3];
  
  char * send_buff = malloc(buff_len);
  for(int label_count = 0; ; ++label_count){
    char * buffer= malloc(buff_len);
    void * args = malloc(sizeof(void *) * 3); // THIS IS FREED in update_box
    /* Wait to receive */
    irc_recv(sockfd, buffer, LABEL_BUFFER_MAX);


    ((GtkWidget **) args)[0] = vertical_box;
    strncpy( send_buff, buffer, buff_len);
    ((char **) args)[1] = send_buff;
    ((GtkWidget **) args)[2] = window;
    
    g_main_context_invoke (NULL, update_box, args);
    
    free(buffer);
  } 
  free(send_buff);
  return NULL;
}

static gboolean
update_box(gpointer sick_data){
  
  GtkWidget * v_box = ((GtkWidget **)sick_data)[0];
  char * str = ((char **) sick_data)[1];
  GtkWidget * window = ((GtkWidget **) sick_data)[2];
  
  GtkWidget * label;
    
  label = gtk_label_new(str);
  
  gtk_container_add(GTK_CONTAINER(v_box), label);
  gtk_widget_show_all(window);
  
  // THE DATA MUST BE FREE'd HERE because while multithreading, there is no guarentee
  // that the data will be free'd before it is used here
  free(sick_data);
  return 0;
}

// Callback function for clicking the send button
static void
cb_send_button_clicked(GtkWidget * button, gpointer callback_data){
  
  int sockfd = *((int **) callback_data)[0];
  GtkWidget *entry = ((GtkWidget **) callback_data)[1];
  GtkWidget *vertical_box = ((GtkWidget **) callback_data)[2];
  GtkWidget *window = ((GtkWidget **) callback_data)[3];
  void * args = malloc(sizeof(void *) * 3);
  
  char * send_str = (char *) gtk_entry_get_text(GTK_ENTRY(entry));
  
  irc_send(sockfd, send_str);
  
  ((GtkWidget **) args)[0] = vertical_box;
  ((char **) args)[1] = send_str;
  ((GtkWidget **) args)[2] = window;
    
  g_main_context_invoke (NULL, update_box, args);
  
  gtk_entry_set_text(GTK_ENTRY(entry), "\0");
}
