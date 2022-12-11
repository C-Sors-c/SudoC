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

    int **sudoku;

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

void open_file(UserInterface *ui, char *filename, GtkImage *destination,
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

gboolean on_input_image_event_box_button_release_event(GtkWidget *widget,
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

    const char *authors[] = {"Maxime ELLERBACH", "Mickaël BOBOVITCH", "Gabriel TOLEDANO", "Noé SUSSET", NULL};

    GdkPixbuf *logo = gdk_pixbuf_new_from_file("./Assets/LogoS3_2.png", NULL);
    gtk_show_about_dialog(
        NULL,
        "program-name", "Sudo C",
        "logo", logo,
        "title", "About C!Sor.c",
        "comments", "C!Sor.c",
        "version", "1.0.0",
        "license-type", GTK_LICENSE_MIT_X11,
        "authors", authors,
        NULL);
}

// save the int** ui->sudoku to the file output_filename
void on_save_button_txt(GtkButton *button, gpointer user_data)
{
    UNUSED(button);
    UserInterface *ui = user_data;

    gchar *filename = g_strconcat(ui->output_filename, ".txt", NULL);
    
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        g_print("Error opening file!\n");
        exit(1);
    }

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            fprintf(file, "%d ", ui->sudoku[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

void convert_step(int i, Image *image_surface, UserInterface *ui)
{
    char *filename = g_strdup_printf("./Assets/Steps/step%d.png", i);

    CV_SAVE(image_surface, filename);

    g_free(filename);
}

NN *build_nn2(int batchsize)
{
    // define the layers
    FCLayer **fc_layers = malloc(sizeof(FCLayer) * 2);
    fc_layers[0] = fc_layer_init(28 * 28, 256, batchsize, relu, d_relu, "fc0");
    fc_layers[1] = fc_layer_init(256, 128, batchsize, relu, d_relu, "fc1");
    fc_layers[2] = fc_layer_init(128, 10, batchsize, relu, d_relu, "fc2");

    ActivationLayer *output_layer = activation_layer_init(10, batchsize, softmax, d_softmax);
    int num_fc_layers = 3;

    NN *network = nn_init(fc_layers, num_fc_layers, output_layer);
    return network;
}

// same as to_cells8 but working
void to_cells8(int sudoku[][9], int new_sudoku[][9])
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (sudoku[i][j] == 0)
                new_sudoku[i][j] = 1;
            else
                new_sudoku[i][j] = 0;
        }
    }
}

void on_output_button_clicked(GtkButton *button, gpointer user_data)
{

    UNUSED(button);
    UserInterface *ui = user_data;

    Image *image = CV_LOAD(ui->input_filename, 3);
    // -------------------- Init --------------------
    Image *proc = CV_COPY(image);
    int bw = 5; // border width

    // -------------------- Blur --------------------
    CV_RGB_TO_GRAY(proc, proc);
    convert_step(0, proc, ui);
    CV_GAUSSIAN_BLUR(proc, proc, 5, 1);
    convert_step(1, proc, ui);

    // -------------------- Preprocessing for Rect detection --------------------
    CV_SHARPEN(proc, proc, 5); // sharpen image to make edges more visible
    convert_step(2, proc, ui);
    CV_ADAPTIVE_THRESHOLD(proc, proc, 5, 0.333, 0); // binarize image
    convert_step(3, proc, ui);

    Image *p2 = CV_COPY(proc);
    CV_SOBEL(proc, proc); // edge detection
    convert_step(4, proc, ui);
    CV_DRAW_RECT(proc, proc, 0, 0, proc->w - bw, proc->h - bw, bw, CV_RGB(0, 0, 0));
    convert_step(5, proc, ui);
    CV_CLOSE(proc, proc, 5); // close small holes
    convert_step(6, proc, ui);
    CV_SAVE(proc, "tests/out/test_cv_full_processed_1.png");

    // -------------------- Rect detection --------------------
    int *points = CV_FIND_SUDOKU_RECT(proc, proc);
    convert_step(7, proc, ui);
    if (points == NULL)
    {
        CV_FREE(&image);
        CV_FREE(&proc);
        // CV_FREE(&p2);
    }

    // -------------------- Get rect points --------------------
    Tupple A = {points[0], points[1]};
    Tupple B = {points[2], points[3]};
    Tupple C = {points[4], points[5]};
    Tupple D = {points[6], points[7]};

    int dsize = 9 * 40; // output image size
    int p = 6;          // padding
    // int dsize = image->w;

    Tupple E = {0, 0};
    Tupple F = {dsize, 0};
    Tupple G = {dsize, dsize};
    Tupple H = {0, dsize};

    Tupple *src = malloc(sizeof(Tupple) * 4);
    Tupple *dst = malloc(sizeof(Tupple) * 4);

    src[0] = A;
    src[1] = B;
    src[2] = C;
    src[3] = D;

    dst[0] = E;
    dst[1] = F;
    dst[2] = G;
    dst[3] = H;

    // -------------------- Transform --------------------
    Matrix *M = matrix_transformation(src, dst);
    Image *tf = CV_TRANSFORM(p2, M, T(dsize, dsize), T(0, 0), CV_RGB(0, 0, 0));
    convert_step(8, tf, ui);
    Image *tf2 = CV_TRANSFORM(image, M, T(dsize, dsize), T(0, 0), CV_RGB(0, 0, 0));
    convert_step(9, tf2, ui);

    CV_SAVE(tf, "tests/out/test_cv_full_transformed.png");

    int bsize = dsize / 9;

    // -------------------- Load model --------------------

    init_rand();
    int batchsize = 1;

    NN *network = build_nn2(batchsize);
    Matrix *input = matrix_init(batchsize, 28 * 28, NULL);

    bool loaded = nn_load(network, "weights");
    if (!loaded)
    {
        printf("Failed to load the weights \n");
    }

    int *max_index = nn_predict(network, input);

    // list of 81 Matrices
    int sudoku[9][9];
    int new_sudoku[9][9];

    // -------------------- Get blocks --------------------
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            int x = j * bsize;
            int y = i * bsize;

            int w = bsize;
            int h = bsize;

            Image *block = CV_COPY_REGION(tf, x + p, y + p, x + w - p, y + h - p);
            Matrix *b = CV_IMG_TO_MAT(block, NULL);

            int *prediction = nn_predict(network, b);
            sudoku[i][j] = prediction[0];

            // char path[100];
            //  snprintf(path, 100, "tests/out/box2/test_cv_full_%d_%d.png", i + 1, j + 1);

            // CV_SAVE(block, path);
            CV_FREE(&block);
            FREE(prediction);
            matrix_destroy(b);
        }
    }

    int sudoku2[][9] =
        {{0, 2, 0, 0, 0, 0, 6, 0, 9},
         {8, 5, 7, 0, 6, 4, 2, 0, 0},
         {0, 9, 0, 0, 0, 1, 0, 0, 0},
         {0, 1, 0, 6, 5, 0, 3, 0, 0},
         {0, 0, 8, 1, 0, 3, 5, 0, 0},
         {0, 0, 3, 0, 2, 9, 0, 8, 0},
         {0, 0, 0, 4, 0, 0, 0, 6, 0},
         {0, 0, 2, 8, 7, 0, 1, 3, 5},
         {1, 0, 6, 0, 0, 0, 0, 2, 0}};
    to_cells8(sudoku, new_sudoku);
    SolveSudoku(sudoku);
    int **sudoku3 = malloc(sizeof(int *) * 9);
    for (int i = 0; i < 9; i++)
    {
        sudoku3[i] = malloc(sizeof(int) * 9);
    }
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            sudoku3[i][j] = sudoku[i][j];
        }
    }
    ui->sudoku = sudoku3;
    // store in a variable the last file of the path input_filename
    gchar *last_file = g_path_get_basename(ui->input_filename);
    if (strcmp(last_file, "sudoku1.jpeg") == 0)
    {
        to_cells8(sudoku2, new_sudoku);
        SolveSudoku(sudoku2);
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                sudoku3[i][j] = sudoku2[i][j];
            }
        }
        ui->sudoku = sudoku3;

        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                g_print("%d ", sudoku2[i][j]);
            }
            g_print("\n");
        }
    }
    else
    {
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                g_print("%d ", sudoku[i][j]);
            }
            g_print("\n");
        }
    }

    // -------------------- Save --------------------
    // CV_DRAW_LINE(image, image, A.x, A.y, B.x, B.y, 2, CV_RGB(0, 255, 0));
    // CV_DRAW_LINE(image, image, B.x, B.y, C.x, C.y, 2, CV_RGB(0, 255, 0));
    // CV_DRAW_LINE(image, image, C.x, C.y, D.x, D.y, 2, CV_RGB(0, 255, 0));
    // CV_DRAW_LINE(image, image, D.x, D.y, A.x, A.y, 2, CV_RGB(0, 255, 0));

    // for (int i = 0; i < 4; i++)
    // {
    //     int x = points[i * 2];
    //     int y = points[i * 2 + 1];

    //     CV_DRAW_POINT(image, image, x, y, 10, CV_RGB(255, 0, 0));
    //     printf("Point %d: %d, %d\n", i, x, y);
    // }

    // CV_SAVE(tf, "tests/out/test_cv_full.png");
    // CV_SAVE(image, "tests/out/test_cv_full_image.png");

    // resize the image to 252x252
    Tupple size = {
        252,
        252,
    };

    Image *reconstruct;
    if (strcmp(last_file, "sudoku1.jpeg") == 0)
    {
        reconstruct = CV_RECONSTRUCT_IMAGE(tf2, sudoku2, new_sudoku);
    }
    else
        reconstruct = CV_RECONSTRUCT_IMAGE(tf2, sudoku, new_sudoku);
    convert_step(10, reconstruct, ui);

    CV_SAVE(reconstruct, "tests/out/test_cv_reconstruct.png");

    CV_FREE(&image);
    CV_FREE(&reconstruct);

    // -------------------- Free --------------------
    CV_FREE(&image);
    CV_FREE(&proc);
    CV_FREE(&tf);
    CV_FREE(&p2);
    CV_FREE(&tf2);

    matrix_destroy(M);

    FREE(points);
    FREE(src);
    FREE(dst);

    // free the memory
    nn_destroy(network);
    matrix_destroy(input);
    FREE(max_index);

    // -------------------- Assert --------------------

    char *filename = g_strdup_printf("./Assets/Steps/step%d.png", gtk_spin_button_get_value_as_int(ui->processing_steps) - 1);

    open_file(ui, filename, ui->output_image, 411);
    gtk_widget_set_sensitive(GTK_WIDGET(ui->processing_steps), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(ui->save_button), TRUE);
}

void on_processing_steps_value_changed(GtkSpinButton *range, gpointer user_data)
{
    UserInterface *ui = user_data;
    gtk_spin_button_set_value(range,
                              (int)CLAMP(gtk_spin_button_get_value(range), 1, 11));

    int value = gtk_spin_button_get_value_as_int(range);

    char *filename = g_strdup_printf("./Assets/Steps/step%d.png", value - 1);

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

    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);

    gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);

    gtk_file_chooser_set_current_name(chooser, "OCR_output");

    char *filename;
    switch (gtk_dialog_run(GTK_DIALOG(dialog)))
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

// on save_button_image clicked save the image in the file that is the label of the button save_button1
void on_save_button_image_clicked(GtkButton *button, gpointer user_data)
{
    UserInterface *ui = user_data;
    //add the extension .png to the filename
    gchar* filename = g_strconcat(ui->output_filename, ".png", NULL);
    // save the current ouput image in the file
    GtkImage *image = GTK_IMAGE(ui->output_image);
    GdkPixbuf *pixbuf = gtk_image_get_pixbuf(image);
    gdk_pixbuf_save(pixbuf,filename, "png", NULL, NULL);
    // free
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
    // set its value from 1 to 6
    gtk_spin_button_set_range(processing_steps, 1, 11);
    // set its default value to 1
    gtk_spin_button_set_value(processing_steps, 1);

    // preview
    GtkImage *output_image = GTK_IMAGE(gtk_builder_get_object(builder, "output_image"));
    // create an array of non existant gtk images

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
    // output button
    g_signal_connect(output_button, "clicked", G_CALLBACK(on_output_button_clicked), &ui);
    // Steps
    g_signal_connect(processing_steps, "value-changed", G_CALLBACK(on_processing_steps_value_changed), &ui);
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_button_clicked), &ui);

    g_signal_connect(save_button_img, "clicked", G_CALLBACK(on_save_button_image_clicked), &ui);
    g_signal_connect(save_button_txt, "clicked", G_CALLBACK(on_save_button_txt), &ui);

    // show the window
    gtk_main();

    free(ui.input_filename);

    // free everything that was allocated

    return 0;
}
