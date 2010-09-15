#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 500
#endif /* _XOPEN_SOURCE */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <X11/Xlib.h>

#include "key.h"
#include "xchainkeys.h"

extern XChainKeys_t *xc;

Key_t* key_new(char *keyspec) {

  Key_t *self = (Key_t *) calloc(1, sizeof(Key_t));;
  
  if( key_parse_keyspec(self, keyspec) ) {
    return(self);
  }
  else {
    free(self);
    return(NULL);
  }
}

int key_parse_keyspec(Key_t *self, char *keyspec) {

  char str[256];
  char *original_keyspec = strdup(keyspec);
  int len, ret;

  if (keyspec[0] == ':')
    return 0;

  /* parse modifiers */
  while( strstr(keyspec, "-") != NULL ) {

    len = strcspn(keyspec, "-");
    strncpy(str, keyspec, len);
    str[len] = '\0';
    
    if(!key_add_modifier(self, str))
      fprintf(stderr, "Warning: ignoring unknown modifier '%s' in keyspec '%s'\n", 
	      str, original_keyspec);

    keyspec += len + 1;
  }
  
  /* keyspec now contains the keysym string */
  ret = key_set_keycode(self, keyspec);
  
  free(original_keyspec);

  return ret;
}

int key_add_modifier(Key_t *self, char *str) {
  
  if( strcasecmp(str, "shift") == 0 || strcmp(str, "S") == 0) {
    self->modifiers |= ShiftMask;
    return 1;
  }

  if( strcasecmp(str, "lock") == 0 ) {
    self->modifiers |= LockMask;
    return 1;
  }

  if( strcasecmp(str, "control") == 0 || strcmp(str, "C") == 0) {
    self->modifiers |= ControlMask;
    return 1;
  }

  if( strcasecmp(str, "mod1") == 0 || strcmp(str, "A") == 0 || strcmp(str, "M") == 0 ) {
    self->modifiers |= Mod1Mask;
    return 1;
  }
  if( strcasecmp(str, "mod2") == 0 ) {
    self->modifiers |= Mod2Mask;
    return 1;
  }

  if( strcasecmp(str, "mod3") == 0 ) {
    self->modifiers |= Mod3Mask;
    return 1;
  }

  if( strcasecmp(str, "mod4") == 0 || strcmp(str, "W") == 0 || strcmp(str, "H") == 0 ) {
    self->modifiers |= Mod4Mask;
    return 1;
  }

  if( strcasecmp(str, "mod5") == 0 ) {
    self->modifiers |= Mod4Mask;
    return 1;
  }

  return 0;
}

int key_set_keycode(Key_t *self, char *str) {
  KeySym keysym;

  keysym = XStringToKeysym(str);
  if( keysym == NoSymbol ) {
    return 0;
  }
  self->keycode = XKeysymToKeycode(xc->display, keysym);
  return 1;
}

int key_equals(Key_t *self, Key_t *key) {
  if(self->modifiers == key->modifiers &&
     self->keycode == key->keycode)
    return 1;
  return 0;
}

void key_grab(Key_t *self) {
  int i;

  for( i=0; i<8; i++ ) {
    XGrabKey(xc->display, self->keycode, self->modifiers | xc->modmask[i], 
	     DefaultRootWindow(xc->display), False,
	     GrabModeAsync, GrabModeAsync);
  }
}

void key_ungrab(Key_t *self) {
  XUngrabKey(xc->display, self->keycode, self->modifiers, 
	   DefaultRootWindow(xc->display));
}

char *key_to_str(Key_t *self) {

  char *str = (char *) calloc(256, sizeof(char));

  if (self->modifiers & LockMask)    strcat(str, "lock-");
  if (self->modifiers & ControlMask) strcat(str, "C-");
  if (self->modifiers & Mod1Mask)    strcat(str, "A-");
  if (self->modifiers & Mod2Mask)    strcat(str, "mod2-");
  if (self->modifiers & Mod3Mask)    strcat(str, "mod3-");
  if (self->modifiers & Mod4Mask)    strcat(str, "W-");
  if (self->modifiers & Mod5Mask)    strcat(str, "mod5-");
  if (self->modifiers & ShiftMask)   strcat(str, "S-");

  strcat(str, XKeysymToString(XKeycodeToKeysym(xc->display, self->keycode, 0)));

  return str;	 
}
