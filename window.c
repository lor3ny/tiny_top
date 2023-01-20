#include <gtk/gtk.h>

//gcc -o `pkg-config gtk+-3.0 --cflags` window.c -o window `pkg-config gtk+-3.0 --libs`
GtkWidget *proc_area;

void printeee(){

    char buffer[1000];
    snprintf(buffer, sizeof(buffer), "PID");


    gtk_label_set_text(GTK_LABEL(proc_area), buffer);
}

int main(int argc, char **argv){

    gtk_init(&argc, &argv);
    GtkWidget *window;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title(GTK_WINDOW(window), "tiny top");
    
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);


    //widgets

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);


    GtkWidget *send_btn = gtk_button_new_with_label("send");
    proc_area = gtk_label_new("");
    GtkWidget *signal_area = gtk_entry_new();
    gtk_widget_set_size_request(send_btn, 10, 30);
    gtk_widget_set_size_request(proc_area, 800, 500);
    gtk_widget_set_size_request(signal_area, 70, 20);

    gtk_grid_attach(GTK_GRID(grid), send_btn, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), proc_area, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), signal_area, 0, 2, 1, 1);

    g_signal_connect(send_btn, "clicked", G_CALLBACK(printeee), NULL);
    
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;

}