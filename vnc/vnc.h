/**
 * xrdp: A Remote Desktop Protocol server.
 *
 * Copyright (C) Jay Sorg 2004-2015
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * libvnc
 */

#ifndef VNC_H
#define VNC_H

/* include other h files */
#include "arch.h"
#include "parse.h"
#include "os_calls.h"
#include "defines.h"

#define CURRENT_MOD_VER 4

/* Screen used for ExtendedDesktopSize / Set DesktopSize */
struct vnc_screen
{
    int id;
    int x;
    int y;
    int width;
    int height;
    int flags;
};

struct vnc_screen_layout
{
    int total_width;
    int total_height;
    unsigned int count;
    /* For comparison, screens are sorted in increasing order of ID */
    struct vnc_screen *s;
};

/**
 * Keep track of resize status at start of connection
 */
enum vnc_resize_status
{
    VRS_WAITING_FOR_FIRST_UPDATE,
    VRS_WAITING_FOR_RESIZE_CONFIRM,
    VRS_DONE
};

struct source_info;

/* Defined in vnc_clip.c */
struct vnc_clipboard_data;

struct vnc
{
    int size; /* size of this struct */
    int version; /* internal version */
    /* client functions */
    int (*mod_start)(struct vnc *v, int w, int h, int bpp);
    int (*mod_connect)(struct vnc *v);
    int (*mod_event)(struct vnc *v, int msg, long param1, long param2,
                     long param3, long param4);
    int (*mod_signal)(struct vnc *v);
    int (*mod_end)(struct vnc *v);
    int (*mod_set_param)(struct vnc *v, const char *name, const char *value);
    int (*mod_session_change)(struct vnc *v, int, int);
    int (*mod_get_wait_objs)(struct vnc *v, tbus *read_objs, int *rcount,
                             tbus *write_objs, int *wcount, int *timeout);
    int (*mod_check_wait_objs)(struct vnc *v);
    int (*mod_frame_ack)(struct vnc *v, int flags, int frame_id);
    int (*mod_suppress_output)(struct vnc *v, int suppress,
                               int left, int top, int right, int bottom);
    int (*mod_server_monitor_resize)(struct vnc *v,
                                     int width, int height);
    int (*mod_server_monitor_full_invalidate)(struct vnc *v,
            int width, int height);
    int (*mod_server_version_message)(struct vnc *v);
    tintptr mod_dumby[100 - 14]; /* align, 100 minus the number of mod
                                  functions above */
    /* server functions */
    int (*server_begin_update)(struct vnc *v);
    int (*server_end_update)(struct vnc *v);
    int (*server_fill_rect)(struct vnc *v, int x, int y, int cx, int cy);
    int (*server_screen_blt)(struct vnc *v, int x, int y, int cx, int cy,
                             int srcx, int srcy);
    int (*server_paint_rect)(struct vnc *v, int x, int y, int cx, int cy,
                             char *data, int width, int height, int srcx, int srcy);
    int (*server_set_cursor)(struct vnc *v, int x, int y, char *data, char *mask);
    int (*server_palette)(struct vnc *v, int *palette);
    int (*server_msg)(struct vnc *v, const char *msg, int code);
    int (*server_is_term)(struct vnc *v);
    int (*server_set_clip)(struct vnc *v, int x, int y, int cx, int cy);
    int (*server_reset_clip)(struct vnc *v);
    int (*server_set_fgcolor)(struct vnc *v, int fgcolor);
    int (*server_set_bgcolor)(struct vnc *v, int bgcolor);
    int (*server_set_opcode)(struct vnc *v, int opcode);
    int (*server_set_mixmode)(struct vnc *v, int mixmode);
    int (*server_set_brush)(struct vnc *v, int x_origin, int y_origin,
                            int style, char *pattern);
    int (*server_set_pen)(struct vnc *v, int style,
                          int width);
    int (*server_draw_line)(struct vnc *v, int x1, int y1, int x2, int y2);
    int (*server_add_char)(struct vnc *v, int font, int character,
                           int offset, int baseline,
                           int width, int height, char *data);
    int (*server_draw_text)(struct vnc *v, int font,
                            int flags, int mixmode, int clip_left, int clip_top,
                            int clip_right, int clip_bottom,
                            int box_left, int box_top,
                            int box_right, int box_bottom,
                            int x, int y, char *data, int data_len);
    int (*server_reset)(struct vnc *v, int width, int height, int bpp);
    int (*server_get_channel_count)(struct vnc *v);
    int (*server_query_channel)(struct vnc *v, int index,
                                char *channel_name,
                                int *channel_flags);
    int (*server_get_channel_id)(struct vnc *v, const char *name);
    int (*server_send_to_channel)(struct vnc *v, int channel_id,
                                  char *data, int data_len,
                                  int total_data_len, int flags);
    int (*server_bell_trigger)(struct vnc *v);
    int (*server_chansrv_in_use)(struct vnc *v);
    tintptr server_dumby[100 - 27]; /* align, 100 minus the number of server
                                     functions above */
    /* common */
    tintptr handle; /* pointer to self as long */
    tintptr wm;
    tintptr painter;
    struct source_info *si;
    /* mod data */
    int server_width;
    int server_height;
    int server_bpp;
    char mod_name[256];
    int mod_mouse_state;
    int palette[256];
    int vnc_desktop;
    char username[256];
    char password[256];
    char ip[256];
    char port[256];
    int sck_closed;
    int shift_state; /* 0 up, 1 down */
    int keylayout;
    int clip_chanid;
    struct vnc_clipboard_data *vc;
    int delay_ms;
    struct trans *trans;
    int got_guid;
    tui8 guid[16];
    int suppress_output;
    unsigned int enabled_encodings_mask;
    /* Resizeable support */
    struct vnc_screen_layout client_layout;
    enum vnc_resize_status resize_status;
};

/*
 * Functions
 */
int
lib_send_copy(struct vnc *v, struct stream *s);
int
skip_trans_bytes(struct trans *trans, unsigned int bytes);

#endif /* VNC_H */
