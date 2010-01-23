#ifndef _SMARTPEN_H_
#define _SMARTPEN_H_

#include <openobex/obex.h>

obex_t *smartpen_connect(short vendor, short product);
char *smartpen_get_changelist(obex_t *handle, int starttime);
void smartpen_disconnect (obex_t *handle);

#endif
