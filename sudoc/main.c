#include "include/main.h"

#define COUCOU(x) g_print("coucou %i\n", x);
#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

typedef struct BannerMenu
{
    GtkMenuBar *menu;
    GtkMenuItem *open;
    GtkMenuItem *quit;
    GtkMenuItem *about;
} BannerMenu;

typedef struct UserInterface
{
    // Neural network
    CNN *net;

    // Main top-level window
    GtkWindow *window;

    // Top menu
    BannerMenu banner_menu;

    // Input image
    char *input_filename;
    GtkEventBox *input_image_event_box;
    GtkImage *input_image;

    // Buttons /////////////////////////////
    //// Preview
    GtkComboBox *preview_interpolation_menu;
    GdkInterpType interp_type;


    //// Output
    GtkButton *save_button_img;
    GtkButton *save_button_txt;
    GtkButton *output_button;
    GtkButton *save_button;
    GtkSpinButton *processing_steps;
    char *output_filename;
    ////////////////////////////////////////

    // Input
    GtkFileFilter *file_filter;

    // Output
    GtkImage *output_image;
    GtkImage **processing_images;
} UserInterface;

/*
SDL_Surface* Resize(SDL_Surface *img)
{
    SDL_Surface *dest =
        SDL_CreateRGBSurface(SDL_HWSURFACE,28,28,img->format->BitsPerPixel,\
                0,0,0,0);
    SDL_SoftStretch(img, NULL, dest, NULL);
    return dest;
}
*/

/*
SDL_Surface* redImage(int w,int h,SDL_Surface* src)
{
    SDL_Surface* ret =
        SDL_CreateRGBSurface(src->flags,w,h,src->format->BitsPerPixel,\
                src->format->Rmask, src->format->Gmask, src->format->Bmask,\
                src->format->Amask);
    if (!ret)
        return src;
    SDL_BlitSurface(src,NULL,ret,NULL);
    SDL_FreeSurface(src);
    SDL_Surface* surface = SDL_DisplayFormatAlpha(ret);
    SDL_FreeSurface(ret);
    return surface;
}
*/

void resize_to_fit(UserInterface *ui, GtkImage *image, int size)
{
    // Resize image to fit
    const GdkPixbuf *pb = gtk_image_get_pixbuf(image);
    // g_print("%s\n", (pb == NULL ? "NULL" : "NOT NULL"));
    const int imgW = gdk_pixbuf_get_width(pb);
    const int imgH = gdk_pixbuf_get_height(pb);

    double ratio;
    int destW;
    int destH;

    if (imgW > imgH)
        ratio = size / (double)imgW;
    else
        ratio = size / (double)imgH;

    destW = ratio * imgW;
    destH = ratio * imgH;

    GdkPixbuf *result =
        gdk_pixbuf_scale_simple(pb, destW, destH, ui->interp_type);

    gtk_image_set_from_pixbuf(image, result);
}

void open_file(UserInterface *ui, char *filename, GtkImage *destination,\
               int size)
{
    gtk_image_set_from_file(destination, filename);
    resize_to_fit(ui, destination, size);
}

void run_file_opener(UserInterface *ui)
{
    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Open File", ui->window,
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Open", GTK_RESPONSE_ACCEPT,
        NULL);
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pixbuf_formats(filter);
    gtk_file_filter_set_name(filter, "Images (.png/.jpg/.jpeg/etc...)");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    char *filename;
    switch (gtk_dialog_run(GTK_DIALOG(dialog)))
    {
    case GTK_RESPONSE_ACCEPT:
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        open_file(ui, filename, ui->input_image, 411);
        ui->input_filename = filename;

        break;
    default:
        break;
    }

    gtk_widget_destroy(dialog);
}

void on_open_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    UNUSED(menuitem);
    UserInterface *ui = user_data;

    run_file_opener(ui);
}

gboolean on_input_image_event_box_button_release_event(GtkWidget *widget,\
                                                       GdkEvent *event, gpointer user_data)
{
    UNUSED(widget);
    UNUSED(event);
    UserInterface *ui = user_data;

    run_file_opener(ui);
    gtk_widget_set_sensitive(GTK_WIDGET(ui->output_button), TRUE);

    return TRUE;
}


void on_about_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    UNUSED(menuitem);
    UNUSED(user_data);

    const char *authors[] = {"Maxime ELLERBACH", "Mickaël BOBOVITCH", "Gabriel TOLEDANO","Noé SUSSET", NULL};

            
    GdkPixbuf *logo = gdk_pixbuf_new_from_file("./Assets/LogoS3_2.png", NULL);
    gtk_show_about_dialog(\
            NULL,\
            "program-name", "Sudo C",\
            "logo", logo,\
            "title", "About C!Sor.c",\
            "comments", "C!Sor.c",
            "version", "1.0.0",
            "license-type", GTK_LICENSE_MIT_X11,
            "authors", authors,
            NULL);


}




void convert_step(int i, Image *image_surface, UserInterface *ui)
{
    char *filename = g_strdup_printf("./Assets/steps/step%d.png", i);
    CV_SAVE(image_surface, filename);

    g_free(filename);
}


void on_output_button_clicked(GtkButton *button, gpointer user_data)
{
    UNUSED(button);
    UserInterface *ui = user_data;

    Image *image_surface = CV_LOAD(ui->input_filename, 3);

    // convert to grayscale
    CV_RGB_TO_GRAY(image_surface, image_surface);
    convert_step(0, image_surface, ui);

    // apply gaussian blur
    CV_GAUSSIAN_BLUR(image_surface, image_surface, 5, 1);
    convert_step(1, image_surface, ui);

    // apply sharpen
    CV_SHARPEN(image_surface, image_surface, 15);
    convert_step(2, image_surface, ui);

    // apply adaptive threshold
    CV_ADAPTIVE_THRESHOLD(image_surface, image_surface, 5, 1.0/3.0, 0);
    convert_step(3, image_surface, ui);

    // aplly sobel
    CV_SOBEL(image_surface, image_surface);
    convert_step(4, image_surface, ui);

    // apply dilate
    CV_DILATE(image_surface, image_surface, 3);
    //apply erode
    CV_ERODE(image_surface, image_surface, 3);
    convert_step(5, image_surface, ui);
    
    char *filename = g_strdup_printf("./Assets/steps/step%d.png", gtk_spin_button_get_value_as_int(ui->processing_steps) - 1);
    open_file(ui, filename, ui->output_image, 411);
    gtk_widget_set_sensitive(GTK_WIDGET(ui->processing_steps), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(ui->save_button), TRUE);

}

void on_processing_steps_value_changed(GtkSpinButton *range, gpointer user_data)
{
    UserInterface *ui = user_data;
    gtk_spin_button_set_value(range,\
            (int)CLAMP(gtk_spin_button_get_value(range), 1, 6));

    int value = gtk_spin_button_get_value_as_int(range);

    char *filename = g_strdup_printf("./Assets/steps/step%d.png", value - 1);

    open_file(ui, filename, ui->output_image, 411);
    g_free(filename);

}


void on_save_button_clicked(GtkButton *button, gpointer user_data)
{
    UserInterface *ui = user_data;

    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
            "Select File", ui->window, action,
            "Cancel", GTK_RESPONSE_CANCEL,
            "Select", GTK_RESPONSE_ACCEPT,
            NULL);

    GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);

    gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);

    gtk_file_chooser_set_current_name(chooser, "OCR_output.png");

    char *filename;
    switch(gtk_dialog_run(GTK_DIALOG(dialog)))
    {
        case GTK_RESPONSE_ACCEPT:
            filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

            gtk_button_set_label(button, filename);
            ui->output_filename = filename;
            break;
        default:
            break;
    }
    gtk_widget_set_sensitive(GTK_WIDGET(ui->save_button_txt), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(ui->save_button_img), TRUE);

    gtk_widget_destroy(dialog);
}

//on save_button_image clicked save the image in the file that is the label of the button save_button1
void on_save_button_image_clicked(GtkButton *button, gpointer user_data)
{
    UserInterface *ui = user_data;


    //save the current ouput image in the file
    GtkImage *image = GTK_IMAGE(ui->output_image);
    GdkPixbuf *pixbuf = gtk_image_get_pixbuf(image);
    gdk_pixbuf_save(pixbuf, ui->output_filename, "png", NULL, NULL);
    //free
    g_object_unref(pixbuf);
}



int main(int argc, char **argv)
{
    // init gtk
    gtk_init(NULL, NULL);

    // construct the gtk builder
    GtkBuilder *builder = gtk_builder_new();

    // load the ui file (exit if it fails)
    if (!gtk_builder_add_from_file(builder, "./sudoc/SudoC.glade", NULL))
    {
        g_printerr("Error: could not load ui file.\n");
        return 1;
    }

    // get the main window
    GtkWindow *window = GTK_WINDOW(gtk_builder_get_object(builder, "main_window"));

    // get the menu
    GtkMenuBar *menu = GTK_MENU_BAR(gtk_builder_get_object(builder, "menu"));
    GtkMenuItem *open = GTK_MENU_ITEM(gtk_builder_get_object(builder, "open"));
    GtkMenuItem *quit = GTK_MENU_ITEM(gtk_builder_get_object(builder, "quit"));
    GtkMenuItem *about = GTK_MENU_ITEM(gtk_builder_get_object(builder, "about"));

    // input image
    GtkEventBox *input_image_event_box = GTK_EVENT_BOX(gtk_builder_get_object(builder, "input_image_event_box"));
    GtkImage *input_image = GTK_IMAGE(gtk_builder_get_object(builder, "input_image"));

    // buttons
    // output
    GtkButton *save_button = GTK_BUTTON(gtk_builder_get_object(builder, "save_button1"));
    GtkButton *save_button_img = GTK_BUTTON(gtk_builder_get_object(builder, "save_image_button"));
    GtkButton *save_button_txt = GTK_BUTTON(gtk_builder_get_object(builder, "save_text_button"));
    GtkButton *output_button = GTK_BUTTON(gtk_builder_get_object(builder, "output_button"));
    GtkSpinButton *processing_steps = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "processing_steps"));
    // enable the spin button
    //set its value from 1 to 6
    gtk_spin_button_set_range(processing_steps, 1, 6);
    //set its default value to 1
    gtk_spin_button_set_value(processing_steps, 1);



    // preview
    GtkImage *output_image = GTK_IMAGE(gtk_builder_get_object(builder, "output_image"));
    //create an array of non existant gtk images


    UserInterface ui =
        {
            .window = window,
            .banner_menu = {
                .menu = menu,
                .open = open,
                .quit = quit,
                .about = about,
            },
            .input_filename = malloc(256),
            .input_image_event_box = input_image_event_box,
            .input_image = input_image,
            .save_button = save_button,
            .save_button_img = save_button_img,
            .save_button_txt = save_button_txt,
            .output_button = output_button,
            .output_image = output_image,
            .processing_steps = processing_steps,
            .output_filename = malloc(256),
        };

    // connect signals
    gtk_builder_connect_signals(builder, &ui);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(quit, "activate", G_CALLBACK(gtk_main_quit), NULL);

    // Top menu
    g_signal_connect(open, "activate", G_CALLBACK(on_open_activate), &ui);
    g_signal_connect(about, "activate", G_CALLBACK(on_about_activate), &ui);

    // Input image
    g_signal_connect(GTK_WIDGET(input_image_event_box), "button-release-event", G_CALLBACK(on_input_image_event_box_button_release_event), &ui);

    // Buttons
    //output button
    g_signal_connect(output_button, "clicked", G_CALLBACK(on_output_button_clicked), &ui);
    //Steps
    g_signal_connect(processing_steps, "value-changed", G_CALLBACK(on_processing_steps_value_changed), &ui);
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_button_clicked), &ui);

    g_signal_connect(save_button_img, "clicked", G_CALLBACK(on_save_button_image_clicked), &ui);

    // show the window 
    gtk_main();

    free(ui.input_filename);

    // free everything that was allocated

    return 0;
}
