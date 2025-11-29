#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

    static GtkWidget *entry1;  // text field : Priority
    static GtkWidget *entry2;  // text field : Priority
    static GtkWidget *list_box; // Task List
    static GtkWidget *status_combo; // Multiple choice button : Task Status


    typedef struct task
    {
        int ID;
        char Desc[100];
        int Prio;
        char Status[15];
        struct task* next;
    }Task;

// Close Window
static void on_window_destroy(GtkWidget *widget, gpointer data);

// Generate Unique ID for each Task
int GenerateUniqueID(Task *L);

// Update the List Box after Deletion or Addition
static void Update(Task *L);

// Adding a Task To The List
static void TaskInsertion(GtkWidget *widget, gpointer data);

// Delete a Task
static void TaskDeletion(GtkWidget *widget, gpointer data);

// Display The List SORTED
static void TaskDisplay(GtkWidget *widget, gpointer data);

// Update The Status of A Task
static void UpdateStatus(GtkWidget *widget, gpointer data);

// Search a Task with his Priority
static void SearchByPriority(GtkWidget *widget, gpointer data);

int main(int argc, char *argv[]){

    Task *M = NULL;

    // Initialisation GTK
    if (!gtk_init_check(&argc,&argv)) {
        g_print("Échec de l'initialisation de GTK.\n");
        return 1;
    }

    // Window Creation
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);  
    gtk_window_set_title(GTK_WINDOW(window), "Gestionnaire de tâches");
    gtk_window_set_default_size(GTK_WINDOW(window), 1000, 1000);
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);


    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Text Field (Description, Priority)
    entry1 = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry1), "Task Description ");
    gtk_box_pack_start(GTK_BOX(vbox), entry1, FALSE, FALSE, 5);

    entry2 = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry2), "Priority (1-5) Add/Search");
    gtk_box_pack_start(GTK_BOX(vbox), entry2, FALSE, FALSE, 5);


    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);


    // Button Creation
    GtkWidget *button1 = gtk_button_new_with_label("Add Task");
    gtk_box_pack_start(GTK_BOX(hbox), button1, TRUE, TRUE, 5);

    GtkWidget *button2 = gtk_button_new_with_label("Delete Task");
    gtk_box_pack_start(GTK_BOX(hbox), button2, TRUE, TRUE, 5);

    GtkWidget *button3 = gtk_button_new_with_label("Update");
    gtk_box_pack_start(GTK_BOX(hbox), button3, TRUE, TRUE, 5);

    // Multiple choice Button
    status_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(status_combo), NULL, "Pending");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(status_combo), NULL, "Completed");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(status_combo), NULL, "In Progress");
    gtk_box_pack_start(GTK_BOX(vbox), status_combo, FALSE, FALSE, 5);


    GtkWidget *button4 = gtk_button_new_with_label("Display (Sorted)");
    gtk_box_pack_start(GTK_BOX(hbox), button4, TRUE, TRUE, 5); 
    
    GtkWidget *button5 = gtk_button_new_with_label("Search");
    gtk_box_pack_start(GTK_BOX(hbox), button5, TRUE, TRUE, 5);

    // List Box (wheres Tasks are Displayed)
    list_box = gtk_list_box_new();
    gtk_box_pack_start(GTK_BOX(vbox), list_box, TRUE, TRUE, 5);


    // Link Buttons and Function
    g_signal_connect(button1, "clicked", G_CALLBACK(TaskInsertion), &M);
    g_signal_connect(button2, "clicked", G_CALLBACK(TaskDeletion), &M);
    g_signal_connect(button3, "clicked", G_CALLBACK(UpdateStatus), &M);
    g_signal_connect(button4, "clicked", G_CALLBACK(TaskDisplay), &M);
    g_signal_connect(button5, "clicked", G_CALLBACK(SearchByPriority), &M);

    // Link CSS with GTK 
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "C:/Users/Dell/Dev/ACAD-B-1-4/Projet-P1/styles.css",NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),GTK_STYLE_PROVIDER(provider),GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_widget_show_all(window); // show all widget

    gtk_main(); // main Loop for the window

    return 0;
}


static void Update(Task *L){

    GList *children = gtk_container_get_children(GTK_CONTAINER(list_box)); // Delete all element from the list box

    for (GList *i = children; i != NULL; i = g_list_next(i)) {
        gtk_widget_destroy(GTK_WIDGET(i->data));
    }
    g_list_free(children);

    Task *P = L;
    while (P != NULL) { // Adding the new Tasks to the List Box
        char task_info[200];
        snprintf(task_info, sizeof(task_info), "ID: %d | Description: %s | Priority: %d | Status: %s", P->ID, P->Desc, P->Prio, P->Status);

        GtkWidget *label = gtk_label_new(task_info);
        gtk_list_box_insert(GTK_LIST_BOX(list_box), label, -1);
        P = P->next;
    }

    gtk_widget_show_all(list_box); // Update
}

int GenerateUniqueID(Task *L){

    Task *temp;
    bool isUnique;
    int num;

    do{
        num = rand() % 10000;
        isUnique = true;
        temp = L;

        while(temp !=NULL && isUnique){

            if(temp->ID == num){
                isUnique = false;
            }
            temp = temp->next;
        }

    }while(!isUnique);

    return num;
}

static void TaskInsertion(GtkWidget *widget, gpointer data){

    Task **L = (Task **)data;
    Task *H = *L;
    Task *task = malloc(sizeof(Task));

    const char *desc = gtk_entry_get_text(GTK_ENTRY(entry1));
    const char *PrioText = gtk_entry_get_text(GTK_ENTRY(entry2));

    // Non-Valid Value (Pop-Up)
    if(desc == NULL || strlen(desc) == 0){

        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(gtk_widget_get_toplevel(widget)),GTK_DIALOG_MODAL,GTK_MESSAGE_WARNING,GTK_BUTTONS_OK,"Add Task Description");

        gtk_entry_set_text(GTK_ENTRY(entry1), ""); // Reset Texte Field
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        free(task); 
        return; 
    }

    int i = atoi(PrioText);

    if(strlen(PrioText) == 0 || i < 1 || i > 5){

        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(gtk_widget_get_toplevel(widget)),GTK_DIALOG_MODAL,GTK_MESSAGE_WARNING,GTK_BUTTONS_OK,"Add Task Priority");

        gtk_entry_set_text(GTK_ENTRY(entry2), "");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        free(task); 

        return; 
    }

    task->Prio = i; //Priority
    snprintf(task->Desc, sizeof(task->Desc), "%s", desc); //description
    task->next = NULL; 
    task->ID = GenerateUniqueID(*L); // ID
    snprintf(task->Status, sizeof(task->Status), "Pending"); // Status, default : Pending


    // Adding The Task in Decreasing Order
    if (*L == NULL || (*L)->Prio < task->Prio){
        task->next = *L;
        *L = task;
    } 
    else{
        while (H->next != NULL && (H->next)->Prio >= task->Prio){
            H = H->next;
        }
        task->next = H->next;
        H->next = task;
    }

    Update(*L);

    // Reset Text Field
    gtk_entry_set_text(GTK_ENTRY(entry1), "");
    gtk_entry_set_text(GTK_ENTRY(entry2), "");

    return;
}

static void TaskDeletion(GtkWidget *widget, gpointer data) {
    Task **L = (Task **)data;


    GtkListBoxRow *selected_row = gtk_list_box_get_selected_row(GTK_LIST_BOX(list_box)); // Get The Task from the Selected Row
    if (!selected_row){
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(gtk_widget_get_toplevel(widget)), GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, "No task selected to delete.");

        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    GtkWidget *label = gtk_bin_get_child(GTK_BIN(selected_row)); 
    const char *task_info = gtk_label_get_text(GTK_LABEL(label));

    if (!task_info) {
        printf("Erreur : impossible de récupérer le texte de la ligne sélectionnée.\n");
        return;
    }


    int i;
    sscanf(task_info, "ID: %d", &i); // Take the value after "ID : "

    // Delete the Task with the ID == i
    Task *P = *L;
    Task *Q = NULL;

    while(P != NULL && P->ID != i){
        Q = P;
        P = P->next;
    }

    if(P == NULL){
        printf("Task Not Found\n"); //impossible Case (Normalement)
        return;
    }

    if(Q == NULL){
        *L = P->next; 
    } 
    else{
        Q->next = P->next;
    }
    free(P);

    gtk_widget_destroy(GTK_WIDGET(selected_row)); //Delete From the List Box

    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(gtk_widget_get_toplevel(widget)),GTK_DIALOG_MODAL,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,"Task Deleted Successfully.");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    return;
}

static void UpdateStatus(GtkWidget *widget, gpointer data){

    Task **L = (Task **)data;

    // Same with DeleteTask
    GtkListBoxRow *selected_row = gtk_list_box_get_selected_row(GTK_LIST_BOX(list_box));
    if (!selected_row){
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(gtk_widget_get_toplevel(widget)), GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, "No task selected to Modify his Status.");

        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    GtkWidget *label = gtk_bin_get_child(GTK_BIN(selected_row));
    const char *task_info = gtk_label_get_text(GTK_LABEL(label));

    if (!task_info) {
        printf("Erreur : impossible de récupérer le texte de la ligne sélectionnée.\n");
        return;
    }

    int task_id;
    sscanf(task_info, "ID: %d", &task_id);

    // Read The value from a Multiple Choice input
    const char *new_status = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(status_combo));
    if (new_status == NULL) {

        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(gtk_widget_get_toplevel(widget)),GTK_DIALOG_MODAL,GTK_MESSAGE_WARNING,GTK_BUTTONS_OK,"Please select a valid status.");

        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    //find the Task with the selected ID
    Task *P = *L;
    while (P != NULL && P->ID != task_id) {
        P = P->next;
    }

    if (P == NULL) {
        printf("Erreur : tâche non trouvée dans la liste chaînée.\n");
        return;
    }

    snprintf(P->Status, sizeof(P->Status), "%s", new_status); // Modify his status

    Update(*L);
}

static void SearchByPriority(GtkWidget *widget, gpointer data){

    Task **L = (Task **) data;

    //get the input from the text field
    const char *PrioText = gtk_entry_get_text(GTK_ENTRY(entry2));
    int i = atoi(PrioText);

    if(strlen(PrioText) == 0 || i < 1 || i > 5){

        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(gtk_widget_get_toplevel(widget)),GTK_DIALOG_MODAL,GTK_MESSAGE_WARNING,GTK_BUTTONS_OK,"Add Task Priority");

        gtk_entry_set_text(GTK_ENTRY(entry2), "");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);

        return; 
    }

    //delete all Tasks from the lists Box
    GList *children = gtk_container_get_children(GTK_CONTAINER(list_box));
    for (GList *j = children; j != NULL; j = g_list_next(j)) {
        gtk_widget_destroy(GTK_WIDGET(j->data));
    }
    g_list_free(children);

    // Take the Tasks of the list box and reorder it with the searched Priority
    Task *P = *L;
    while(P != NULL){

        char task_info[200];
        snprintf(task_info, sizeof(task_info), "ID: %d | Description: %s | Priority: %d | Status: %s",P->ID, P->Desc, P->Prio, P->Status);

        GtkWidget *label = gtk_label_new(task_info); // add the string to the label
        GtkWidget *row = gtk_list_box_row_new(); // creating new row for the list box
        gtk_container_add(GTK_CONTAINER(row), label); //merge 

        //apply the CSS
        GtkStyleContext *context = gtk_widget_get_style_context(row);

            if (P->Prio == i) {
                gtk_widget_set_name(row, "searched");
            }
            else{
                gtk_widget_set_name(row, "not-searched");
            }

        // if P->Prio == i then add it to row 0; else add it to row -1 
        if (P->Prio == i) {
            gtk_list_box_insert(GTK_LIST_BOX(list_box), row, 0);
        } else {
            gtk_list_box_insert(GTK_LIST_BOX(list_box), row, -1);
        }

        P = P->next;
    }
    gtk_widget_show_all(list_box);

    return;
}

static void TaskDisplay(GtkWidget *widget, gpointer data){

    Task **L = (Task **)data;

    if(L != NULL){
    GList *children = gtk_container_get_children(GTK_CONTAINER(list_box));

    for (GList *i = children; i != NULL; i = g_list_next(i)) {
        gtk_widget_destroy(GTK_WIDGET(i->data));
    }

    g_list_free(children);

    //display all tasks
    Task *P = *L;
    while (P != NULL) {
        char task_info[200];
        snprintf(task_info, sizeof(task_info), "ID: %d | Description: %s | Priority: %d | Status: %s", P->ID, P->Desc, P->Prio, P->Status);

        GtkWidget *label = gtk_label_new(task_info);
        gtk_list_box_insert(GTK_LIST_BOX(list_box), label, -1);
        P = P->next;
    }

    gtk_widget_show_all(list_box); // Update 

    }
}


static void on_window_destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

/*
Commande :
gcc $(pkg-config --cflags gtk+-3.0) -o main C:/Users/Dell/Dev/ACAD-B-1-4/Projet-P1/main.c $(pkg-config --libs gtk+-3.0)
*/

