
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
    
    char *filename = g_strdup_printf("./Assets/Steps/step%d.png", gtk_spin_button_get_value_as_int(ui->processing_steps) - 1);

    open_file(ui, filename, ui->output_image, 411);
    gtk_widget_set_sensitive(GTK_WIDGET(ui->processing_steps), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(ui->save_button), TRUE);
