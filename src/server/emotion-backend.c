/*
 * Copyright (C) 2001 Void Technologies
 * Copyright (C) 2005,2006 Alexandre Fiori and Arnaldo Pereira
 *
 * VTmpeg2 is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * VTmpeg2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "VTserver.h"

Ecore_Evas  *ee;
Evas        *evas;
Evas_Object *bg;
Evas_Object *text;
Evas_Object *v;
Evas_Object *vv = NULL;
Evas_Object *rect = NULL;
Evas_Object *img  = NULL;

static int pp_width = 300;
static int pp_height = 150;

static int pp_watch(void *data);
static int pp_init(char *uri);
static void pp_play();

static void rest_image_show(char *image)
{
    if(!image)
        return;

    img = evas_object_image_add(evas);
    evas_object_image_file_set(img, image, NULL);
    evas_object_image_size_set(img, DEF_WIDTH, DEF_HEIGHT);
    evas_object_move(img, 0, 0);
    evas_object_resize(img, DEF_WIDTH, DEF_HEIGHT);
    evas_object_image_fill_set(img, 0, 0, DEF_WIDTH, DEF_HEIGHT);
    evas_object_show(img);

    return;
}

static void rest_image_hide(void) { evas_object_hide(img); }

static void draw_dummy_rect(void)
{
    static int hidden = 0;

    if(rect && (hidden = !hidden)) {
        evas_object_hide(rect);
        return;
    }

    rect = evas_object_rectangle_add(evas);
    //evas_object_move(rect, 0, 0);
    evas_object_resize(rect, DEF_WIDTH, DEF_HEIGHT);
    evas_object_color_set(rect, 222, 33, 33, 80);
    evas_object_layer_set(rect, 2);
    evas_object_show(rect);

#if 0
    printf("RECT\n\n");
    {
        Evas_Object *text;
        text = evas_object_text_add(evas);
        evas_object_clip_set(text, rect);
        evas_object_text_font_set(text, "Vera", 28);
        evas_object_text_text_set(text, "XXX");
        //evas_object_move(text, 0, 0);
        evas_object_layer_set(text, 5);
        evas_object_color_set(text, 100, 100, 100, 255);
        evas_object_show(text);
        printf("TEXT\n\n");
    }
#endif
}

static void cb_pp_mousewheel(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
    Evas_Event_Mouse_Wheel *ev;
    int w, h;

    evas_object_geometry_get(obj, NULL, NULL, &w, &h);

    ev = (Evas_Event_Mouse_Wheel *) event_info;
    /* down */
    if(ev->z < 0) {
        w -= 5;
        h -= 5;
        evas_object_resize(obj, w, h);

        /* up */
    } else {
        w += 5;
        h += 5;
        evas_object_resize(obj, w, h);
    }
}

static void cb_mousewheel(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
    Evas_Event_Mouse_Wheel *ev;
    int w, h;

    evas_object_geometry_get(v, NULL, NULL, &w, &h);

    ev = (Evas_Event_Mouse_Wheel *) event_info;
    /* down */
    if(ev->z < 0) {
        w -= 5;
        h -= 5;
        evas_object_resize(v, w, h);

        /* up */
    } else {
        w += 5;
        h += 5;
        evas_object_resize(v, w, h);
    }
}

static void cb_mouseup(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
    Evas_Event_Mouse_Up *ev;

    ev = (Evas_Event_Mouse_Up *) event_info;
    if(ev->button == 3)
        puts("show menu ?");
}

static int fade_in(void *data)
{
    static int a = 255;
    evas_object_color_set(rect, 0, 0, 0, (a-=10));
    if(a <= 0) {
        a = 255;
        return 0;
    }
    return 1;
}

static int fade_out(void *data)
{
    static int a = 0;
    evas_object_color_set(rect, 0, 0, 0, (a+=10));
    if(a >= 255) {
        a = 0;
        return 0;
    }
    return 1;
}

static void pp_make_transparent(void)
{
    static int s = 0;

    if(!vv)
        return;

    if(s)
        evas_object_color_set(vv, 255, 255, 255, 255);
    else
        evas_object_color_set(vv, 255, 255, 255, 128);

    s = !s;
}

static void cb_keydown(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
    Evas_Event_Key_Down *ev;
    double sec;
    int    fs;

    ev = (Evas_Event_Key_Down *) event_info;
    if(!strcmp(ev->keyname, "Escape") || !strcmp(ev->keyname, "q"))
        ecore_main_loop_quit();
    else if(!strcmp(ev->keyname, "f")) {
        fs = ecore_evas_fullscreen_get(ee);
        ecore_evas_fullscreen_set(ee, !fs);
    } else if(!strcmp(ev->keyname, "a")) {
        draw_dummy_rect();
    } else if(!strcmp(ev->keyname, "p")) {
        pp_play();
    } else if(!strcmp(ev->keyname, "e")) {
        pp_make_transparent();
    } else if(!strcmp(ev->keyname, "s")) { 
        ecore_timer_add(0.5, fade_in, NULL);
    } else if(!strcmp(ev->keyname, "d")) {
        ecore_timer_add(0.5, fade_out, NULL);
    } else if(!strcmp(ev->keyname, "m"))
        emotion_object_audio_mute_set(v, !emotion_object_audio_mute_get(v));
    else if(!strcmp(ev->keyname, "p"))
        emotion_object_play_set(v, !emotion_object_play_get(v));
    else if(!strcmp(ev->keyname, "Right")) {
        if(emotion_object_seekable_get(v)) {
            sec = emotion_object_position_get(v);
            emotion_object_position_set(v, sec+40);
        } else
            fprintf(stderr, "Movie not seekable.\n");

    } else if(!strcmp(ev->keyname, "Left")) {
        if(emotion_object_seekable_get(v)) {
            sec = emotion_object_position_get(v);
            emotion_object_position_set(v, sec-40);
        } else
            fprintf(stderr, "Movie not seekable.\n");
    } else if(!strcmp(ev->keyname, "Up")) {
        if(emotion_object_seekable_get(v)) {
            sec = emotion_object_position_get(v);
            emotion_object_position_set(v, sec+400);
        } else
            fprintf(stderr, "Movie not seekable.\n");
    } else if(!strcmp(ev->keyname, "Down")) {
        if(emotion_object_seekable_get(v)) {
            sec = emotion_object_position_get(v);
            emotion_object_position_set(v, sec-400);
        } else
            fprintf(stderr, "Movie not seekable.\n");
    }
}

static void cb_resize(Ecore_Evas *ee)
{
    Evas_Object *bg;
    int w, h;

    /* get the geometry of ee. we don't need the x,y coords so we send NULL. */
    ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);

    /* find our bg object and resize it to the window size (if it exists) */
    bg = evas_object_name_find(ecore_evas_get(ee), "video");
    if(bg) evas_object_resize(bg, w, h);
}

int video_init(char *display, char *win_title, int width, int height, int fs)
{
    int r;

    /* initialize our libraries */
    evas_init();
    ecore_init();
    ecore_evas_init();

    /* create our Ecore_Evas and show it */
    //ee = ecore_evas_directfb_new(0, 0, 0, 0, width, height);
    ee = ecore_evas_software_x11_new(display, 0, 0, 0, width, height);
    ecore_evas_title_set(ee, win_title);
    if(fs)
        ecore_evas_fullscreen_set(ee, 1);
    ecore_evas_show(ee);

    /* get a pointer our new Evas canvas */
    evas = ecore_evas_get(ee);
    v = emotion_object_add(evas);
    r = emotion_object_init(v, "emotion_decoder_xine.so");
    if(!r) {
        fprintf(stderr, "emotion_object_init() failed.\n");
        exit(EXIT_FAILURE);
    }
    //emotion_object_file_set(v, argv[1]);
    evas_object_move(v, 0, 0);
    emotion_object_play_set(v, 0);
    evas_object_resize(v, width, height);
    evas_object_focus_set(v, 1);
    evas_object_name_set(v, "video");
    /* evas_object_geometry_get() */
    emotion_object_smooth_scale_set(v, 1);
    evas_object_show(v);

    pp_init("testdata/pjflipeindhoven.mpg");

    /* callbacks */
    evas_object_event_callback_add(v, EVAS_CALLBACK_KEY_DOWN,    cb_keydown,    NULL);
    evas_object_event_callback_add(v, EVAS_CALLBACK_MOUSE_UP,    cb_mouseup,    NULL);
    evas_object_event_callback_add(v, EVAS_CALLBACK_MOUSE_WHEEL, cb_mousewheel, NULL);

    ecore_evas_callback_resize_set(ee, cb_resize);
    /* FIXME: where may we get the image path from ? */
    rest_image_show("../../images/logo.jpg");
    return 0;
}

int video_play(char *filename)
{
    emotion_object_file_set(v, filename);
    rest_image_hide();
    emotion_object_play_set(v, 1);

    return 0;
}

void video_stop(void) { emotion_object_play_set(v, 0); }

int video_is_playing(void) { return emotion_object_play_get(v); }

void video_close(void)
{
    /* when the main event loop exits, shutdown our libraries */
    ecore_evas_shutdown();
    ecore_shutdown();
    evas_shutdown();
}

static int pp_watch(void *data)
{
    Evas_Object *vv = data;

    if(!emotion_object_play_get(vv)) {
        evas_object_hide(vv);
        emotion_object_position_set(vv, 0.0);
        return 0;
    }
    return 1;
}

static int pp_init(char *uri)
{
    int r;

    if(!uri)
        return -1;

    vv = emotion_object_add(evas);
    r = emotion_object_init(vv, "emotion_decoder_xine.so");
    if(!r) {
        fprintf(stderr, "emotion_object_init() failed.\n");
        exit(EXIT_FAILURE);
    }
    evas_object_move(vv, 200, 40);
    emotion_object_play_set(vv, 0);
    emotion_object_file_set(vv, uri);
    evas_object_resize(vv, pp_width, pp_height);
    evas_object_name_set(vv, "ppvideo");
    emotion_object_smooth_scale_set(vv, 1);
    emotion_object_play_set(vv, 0);
    //rest_image_hide();

    evas_object_event_callback_add(vv, EVAS_CALLBACK_MOUSE_WHEEL, cb_pp_mousewheel, NULL);

    return 0;
}

/* picture in picture test */
static void pp_play()
{
    if(emotion_object_play_get(vv)) {
        evas_object_hide(vv);
        emotion_object_play_set(vv, 0);
        return;
    }
    evas_object_show(vv);
    emotion_object_play_set(vv, 1);
    ecore_timer_add(0.5, pp_watch, vv);
}
