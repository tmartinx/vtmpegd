#include "VTserver.h"

static GtkWidget  *vbox;
static GstElement *play;
static GstElement *videosink;

static void cb_eos (GstElement *play, gpointer data)
{
    g_print("CB_EOS !!!\n");
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

    while(gst_element_get_state(play) == GST_STATE_PLAYING) usleep(800000);
    return 0;
}

gint md_gst_finish(void)
{
    gst_object_unref(GST_OBJECT(videosink));
    gst_object_unref(GST_OBJECT(play));
    g_object_unref(vbox);
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
    g_signal_connect (play, "eos", G_CALLBACK (cb_eos), NULL);
    g_signal_connect (play, "error", G_CALLBACK (cb_error), NULL);

    videosink = gst_element_factory_make("ximagesink", "videosink");
    g_object_set(G_OBJECT(play), "video-sink", videosink, NULL);

    /* XXX choose what widget to put the video into. XXX
    vbox=gtk_vbox_new(TRUE,5);
    */
    vbox = gst_player_video_new(videosink,play); 
    /* XXX I have no idea on how to change the video size. If needed, might adding a filter
     * be a good idea ? XXX */
    gtk_container_add(GTK_CONTAINER(win), vbox);

    //g_object_set(G_OBJECT(play), "uri", argv[1], NULL);
    gst_element_set_state(play, GST_STATE_PAUSED);

    gtk_widget_show(vbox);
    return 0;
}
