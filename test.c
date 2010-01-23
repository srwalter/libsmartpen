
#include <stdio.h>
#include "smartpen.h"

int main (void)
{
        obex_t *handle = smartpen_connect(0x1cfb, 0x1020);
        char *changelist = smartpen_get_changelist(handle, 0);

        printf("Changelist: %s\n", changelist);

        smartpen_disconnect(handle);
        return 0;
}
