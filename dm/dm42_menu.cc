/*

BSD 3-Clause License

Copyright (c) 2015-2021, SwissMicros
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


  The software and related material is released as “NOMAS”  (NOt MAnufacturer Supported). 

  1. Info is released to assist customers using, exploring and extending the product
  2. Do NOT contact the manufacturer with questions, seeking support, etc. regarding
     NOMAS material as no support is implied or committed-to by the Manufacturer
  3. The Manufacturer may reply and/or update materials if and when needed solely
     at their discretion

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core_main.h"
#include "core_aux.h"


extern "C" {

#include <main.h>
#include <dmcp.h>

#include <dm42_menu.h>
#include <dm42_fns.h>


// Possibility to override default
#ifdef printf
//#undef printf
#endif


void after_fat_format_dm42() {
  if ( sys_disk_ok() ) {
    set_fat_label("DM42");
    check_create_dir(PGM_DIR);    // Create /PROGRAMS directory
    check_create_dir(STATE_DIR);  // Create /STATE    directory

    check_create_dir(SCR_DIR);    // Create /SCREENS  directory       
    check_create_dir(OFFIMG_DIR); // Create /OFFIMG   directory      
    check_create_dir(HELP_DIR);   // Create /HELP     directory    
    check_create_dir(PRINT_DIR);  // Create /PRINTS   directory      
  }
}



/*
 ▄    ▄                               ▄▀▀                         ▄      ▀                        
 ██  ██  ▄▄▄   ▄ ▄▄   ▄   ▄         ▄▄█▄▄  ▄   ▄  ▄ ▄▄    ▄▄▄   ▄▄█▄▄  ▄▄▄     ▄▄▄   ▄ ▄▄    ▄▄▄  
 █ ██ █ █▀  █  █▀  █  █   █           █    █   █  █▀  █  █▀  ▀    █      █    █▀ ▀█  █▀  █  █   ▀ 
 █ ▀▀ █ █▀▀▀▀  █   █  █   █           █    █   █  █   █  █        █      █    █   █  █   █   ▀▀▀▄ 
 █    █ ▀█▄▄▀  █   █  ▀▄▄▀█           █    ▀▄▄▀█  █   █  ▀█▄▄▀    ▀▄▄  ▄▄█▄▄  ▀█▄█▀  █   █  ▀▄▄▄▀ */




int lcd_for_dm42(int what) {
  int refresh = 1;

  lcd_clear_buf();
  lcd_writeClr(t24);

  switch ( what ) {
    case DISP_CALC:
      refresh = 0; // Just empty screen
      break;

    case DISP_ALLOC_FAIL:
      lcd_putsR(t24, "Free42 Memory Allocation Fail");
      t24->y += 4;
      lcd_puts(t24, "Unhandled memory allocation:");
      break;

    case DISP_PRBUF_ALLOC_FAIL:
      lcd_putsR(t24, "Print Buffer Fail");
      t24->y += 4;
      lcd_puts(t24, "Cannot prepare buffer for");
      lcd_puts(t24, "printing to file.");
      lcd_puts(t24, "ERROR:");
      break;
      

    case DISP_NO_PGM_SEL:
      lcd_putsR(t24, "Select Programs");
      t24->y += 4;
      lcd_putsAt(t24, 3, "No program selected");
      lcd_putsAt(t24, 7, "Press any key to continue...");
    break;

    case DISP_LOADING_STATE:
      lcd_putsR(t24, "Loading from File");
      lcd_putsAt(t24, 3, "Loading calculator state ...");
      break;

    case DISP_SAVING_STATE:
      lcd_putsR(t24, "Saving to File");
      lcd_putsAt(t24, 3, "Saving calculator state ...");
      break;

    case DISP_SAVE_FAILED:
      lcd_putsR(t24, "Save State Error");
      lcd_putsAt(t24, 2,"Calculator state save failed.");
      lcd_putsAt(t24, 3,"Check the FAT filesystem.");
      lcd_putsAt(t24, 4,"Disk full or too many");
      lcd_putsAt(t24, 5,"root directory items?");
      lcd_putsAt(t24, 7,"    Press EXIT key to continue...");
      break;

    case DISP_NEW_HELP:
      lcd_putsR(t24, "Missing help file");
      lcd_puts(t24, "");
      lcd_puts(t24, "Help file /HELP/dm42help.htm is");
      lcd_puts(t24, "missing. Please, download it from");
      lcd_puts(t24, "https://www.swissmicros.com/dm42/");
      lcd_puts(t24, "fat/HELP/");
      lcd_puts(t24, "and place it into /HELP/ directory.");
      t24->y = LCD_Y; lcd_prevLn(t24);
      lcd_puts(t24, "  Press any key to continue...");
      break;

    case DISP_ABOUT:
      // Just base of original system about
      lcd_for_calc(DISP_ABOUT);
      lcd_putsAt(t24,4,"");
      lcd_prevLn(t24);
      // --
      
      int h2 = lcd_lineHeight(t20)/2;
      lcd_setXY(t20, t24->x, t24->y);
      t20->y += h2;
      lcd_print(t20, "DM42 v" DM42_VERSION " (C) SwissMicros GmbH");
      t20->y += h2;
      lcd_print(t20, "%s (C) 2004-2021, Thomas Okken", free42_version_str());
      t20->y += h2;
      lcd_puts(t20, "Intel Decimal FloatingPointMath Lib v2.0u1");
      lcd_puts(t20, "  (C) 2007-2018, Intel Corp.");

      t20->y = LCD_Y - lcd_lineHeight(t20);
      lcd_putsR(t20, "    Press EXIT key to continue...");
      break;

  }

  if (refresh)
    lcd_refresh();

  return t24->y;
}





#define DM42_HELP_FILE  "/HELP/dm42help.htm"

void start_help() {

  // Check whether DM42_HELP_FILE exists
  if (!file_exists(DM42_HELP_FILE)) {
    if (file_exists(HELP_INDEX)) {
      // Rename old help file to new one
      sys_disk_write_enable(1);
      f_rename(HELP_INDEX, DM42_HELP_FILE);
      sys_disk_write_enable(0);
    } else {
      // No help file -> display note
      lcd_for_dm42(DISP_NEW_HELP);
      wait_for_key_press();
      return;
    }
  }
  run_help_file(DM42_HELP_FILE);
}


/* typedef struct {
  char *buffer;
  int offs;     // buffer position in file
  int pos;      // ftell = buf_offs + pos
  uint last;     // last byte count read (valid bytes in buf)
} buf_t;


#define FORWARD    false
#define BACKWARD   true



char buf_getc(buf_t *buf, bool direction) {
  FRESULT res;
  char c;
  if (direction == FORWARD) {
    if (buf->pos == (int)buf->last) {
      c = EOF;
    } else {
      if (buf->pos == -1) {
        buf->pos = 0;
      }
      c = buf->buffer[buf->pos];
      buf->pos = buf->pos+1;
      if (( (uint)buf->pos == buf->last) && (f_size(ppgm_fp) > (uint) buf->offs+ buf->last)) {
        res = f_lseek(ppgm_fp,buf->offs+AUX_BUF_SIZE);
        if (res == FR_OK) {
          buf->offs = f_tell(ppgm_fp);
          buf->pos = 0;
          res = f_read(ppgm_fp,buf->buffer,AUX_BUF_SIZE,&(buf->last));
        }
      }
    }
  } else {
    if (buf->pos == -1) {
      c = EOF;
    } else {
      if ((uint) buf->pos == buf->last) {
        buf->pos = buf->last -1;
      }
      c = buf->buffer[buf->pos];
      buf->pos = buf->pos-1;
      if ((buf->pos == -1) && (buf->offs > 0)) {
        res = f_lseek(ppgm_fp,buf->offs-AUX_BUF_SIZE);
        if (res == FR_OK) {
          buf->offs = f_tell(ppgm_fp);
          buf->pos = 0;
          res = f_read(ppgm_fp,buf->buffer,AUX_BUF_SIZE,&(buf->last));
        }
      }
    }
  }
  return c;
}

FRESULT buf_setbuf(buf_t *buf, int offs, int pos) {
  FRESULT res = FR_OK;
  if (buf->offs != offs) {
    buf->offs = offs;
    res = f_lseek(ppgm_fp,buf->offs);
    res = f_read(ppgm_fp,buf->buffer,AUX_BUF_SIZE,&(buf->last));
  }
  buf->pos=pos;
  return res;
}


char *buf_getline(char *str, buf_t *buf) {
  int k=0;
  char *res = str;

  do {
    str[k++] = buf_getc(buf,FORWARD);
  } while (str[k]!='\n' || str[k]!=EOF || k<MAX_LN_LEN);
  
  if (str[0] == EOF) {
    res = NULL;
  } 
  
  while (k<MAX_LN_LEN) {
    str[k++]= ' ';
  }
  return res;
}

int buf_setline(buf_t *buf, bool direction) {
  char c;
  int res = 0;
  int k = 0;
  int refln_pos  = buf->pos;
  int refln_offs = buf->offs;

  do {
    k++;
    c = buf_getc(buf,direction);
  } while (c!='\n' || buf->pos!=-1 || buf->pos!=(int)buf->last || k<MAX_LN_LEN);

  if (direction == FORWARD) {
    if ((uint)buf->pos==buf->last) {
      buf_setbuf(buf,refln_offs,refln_pos);
      res = -1;
    }
  } else {
    if ((refln_offs == 0) && (refln_pos == 0))
       res = -1;
    if (buf->pos ==-1) {
      buf_setbuf(buf, 0, 0);
    } else {
      c = buf_getc(buf,FORWARD);
      c = buf_getc(buf,FORWARD);
    }
  }
  return res;
} */

#define MAX_LN_LEN 40

bool readline(char *line) {
  char c = '\0';
  unsigned int rd;
  int k = 0;
  bool rtn = true;
  //FRESULT res;
  if (f_eof(ppgm_fp)) {
    line[0] = '\0';
    rtn = false;
  } else {
    while (!f_eof(ppgm_fp) && c != '\n' && k<MAX_LN_LEN) {
      f_read(ppgm_fp,&c,1,&rd);
      line[k++] = c;
    }
    line[--k] = '\0';
  }
  return rtn;
}

bool rewindline() {
  char c ='\0';
  unsigned int rd;
  //FRESULT res;
  bool rtn = true;
  if (f_tell(ppgm_fp) == 0) {
    rtn = false;
  }
  if (f_tell(ppgm_fp)<2) {
    f_lseek(ppgm_fp,0);
  } else {
    while ( c != '\n') {
      f_lseek(ppgm_fp,f_tell(ppgm_fp)-2);
      if (f_tell(ppgm_fp)==0) break;
      f_read(ppgm_fp,&c,1,&rd);
    }
  }
  return rtn;
}


int text_viewer(const char * fpath, const char * fname, void * data) {
  int upd = 1; // Repaint at first pass
  int k1;
  FRESULT res;
  char *ln = aux_buf_ptr();
  FSIZE_t topline_ptr;
  bool bottom_screen = false;
  char hpf[MAX_LN_LEN];

  res = f_open(ppgm_fp, fpath, FA_READ);

  if ( res != FR_OK ) {
    // Print fail
    lcd_puts(t24,"Fail to open."); lcd_refresh();
    lcd_refresh();
    wait_for_key_press();
  } else {
    topline_ptr = f_tell(ppgm_fp);
    // Display title
    lcd_clear_buf();
    lcd_writeClr(t24);
    //lcd_printR(t24,"Print Viewer: %s    %i", s, t);
    lcd_putsR(t24, fname);
    t24->ln_offs = 40;
    #define top_y_lines 40
    int y_lines = LCD_Y - top_y_lines;
    lcd_fillLines(top_y_lines, LCD_EMPTY_VALUE, y_lines);
    int font_lines = y_lines/lcd_lineHeight(t24);
        //int const linelen = (LCD_X + fReg->xoffs)/lcd_fontWidth(fReg); //(LCD_X - fReg->xoffs)/fReg->f->width;
        // From the top
        //fReg->ln_offs = top_y_lines;
        // == Clear main area ==
    for (;;) {
      k1 = runner_get_key(NULL);
      if ( IS_EXIT_KEY(k1) ) break;
      if ( is_menu_auto_off() ) break;
      switch (k1) {
      case KEY_UP:
        if (rewindline()) {
          upd =1;
          topline_ptr = f_tell(ppgm_fp);
          bottom_screen = false;
        }
        // upd = 1;
        // bottom_screen = rewindline();
        
        // if (buf.offs + buf.pos > 0) {
        //   ln[0] = buf_getc(&buf,BACKWARD);
        //   //buf_setline(&buf,BACKWARD);
        //   upd=1;
        //   lcd_putsAt(fReg, 2, ln);
        //   lcd_refresh();
        // }
        break;
      case KEY_DOWN:
        if (!bottom_screen) {
          if (readline(ln)) {
            upd = 1;
            topline_ptr = f_tell(ppgm_fp);
          }
        }

          // res = f_read(ppgm_fp,&c,1,&rd);
          // cd[0] =c;
          // cd[1] ='\0';
          // if (c =='\n') cd[0] = 'h';
          //upd = 1;
          //bottom_screen = readline(ln);
          //topline_ptr = f_tell(ppgm_fp);
          // if (!bottom_screen) {
          //   while (!f_eof(ppgm_fp) || (c=='\n')) {
          //     res = f_read(ppgm_fp,&c,1,&rd);
          //   }
          //   if f_eof(ppgm_fp) {
          //     res = f_lseek(ppgm_fp, topline_ptr);
          //   } else {
          //     topline_ptr = f_tell(ppgm_fp);
          //   }
          // }
        break;
      }

        // Display
      if (upd == 1) {
        upd = 0;
        for (int line=0; line<font_lines; line++) {
          readline(ln);
          lcd_putsAt(t24, line, ln);
        }
        //   ) {
        //     lcd_putsAt(fReg, line, ln);
        //   } else {
        //     lcd_putsAt(fReg, line, "\0");
        //     bottom_screen = true;
        //   }
        // }
          //bottom_screen = readline(ln);
          //lcd_putsAt(fReg, line, ln);
        if (f_eof(ppgm_fp)) bottom_screen = true;
        //lcd_printR(t24,"%s %s", s, t);
        lcd_refresh();
        res = f_lseek(ppgm_fp, topline_ptr);
      }
    }
  }
  res =f_close(ppgm_fp);
  return 0;
}

/* int text_viewer(const char * fpath, const char * fname, void * data) {
  int upd = 1; // Repaint at first pass
  int k1;
  FRESULT res;
  buf_t buf;
  char ln[MAX_LN_LEN];
  int refln_pos  = 0;
  int refln_offs = 0;
  int bottom_screen = 0;

  res = f_open(ppgm_fp, fpath, FA_READ);
  if ( res != FR_OK ) {
    // Print fail
    lcd_puts(t24,"Fail to open."); lcd_refresh();
    lcd_refresh();
    wait_for_key_press();
  } else {
    buf.pos=0;
    buf.offs=0;
    buf.buffer= (char*) aux_buf_ptr();
    res = f_read(ppgm_fp,buf.buffer,AUX_BUF_SIZE,&(buf.last));


    // Display title
    lcd_writeClr(t24);
    lcd_clear_buf();
    lcd_putsR(t24, fname);
    t24->ln_offs = 8;
    #define top_y_lines 40
    int y_lines = LCD_Y - top_y_lines;
    lcd_fillLines(top_y_lines, LCD_EMPTY_VALUE, y_lines);
        //int font_lines = y_lines/lcd_lineHeight(fReg);
        //int const linelen = (LCD_X + fReg->xoffs)/lcd_fontWidth(fReg); //(LCD_X - fReg->xoffs)/fReg->f->width;
        // From the top
        //fReg->ln_offs = top_y_lines;
        // == Clear main area ==
    for (;;) {
      k1 = runner_get_key(NULL);
      if ( IS_EXIT_KEY(k1) ) break;
      if ( is_menu_auto_off() ) break;
      switch (k1) {
      case KEY_UP:
        if (buf.offs + buf.pos > 0) {
          ln[0] = buf_getc(&buf,BACKWARD);
          //buf_setline(&buf,BACKWARD);
          upd=1;
          lcd_putsAt(fReg, 2, ln);
          lcd_refresh();
        }
        break;
      case KEY_DOWN:
        //if (buf_setline(&buf,FORWARD) != EOF) {
          upd =1;
                    ln[0] = buf_getc(&buf,FORWARD);
          lcd_putsAt(fReg, 2, ln);
          lcd_refresh();
        //}
        break;
      }

      // Display
      if (upd == 1) {
        // upd = 0;
        // char sampletext[5] = "hola" ;
        // lcd_putsAt(fReg, 2, sampletext);
        // lcd_refresh();
      }
    }
  }
  return 0;
} */

/* int text_viewer(const char * fpath, const char * fname, void * data) {
  FRESULT res;
  buf_t buf;
  char ln[MAX_LN_LEN];
  int refln_pos  = 0;
  int refln_offs = 0;
  int last_screen = 0;
  int upd = 1; // Repaint at first pass
  int k1;
  res = f_open(ppgm_fp, fpath, FA_READ);
  if ( res != FR_OK ) {
    // Print fail
    lcd_puts(t24,"Fail to open."); lcd_refresh();
    lcd_refresh();
    wait_for_key_press();
  } else {
    buf.pos=0;
    buf.offs=0;
    buf.buffer=aux_buf_ptr();
    res = f_read(ppgm_fp,(&buf)->buffer,AUX_BUF_SIZE,&(buf.last));

    if ( res != FR_OK ) {
      lcd_puts(t24,"Fail to read."); lcd_refresh();
      lcd_refresh();
      wait_for_key_press();
    } else {
      // Display title
      lcd_writeClr(t24);
      lcd_clear_buf();
      lcd_putsR(t24, fname);
      t24->ln_offs = 8;
      #define top_y_lines 40
      int y_lines = LCD_Y - top_y_lines;
      int font_lines = y_lines/lcd_lineHeight(fReg);
      //int const linelen = (LCD_X + fReg->xoffs)/lcd_fontWidth(fReg); //(LCD_X - fReg->xoffs)/fReg->f->width;
      // From the top
      fReg->ln_offs = top_y_lines;
      // == Clear main area ==
      lcd_fillLines(top_y_lines, LCD_EMPTY_VALUE, y_lines);
      for (;;) {
        k1 = runner_get_key(NULL);
        if ( IS_EXIT_KEY(k1) ) break;
        if ( is_menu_auto_off() ) break;
        switch (k1) {
          case KEY_UP:
            last_screen = 0;
            if (buf_setline(&buf, BACKWARD)!=EOF) {
              upd=1;
            }
            break;
          case KEY_DOWN:
            if ((last_screen !=1) && (buf_setline(&buf, FORWARD)!=EOF)) {
              upd=1;
            }
            break;
        }

        // Display
        if (upd == 1) {
          upd = 0;
          refln_pos = buf.pos;
          refln_offs = buf.offs;
          for (int line=0; line<font_lines; line++) {
            if (buf_getline(ln, &buf)!=NULL) {
              lcd_putsAt(fReg, line, ln);
            } else {
              last_screen = 1;
          }
        }
        res = buf_setbuf(&buf,refln_offs,refln_pos);
        }
      }
      res = f_close(ppgm_fp);
    }
  }

  return 0;
} */

int delete_file(const char * fpath, const char * fname, void * data) {
  FRESULT res;
  sys_disk_write_enable(1);
  res = f_unlink(fpath);
  sys_disk_write_enable(0);
  if ( res != FR_OK ) {
    // Print fail
    lcd_puts(t24,"Fail to delete."); lcd_refresh();
    lcd_refresh();
    wait_for_key_press();
  } 

  return 0;
}


int pgm_import_enter(const char * fpath, const char * fname, void * data) {
  FRESULT res;

  lcd_puts(t24,"Loading ...");
  lcd_puts(t24, fname);  lcd_refresh();
  res = f_open(ppgm_fp, fpath, FA_READ);
  if ( res != FR_OK ) {
    // Print fail
    lcd_puts(t24,"Fail to open."); lcd_refresh();
    lcd_refresh();
    wait_for_key_press();
  } else {

#ifdef FREE42_PRE_2016_IFC
    core_import_programs(NULL);
#else
    init_sf_buf();
    core_import_programs(0, NULL);
    deinit_sf_buf();
#endif

    f_close(ppgm_fp);
    if (pgm_res) {
      lcd_print(t24, "Fail - %i", pgm_res);
      lcd_refresh();
      wait_for_key_press();
    } else {
      lcd_puts(t24, "Success"); 
      lcd_refresh();
      sys_delay(1500);
    }
  }

  return 0; // Continue in the file list
}


typedef struct {
  int pgm_cnt;
  int pgm_indices[MAX_PGM_SEL];
} pgm_import_sel_t;


int pgm_export_enter(const char * fpath, const char * fname, void * data) {
  
  pgm_import_sel_t * pgm_sel = (pgm_import_sel_t*)data;
  int res;

  lcd_writeClr(t24);
  lcd_clear_buf();
  lcd_putsR(t24,"Program Export");
  t24->ln_offs = 8;

  lcd_putsAt(t24,3,"Saving ...");
  lcd_puts(t24, fpath);
  lcd_refresh();

  sys_disk_write_enable(1);
  res = f_open(ppgm_fp, fpath, FA_WRITE|FA_CREATE_ALWAYS);
  if ( res != FR_OK ) {
    sys_disk_write_enable(0);
    lcd_puts(t24,"Fail to open."); lcd_refresh();
    lcd_refresh();
    if ( sys_disk_ok() )
      wait_for_key_press();
  } else {
#ifdef FREE42_PRE_2016_IFC
    res = core_export_programs(pgm_sel->pgm_cnt, pgm_sel->pgm_indices, NULL);
#else
    init_sf_buf();
    core_export_programs(pgm_sel->pgm_cnt, pgm_sel->pgm_indices, NULL);
    deinit_sf_buf();
    res = 0;
#endif
    f_close(ppgm_fp);
    sys_disk_write_enable(0);
    if (res || pgm_res) {
      lcd_print(t24, "Fail - %i", pgm_res);
      lcd_refresh();
      if ( sys_disk_ok() )
        wait_for_key_press();
    } else {
      lcd_puts(t24,"Success"); 
      lcd_refresh();
      sys_delay(1000);
    }
  }

  return MRET_EXIT;
}


void run_reset_state_file() {
  lcd_writeClr(t24);
  lcd_clear_buf();
  lcd_putsR(t24,"Calculator State");
  t24->ln_offs = 8;

  lcd_puts(t24, "You are about to reset calculator");
  lcd_puts(t24, "state.");
  lcd_puts(t24, "");
  lcd_puts(t24, "WARNING: Current calculator state");
  lcd_puts(t24, "will be lost.");
  lcd_puts(t24, "");
  lcd_puts(t24, "");
  //lcd_puts(t24, "Are you sure to proceed?");
  lcd_puts(t24, "Press [ENTER] to confirm.");
  lcd_refresh();
  
  wait_for_key_release(-1);

  for(;;) {
    int k1 = runner_get_key(NULL);
    if ( IS_EXIT_KEY(k1) )
      return;
    if ( is_menu_auto_off() )
      return;
    if ( k1 == KEY_ENTER )
      break; // Proceed with reset
  }

  // Reset statefile name for next load
  set_reset_state_file("");

  // Reset the system to force new statefile load
  set_reset_magic(NO_SPLASH_MAGIC);
  sys_reset();
}


int load_statefile(const char * fpath, const char * fname, void * data) {

  lcd_puts(t24, "Calculator state file:");
  lcd_puts(t24, fname);

  int res = savestat_check_read(fpath);
  if (res < 0) {
    // Bad file or read/write problem

    lcd_puts(t24, "");
    lcd_puts(t24, "ERROR: Cannot read the file");
    lcd_puts(t24, "Not a state file or filesystem error.");
    lcd_puts(t24, "");
    lcd_puts(t24, "");
    lcd_puts(t24, "Press any key to continue");
    lcd_refresh();

    wait_for_key_press();

    return 0;
  }


  // 'Sure' dialog
  lcd_puts(t24, "");
  lcd_puts(t24, "WARNING: Current calculator state");
  lcd_puts(t24, "will be lost.");
  lcd_puts(t24, "");
  lcd_puts(t24, "");
  //lcd_puts(t24, "Are you sure to load this file?");
  lcd_puts(t24, "Press [ENTER] to confirm.");
  lcd_refresh();
  
  wait_for_key_release(-1);

  for(;;) {
    int k1 = runner_get_key(NULL);
    if ( IS_EXIT_KEY(k1) )
      return 0; // Continue the selection screen
    if ( is_menu_auto_off() )
      return MRET_EXIT; // Leave selection screen
    if ( k1 == KEY_ENTER )
      break; // Proceed with load
  }

  lcd_putsRAt(t24, 6, "  Loading ...");
  lcd_refresh_wait();


  // Store statefile name for next load
  set_reset_state_file(fpath);

  // Reset the system to force new statefile load
  set_reset_magic(NO_SPLASH_MAGIC);
  sys_reset();

  // Never return here...
  return 0;
}



#define RESET_STATE_FILE_SIZE        0x38


int save_statefile(const char * fpath, const char * fname, void * data) {

  lcd_puts(t24,"Saving state ...");

  char ff[RESET_STATE_FILE_SIZE];
  const char *fp = fpath;

  // Display correct short name
  strcpy(ff, fname);
  ff[strlen(fname)-3] = 'f';
  lcd_puts(t24, ff);  lcd_refresh();

  int flen = strlen(fpath);
  if (flen < RESET_STATE_FILE_SIZE && fpath[flen-3] != 'f') {
    // Fix filename: .s42 -> .f42
    strcpy(ff, fpath);
    ff[flen-3] = 'f';

    if (file_exists(fpath)) {
      sys_disk_write_enable(1);
      f_rename(fpath, ff);
      sys_disk_write_enable(0);
    }
    fp = ff;
  }

  // Store the state file name
  set_reset_state_file(fp);

  // Exit with appropriate code to force statefile save
  return MRET_SAVESTATE;
}








STATIC_ASSERT( (AUX_BUF_PGM_LIST_SIZE+AUX_BUF_SELS_SIZE) <= AUX_BUF_SIZE, "(AUX_BUF_PGM_LIST_SIZE+AUX_BUF_SELS_SIZE) <= AUX_BUF_SIZE");


int select_programs(const char * title, int * pgm_indices, int * pgm_cnt) {
  char bb[32];
  int k1;
  int upd = 1; // Repaint at first pass

  // Disp state
  int first_ix = 0, cur_ix = 0, a, ix;
  int pix;  // Program index in display loop
  char * p; // Program name in display loop
  int sel_ln = 0;

  // We need rather big buffer ... use aux memory buffer for that
  const uint pgm_list_size = AUX_BUF_PGM_LIST_SIZE;
  const uint sels_size = AUX_BUF_SELS_SIZE;
  const uint max_disp_pgm_char = 24;
  const int pgm_lines = 7;
  
  char * buf = aux_buf_ptr();
  char * sels = buf + pgm_list_size;

  // Read program list
  int pgm_count = core_list_programs(buf,pgm_list_size);

  // Clear selections
  *pgm_cnt = 0;
  bzero(sels, sels_size);

  // Display title
  lcd_writeClr(t24);
  lcd_clear_buf();
  lcd_putsR(t24,title);
  t24->ln_offs = 8;

  for(;;) {
    k1 = runner_get_key(NULL);
    if ( IS_EXIT_KEY(k1) ) break;
    if ( is_menu_auto_off() ) break;

    switch (k1) {
      case KEY_F1: // A - Sel
      case KEY_F6: // F - Sel
        if ( *pgm_cnt == MAX_PGM_SEL && !sels[cur_ix] ) {
          // Too much selections
          lcd_putsRAt(t24,sel_ln,"Max. nr. of selections is " STR(MAX_PGM_SEL));
          lcd_refresh();
          sys_delay(1000);
        } else {
          sels[cur_ix] = !sels[cur_ix];
          if(sels[cur_ix]) pgm_cnt[0]++; else pgm_cnt[0]--;
        }
        upd=1;
        break;
      case KEY_F2: // B - 
        break;
      case KEY_F3: // C - C-All
        // Clear selections
        *pgm_cnt = 0;
        bzero(sels,sels_size);
        upd=1;
        break;
      case KEY_F4: // D - S-All
        if ( pgm_count > MAX_PGM_SEL ) {
          lcd_putsRAt(t24,sel_ln,"Max. nr. of selections is " STR(MAX_PGM_SEL));
          lcd_refresh();
          sys_delay(1000);
        } else {
          *pgm_cnt = pgm_count;
          for(a=0; a<pgm_count; a++)
            sels[a] = 1;
        }
        upd=1;
        break;
      case KEY_F5: // E - 
        break;

      case KEY_UP:
        if (cur_ix > 0) {
          cur_ix--;
        } else {
          cur_ix = pgm_count-1;
        }
        upd=1;
        break;

      case KEY_DOWN:
        if (cur_ix+1 < pgm_count) {
          cur_ix++;
        } else {
          cur_ix = 0;
        }
        upd=1;
        break;

      case KEY_PAGEUP:
        if (cur_ix == 0) {
          cur_ix = pgm_count-1;
        } else {
          cur_ix--;
          cur_ix -= cur_ix % pgm_lines;
        }
        upd=1;
        break;

      case KEY_PAGEDOWN:
        if ( cur_ix == pgm_count-1) {
          cur_ix = 0;
        } else {
          cur_ix += pgm_lines;
          cur_ix -= cur_ix % pgm_lines;
          if ( cur_ix >= pgm_count )
            cur_ix = pgm_count-1;
          else
            first_ix = cur_ix;
        }
        upd=1;
        break;

      case KEY_ENTER:
        // Fill output data
        ix=0;
        for(a=0; a<pgm_count; a++)
          if( sels[a] )
            pgm_indices[ix++]=a;
        return 0;
    }


    if (upd) {
      upd = 0;

      // Update first_ix
      if (cur_ix < first_ix) first_ix = cur_ix;
      if (cur_ix - first_ix > (pgm_lines-1)) first_ix = cur_ix - (pgm_lines-1);
      if (first_ix + pgm_lines >= pgm_count ) first_ix = pgm_count - pgm_lines;
      if (first_ix < 0) first_ix = 0;

      p = buf; pix = 0; sel_ln = 0;
      for(a=0; a<pgm_lines && pix<pgm_count; pix++,p+=strlen(p)+1) { // Over lines
        if ( pix < first_ix ) continue;
        a++;

        // Selection mark
        strcpy(bb, sel_str(sels[pix]));
        //sprintf(bb+strlen(bb), "%02i", pix); // DEBUG :)
        strcat(bb, " ");
        // Program name (truncated to max_disp_pgm_char)
        if ( strlen(p) > max_disp_pgm_char ) {
          ix = strlen(bb);
          memcpy(bb+ix,p,max_disp_pgm_char);
          strcpy(bb+ix+max_disp_pgm_char, "...");
        } else {
          strcat(bb,p);
        }

        if ( pix == cur_ix ) {
          lcd_putsRAt(t24,a,bb);
          sel_ln = a;
        } else {
          lcd_putsAt(t24,a,bb);
        }

      }

      // DEBUG
      printf("pgm_cnt=%i  sel_ln=%i\n", *pgm_cnt, sel_ln);

      // Menu line
      //t24->y = LCD_Y - lcd_lineHeight(t24);
      //lcd_putsR(t24, " Sel |     |C-All|S-All|      | Sel");
      const char * menu[] = {" Sel","","C-All","S-All","","Sel"};
      lcd_draw_menu_keys(menu);

      // display
      lcd_refresh();
    }
  }
  return 1;
}


void pgm_export() {
  int res;

  // Don't pass through if the power is insufficient  
  if ( power_check_screen() )
    return;

  wait_for_key_release(-1);

  if ( check_create_dir(PGM_DIR) ) {
    lcd_puts(t24,"Cannot access " PGM_DIR); lcd_refresh();
    wait_for_key_press();
    return;
  }

  if ( !sys_disk_ok() ) goto pgm_exp_fail;

  char pgm_fn[11+MAX_PGM_FN_LEN+1];
  pgm_import_sel_t pgm_sel;

  strcpy(pgm_fn,PGM_DIR);
  strcat(pgm_fn,"/");

  res = select_programs("Select Programs", pgm_sel.pgm_indices, &pgm_sel.pgm_cnt);
  if ( res != 0 || pgm_sel.pgm_cnt == 0 ) {
    lcd_for_dm42(DISP_NO_PGM_SEL);
    wait_for_key_press();
    return;
  }
  
  //void file_selection_screen(const char * title, const char * base_dir, const char * ext, file_sel_fn_t sel_fn, int disp_new, void * data)
  file_selection_screen("Program Export Filename", PGM_DIR, PGM_EXT, pgm_export_enter, 1, 1, &pgm_sel); 

pgm_exp_fail:
  sys_disk_check_valid(); // Unmount drive if disk is invalid
  if ( !sys_disk_ok() ) {
    disp_disk_info("Program Export");
    wait_for_key_press();
  }
}










/*
 ▄    ▄                                                    ▄                 
 ██  ██  ▄▄▄   ▄ ▄▄   ▄   ▄          ▄▄▄   ▄   ▄   ▄▄▄   ▄▄█▄▄   ▄▄▄   ▄▄▄▄▄ 
 █ ██ █ █▀  █  █▀  █  █   █         █   ▀  ▀▄ ▄▀  █   ▀    █    █▀  █  █ █ █ 
 █ ▀▀ █ █▀▀▀▀  █   █  █   █          ▀▀▀▄   █▄█    ▀▀▀▄    █    █▀▀▀▀  █ █ █ 
 █    █ ▀█▄▄▀  █   █  ▀▄▄▀█         ▀▄▄▄▀   ▀█    ▀▄▄▄▀    ▀▄▄  ▀█▄▄▀  █ █ █ 
                                            ▄▀                               
                                           ▀▀                                */




const uint8_t mid_menu[] = {
    MI_FILE,
    MI_STATEFILE,
    MI_PRINT_MENU,
    MI_SETTINGS,
    MI_SYSTEM_ENTER,
    MI_ABOUT_PGM,
    0 }; // Terminator


const uint8_t mid_file[] = {
    MI_LOAD_PGM,
    MI_SAVE_PGM,
    MI_MSC,
    MI_DISK_INFO,
    0 }; // Terminator


const uint8_t mid_print[] = {
    MI_PRTOF_GRAPH,
    MI_PRTOF_TEXT,
    MI_PRTOF_GR_IN_TXT,
    MI_PRTOF_NOIR,
    MI_PRINT_DBLNL,
    MI_PRINT_VIEW,
    MI_PRINT_DEL,
    0 }; // Terminator


const uint8_t mid_statefile[] = {
    MI_LOAD_STATE,
    MI_SAVE_STATE,
    MI_CLEAN_RESET,
    0 }; // Terminator




const uint8_t mid_settings[] = {
    MI_SET_TIME,
    MI_SET_DATE,
    MI_TOPBAR_MENU,
    MI_STACK_AREA,
    MI_BEEP_MUTE,
    MI_SLOW_AUTOREP,
    MI_STACK_CONFIG,
    MI_DYNSTACKEXT,
    0 }; // Terminator


const uint8_t mid_topbar[] = {
    MI_DISP_STATFN,
    MI_DISP_DOW,
    MI_DISP_DATE,
    MI_DISP_DATE_SEP,
    MI_DISP_SHORTMON,
    MI_DISP_TIME,
    MI_DISP_VOLTAGE,
    0 }; // Terminator


const uint8_t mid_stack_area[] = {
    MI_SA_REG_X,
    MI_SA_REG_Y,
    MI_SA_REG_Z,
    MI_SA_REG_T,
    MI_SA_REG_L,
    MI_SA_REG_A,
    0 }; // Terminator


const uint8_t mid_stack[] = {
    MI_STACK_XYZTL,
    MI_STACK_XYZTA,
    MI_STACK_XYZT,
    MI_STACK_XYL,
    MI_STACK_XYA,
    MI_STACK_XY,
    MI_STACK_LXYZT,
    0 }; // Terminator




int stack_menu_index() {
  const uint8_t ixs[] = {
    STACK_XYZTL,
    STACK_XYZTA,
    STACK_XYZT,
    STACK_XYL,
    STACK_XYA,
    STACK_XY,
    STACK_LXYZT,
    0 }; // Terminator
  int ix = 0;
  int sl = get_stack_layout();

  for(ix=0; ixs[ix]; ix++)
    if (ixs[ix] == sl)
      return ix;

  return 0;
}



const smenu_t         MID_MENU = { "Setup",  mid_menu,   NULL, NULL };
const smenu_t         MID_FILE = { "File",   mid_file,   NULL, NULL };
const smenu_t        MID_PRINT = { "Printing", mid_print, NULL, NULL };
const smenu_t     MID_SETTINGS = { "Settings",  mid_settings,  NULL, NULL};
const smenu_t MID_STACK_CONFIG = { "Stack Layout", mid_stack, NULL, NULL};
const smenu_t    MID_STATEFILE = { "Calculator State", mid_statefile, NULL, NULL};
const smenu_t       MID_TOPBAR = { "Status Bar", mid_topbar, NULL, NULL};
const smenu_t   MID_STACK_AREA = { "Stack Area", mid_stack_area, NULL, NULL};


// ----------------------------------------------------------------------------------





// Prototypes




int run_menu_item(uint8_t line_id) {
  int ret = 0;

  switch(line_id) {
  //void file_selection_screen(const char * title, const char * base_dir, const char * ext, file_sel_fn_t sel_fn, int disp_new, void * data)
  case MI_LOAD_PGM:
    //pgm_import();
    ret = file_selection_screen("Program Import", PGM_DIR, PGM_EXT, pgm_import_enter, 0, 0, NULL);
    if (ret == MRET_EXIT) ret = 0;
    break;
  case MI_SAVE_PGM:
    pgm_export();
    break;
  case MI_LOAD_STATE:
    ret = file_selection_screen("Load Calculator State", STATE_DIR, STATE_EXT_MASK, load_statefile, 0, 0, NULL);
    if (ret == MRET_EXIT) ret = 0;
    break;
  case MI_SAVE_STATE:
    // Don't pass through if the power is insufficient  
    if ( power_check_screen() ) break;
    ret = file_selection_screen("Save Calculator State", STATE_DIR, STATE_EXT_MASK, save_statefile, 1, 1, NULL);
    if (ret == MRET_EXIT) ret = 0;
    break;

  case MI_CLEAN_RESET:
    run_reset_state_file();
    //ret = MRET_EXIT;
    break;

  //case MI_SAVE_STAT:
  //  ret = MRET_SAVESTATE;
  //  break;


  case MI_FILE:
    ret = handle_menu(&MID_FILE,MENU_ADD, 0);
    break;
  case MI_STATEFILE:
    ret = handle_menu(&MID_STATEFILE,MENU_ADD, 0);
    break;

  case MI_MSC:
    // Flush printer buffers before entering MSC
    prtof_buf_flush(PRTOF_GRAPHICS, PRTOF_FULL_FLUSH);
    prtof_buf_flush(PRTOF_TEXT, PRTOF_FULL_FLUSH);
    ret = MRET_UNIMPL; // Leave the action to core
    break;

  case MI_PRINT_MENU:
    ret = handle_menu(&MID_PRINT,MENU_ADD, 0);
    break;

  case MI_PRTOF_GRAPH:
    set_print_to_file(PRTOF_GRAPHICS, !is_print_to_file(PRTOF_GRAPHICS), 1);
    break;
  case MI_PRTOF_TEXT:
    set_print_to_file(PRTOF_TEXT, !is_print_to_file(PRTOF_TEXT), 1);
    break;
  case MI_PRTOF_NOIR:
    set_print_to_file(PRTOF_NOIR, !is_print_to_file(PRTOF_NOIR), 1);
    break;
  case MI_PRTOF_GR_IN_TXT:
    set_print_to_file(PRTOF_GR_IN_TXT, !is_print_to_file(PRTOF_GR_IN_TXT), 1);
    break;
  case MI_PRINT_DBLNL:
    set_print_to_file(PRINT_DBLNL, !is_print_to_file(PRINT_DBLNL), 1);
    break;
  case MI_PRINT_VIEW:
    ret = file_selection_screen("View Text Print", PRINT_DIR, PRINT_TXT_EXT, text_viewer, 0, 0, NULL);
    if (ret == MRET_EXIT) ret = 0;
    break;
  case MI_PRINT_DEL:
    ret = file_selection_screen("Delete Print File", PRINT_DIR, PRINT_TXT_EXT, delete_file, 0, 0, NULL);
    if (ret == MRET_EXIT) ret = 0;
    break;


  /* Stack */
  case MI_STACK_CONFIG:
    handle_menu(&MID_STACK_CONFIG,MENU_ADD,stack_menu_index());
    break;
  case MI_DYNSTACKEXT:
    set_dynstackext(!get_dynstackext());
    break;
  case MI_STACK_XYZTL:
    set_stack_layout(STACK_XYZTL);
    ret = MRET_EXIT;
    break;
  case MI_STACK_XYZTA:
    set_stack_layout(STACK_XYZTA);
    ret = MRET_EXIT;
    break;
  case MI_STACK_XYZT:
    set_stack_layout(STACK_XYZT);
    ret = MRET_EXIT;
    break;
  case MI_STACK_XYL:
    set_stack_layout(STACK_XYL);
    ret = MRET_EXIT;
    break;
  case MI_STACK_XYA:
    set_stack_layout(STACK_XYA);
    ret = MRET_EXIT;
    break;
  case MI_STACK_XY:
    set_stack_layout(STACK_XY);
    ret = MRET_EXIT;
    break;
  case MI_STACK_LXYZT:
    set_stack_layout(STACK_LXYZT);
    ret = MRET_EXIT;
    break;

  /* Top Bar */
  case MI_TOPBAR_MENU:
    ret = handle_menu(&MID_TOPBAR, MENU_ADD, 0);
    break;
  case MI_DISP_STATFN:   toggle_disp(DISP_STATFN);   break;
  case MI_DISP_DOW:      toggle_disp(DISP_DOW);      break;
  case MI_DISP_DATE:     toggle_disp(DISP_DATE);     break;
  case MI_DISP_DATE_SEP: toggle_disp(DISP_DATE_SEP); break;
  case MI_DISP_SHORTMON: toggle_disp(DISP_SHORTMON); break;
  case MI_DISP_TIME:     toggle_disp(DISP_TIME);     break;
  case MI_DISP_VOLTAGE:  toggle_disp(DISP_VOLTAGE);  break;

  case MI_SETTINGS:
    ret = handle_menu(&MID_SETTINGS,MENU_ADD, 0);
    break;

  case MI_STACK_AREA:
    ret = handle_menu(&MID_STACK_AREA,MENU_ADD, 0);
    break;

  case MI_SA_REG_X:  inc_reg_font_offset(LINE_REG_X); break;
  case MI_SA_REG_Y:  inc_reg_font_offset(LINE_REG_Y); break;
  case MI_SA_REG_Z:  inc_reg_font_offset(LINE_REG_Z); break;
  case MI_SA_REG_T:  inc_reg_font_offset(LINE_REG_T); break;
  case MI_SA_REG_L:  inc_reg_font_offset(LINE_REG_L); break;
  case MI_SA_REG_A:  inc_reg_font_offset(LINE_REG_A); break;


  case MI_ABOUT_PGM:
    lcd_for_dm42(DISP_ABOUT);
    wait_for_key_press();
    break;

   default:
    ret = MRET_UNIMPL;
    break;
  }

  return ret;
}




char * sep_str(char * s, char const *txt, char c) {
  sprintf(s,"[ ] %s", txt);
  s[1] = c;
  return s;
}


char * orb_str(char * s, char const *txt, int val) {
  strcpy(s, rb_str(val));
  strcat(s, " ");
  strcat(s, txt);
  return s;
}


char * b_str(char * s, char const *txt, int val) {
  s[0]=0;
  if(val)
    strcpy(s, "\xff");
  strcat(s, txt);
  return s;
}




char * layout_str(char * s, const char * txt) {
  sprintf(s, "%s  [ ", txt);
  get_stack_layout_str(s+strlen(s), get_stack_layout());
  strcat(s, " ]");
  return s;
}


char * state_str(char *s, const char * txt) {
  const int maxlen = 30;
  sprintf(s, "%s [", txt);
  copy_reset_state_filename(s,maxlen);
  strcat(s, "]");
  return s;
}


char * nr_str(char *s, const char * txt, int nr) {
  sprintf(s, "%s: %i", txt, nr);
  return s;
}



// Returns NULL if not found
const char * menu_line_str(uint8_t line_id, char * s, const int slen) {
  const char * ln;

  switch(line_id) {
  case MI_LOAD_PGM:     ln = "Load Program >";       break;
  case MI_SAVE_PGM:     ln = "Save Program >";       break;
  case MI_SAVE_STAT:    ln = "Save Calculator State";break;
  case MI_FILE:         ln = "File >";               break;
  case MI_STATEFILE:    ln = state_str(s, "Calc. State >"); break;
  case MI_LOAD_STATE:   ln = "Load State";           break;
  case MI_SAVE_STATE:   ln = "Save State";           break;
  case MI_CLEAN_RESET:  ln = "Load Clean State";     break;

  case MI_STACK_CONFIG:
    ln = layout_str(s, "Stack Layout");              break;

  case MI_DYNSTACKEXT:  ln = opt_str(s, " Dynamic Stack Extension", get_dynstackext()); break;

  case MI_STACK_XYZTL:  ln = orb_str(s, "XYZTL", get_stack_layout() == STACK_XYZTL); break;
  case MI_STACK_XYZTA:  ln = orb_str(s, "XYZTA", get_stack_layout() == STACK_XYZTA); break;
  case MI_STACK_XYZT:   ln = orb_str(s, "XYZT",  get_stack_layout() == STACK_XYZT);  break;
  case MI_STACK_XYL:    ln = orb_str(s, "XYL",   get_stack_layout() == STACK_XYL);   break;
  case MI_STACK_XYA:    ln = orb_str(s, "XYA",   get_stack_layout() == STACK_XYA);   break;
  case MI_STACK_XY:     ln = orb_str(s, "XY",    get_stack_layout() == STACK_XY);    break;
  case MI_STACK_LXYZT:  ln = orb_str(s, "LXYZT", get_stack_layout() == STACK_LXYZT); break;

  case MI_TOPBAR_MENU:  ln = "Status Bar >";         break;
  case MI_DISP_STATFN:  ln = opt_str(s, " State Filename",  is_disp(DISP_STATFN));  break;
  case MI_DISP_DOW:     ln = opt_str(s, " Day of Week",     is_disp(DISP_DOW));     break;
  case MI_DISP_DATE:    ln = opt_str(s, " Date",            is_disp(DISP_DATE));    break;
  case MI_DISP_DATE_SEP:ln = sep_str(s, " Date Separator",  get_disp_date_sep());   break;
  case MI_DISP_SHORTMON:ln = opt_str(s, "  Month Shortcut", is_disp(DISP_SHORTMON));break;
  case MI_DISP_TIME:    ln = opt_str(s, " Time",            is_disp(DISP_TIME));    break;
  case MI_DISP_VOLTAGE: 
    ln = opt_str(s, " Power Voltage in Header", is_disp(DISP_VOLTAGE));   break;

  case MI_SETTINGS:     ln = "Settings >";           break;
  case MI_ABOUT_PGM:    ln = "About >";              break;

  case MI_STACK_AREA:   ln = "Stack Font Sizes >";         break;
  case MI_SA_REG_X:     ln = nr_str(s, "Font Size Offset Reg X", get_reg_font_offset(LINE_REG_X) ); break;
  case MI_SA_REG_Y:     ln = nr_str(s, "Font Size Offset Reg Y", get_reg_font_offset(LINE_REG_Y) ); break;
  case MI_SA_REG_Z:     ln = nr_str(s, "Font Size Offset Reg Z", get_reg_font_offset(LINE_REG_Z) ); break;
  case MI_SA_REG_T:     ln = nr_str(s, "Font Size Offset Reg T", get_reg_font_offset(LINE_REG_T) ); break;
  case MI_SA_REG_L:     ln = nr_str(s, "Font Size Offset Reg L", get_reg_font_offset(LINE_REG_L) ); break;
  case MI_SA_REG_A:     ln = nr_str(s, "Font Size Offset Reg A", get_reg_font_offset(LINE_REG_A) ); break;

  case MI_PRINT_MENU:      ln = "Printing >"; break;
  case MI_PRTOF_GRAPH:     ln = opt_str(s, " Graphics Print",    is_print_to_file(PRTOF_GRAPHICS));  break;
  case MI_PRTOF_TEXT:      ln = opt_str(s, " Text Print",        is_print_to_file(PRTOF_TEXT));      break;
  case MI_PRTOF_NOIR:      ln = opt_str(s, " Don't print to IR", is_print_to_file(PRTOF_NOIR));      break;
  case MI_PRTOF_GR_IN_TXT: ln = opt_str(s, " Graphics in Text",  is_print_to_file(PRTOF_GR_IN_TXT)); break;
  case MI_PRINT_DBLNL:     ln = opt_str(s, " Double Newline",    is_print_to_file(PRINT_DBLNL));     break;
  case MI_PRINT_VIEW:      ln = "View Text Print >";  break;
  case MI_PRINT_DEL:       ln = "Delete Print >";     break;

  default:
    ln = NULL;
    break;
  }

  return ln;
}


// -------------------------------------------------------------------



} // C
