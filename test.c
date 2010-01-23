
#include <stdio.h>
#include "smartpen.h"

int main (void)
{
        obex_t *handle = smartpen_connect(0x1cfb, 0x1020);
        char *changelist = smartpen_get_changelist(handle, 0);

        printf("Changelist: %s\n", changelist);
        printf("Done\n");

        FILE *out = fopen("data", "w");
        smartpen_get_guid(handle, out, "0xbc61f02c87e32f9b", 8782700699);
        fclose(out);

        smartpen_disconnect(handle);
        return 0;
}
