#include "VTserver.h"

static GtkWidget  *vbox;
static GstElement *play;
static GstElement *videosink;

int md_gst_is_playing(void)
{
    return (gst_element_get_state(play) == GST_STATE_PLAYING) ? 1 : 0;
}

static void cb_eos (GstElement *play, gpointer data)
{
    gst_element_set_state(play, GST_STATE_NULL);
}

    static void
cb_error (GstElement *play,
        GstElement *src,
        GError     *err,
        gchar      *debug,
        gpointer    data)
{
    g_print ("Error: %s\n", err->message);
}

gint md_gst_play(char *uri)
{
    g_return_val_if_fail(uri, -1);
    g_object_set(G_OBJECT(play), "uri", uri, NULL);
    if(GST_IS_ELEMENT(play))
        gst_element_set_state(play, GST_STATE_PLAYING);
    return 0;
}

gint md_gst_play_loop(char *uri)
{
    g_return_val_if_fail(uri, -1);
    g_object_set(G_OBJECT(play), "uri", uri, NULL);
    if(GST_IS_ELEMENT(play))
        gst_element_set_state(play, GST_STATE_PLAYING);

    //while(gst_element_get_state(play) == GST_STATE_PLAYING && !done_playing) usleep(800000);
    while(!md_gst_is_playing()) usleep(400000);
    return 0;
}

gint md_gst_finish(void)
{
    gst_object_unref(GST_OBJECT(videosink));
    gst_object_unref(GST_OBJECT(play));
    g_object_unref(vbox);
    return 0;
}

/*
 * Calls gst-register-0.8
 *
 * I know glib already has its own pipes API,
 * but I don't care, as I trust POSIX.
 */
static int md_gst_register(void)
{
    gchar  cmd[] = "/usr/bin/gst-register-0.8";
    gchar  buffer[512];
    FILE *fp;

    g_printerr("Assuming gst-register-0.8 to be on /usr/bin, which is the default on "
            "Debian systems\n");

    if((fp = popen(cmd, "r")) == NULL) {
        g_printerr("popen() failed: %s\n", strerror(errno));
        return 1;
    }

    /* read from pipe until reach EOF */
    while(!feof(fp)) {
        memset(&buffer, 0, sizeof (buffer));
        fgets(buffer, sizeof (buffer), fp);
        if((strncmp(buffer, "Loaded", 6)) == 0) {
            g_printerr("Successfully registered the available plugins.\n");
            g_printerr("%s\n", buffer);
        }
    }

    /* close pipe fd */
    pclose(fp);
    return 0;
}

/* XXX remember to g_object_unref() what must be freed XXX */
gint md_gst_init(gint argc, gchar **argv, GtkWidget *win)
{
    /* init GStreamer */
    gst_init (&argc, &argv);

    /* make sure we have a URI */
    if (argc < 2) {
        g_print ("Usage: %s <URI>\n", argv[0]);
        return -1;
    }

    /* set up */
    play = gst_element_factory_make ("playbin", "play");
    if(!GST_IS_ELEMENT(play)) {
        g_printerr("gst_element_factory_make() failed, you probably don't have runned "
                "gst-register-0.8 yet.\n"
                "I'm going to try it for you.\n");
        if(md_gst_register()) {
            g_printerr("Sorry, it didn't work out. Please check your gstreamer installation.\n");
            gst_main_quit();
            return -1;
        } else {
            return 1;
        }
    }
    g_signal_connect (play, "eos", G_CALLBACK (cb_eos), NULL);
    g_signal_connect (play, "error", G_CALLBACK (cb_error), NULL);

    g_printerr("\nXXXXXXXX WARNING: don't forget to change videosink to xvimagesink XXXXXXXXXXXXXXX\n\n");
    videosink = gst_element_factory_make("xvimagesink", "videosink");
    g_object_set(G_OBJECT(play), "video-sink", videosink, NULL);

    /* XXX choose what widget to put the video into. XXX
       vbox=gtk_vbox_new(TRUE,5);
     */
    vbox = gst_player_video_new(videosink,play); 

    if(!vbox) {
        g_printerr("gst_player_video_new() failed, aborting.\n");
        exit(EXIT_FAILURE);
    }
    /* XXX I have no idea on how to change the video size. If needed, might adding a filter
     * be a good idea ? XXX */
    gtk_container_add(GTK_CONTAINER(win), vbox);

    //g_object_set(G_OBJECT(play), "uri", argv[1], NULL);
    gst_element_set_state(play, GST_STATE_PAUSED);

    gtk_widget_show(vbox);
    return 0;
}
