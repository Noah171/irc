#include <gtk/gtk.h>
#include "irc.h"

/*
  TODO

  o Scroll window adds a scroll bar to an element, it's not its own separate element. SO
      vertical_box will be the new chat recieving widget, which will be the child of 
      scroll_window, which is a scroll window and will therefore have a scroll bar.

 */

#define WINDOWHEIGHT 960
#define WINDOWWIDTH 480

static void activate(GtkApplication* app,
		   gpointer user_data){
  GtkWidget * window;
  GtkWidget * scroll_window;
  GtkWidget * grid;
  GtkWidget * button;
  GtkWidget * label[5];
  GtkWidget * vertical_box;
  
  // Initialize the first window of the GTK application
  grid = gtk_grid_new();
  button = gtk_button_new_with_label("Send");
  window = gtk_application_window_new(app);
  scroll_window = gtk_scrolled_window_new(NULL,NULL);
  vertical_box = gtk_vertical_box;
  for(int i = 0; i < 5; ++i)
    label[i] = gtk_label_new("adsfdasfasdf");
    
  gtk_window_set_title(GTK_WINDOW(window), "Window");
  gtk_window_set_default_size(GTK_WINDOW(window), WINDOWHEIGHT,WINDOWWIDTH);
  gtk_window_set_resizable(GTK_WINDOW(window),0);

  gtk_grid_set_column_homogeneous(GTK_GRID(grid), 1);
  gtk_grid_set_row_homogeneous(GTK_GRID(grid), 1);

  /* Sets up the window's grid */
  gtk_grid_attach(GTK_GRID(grid), button, 4,3,1,1);
  gtk_grid_attach(GTK_GRID(grid), scroll_window, 0,0,4,3);

  for(int i = 0; i < 5; ++i)
    gtk_container_add(GTK_CONTAINER(scroll_window), label[i]);
  
  gtk_container_add(GTK_CONTAINER(window), grid);
  
  gtk_widget_show_all(window);
}

int main(int argc, char * argv[]){
  
  GtkApplication* app;
  int status = 0;

  // Make new application with id id.noah, the final argument is for special purposes,
  // and for my case is therefore none.
  app = gtk_application_new("id.noah", G_APPLICATION_FLAGS_NONE);
  
  // Connects 'activate' as the callback to the application.
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);


  g_object_unref(app);
  return status;
  
}
