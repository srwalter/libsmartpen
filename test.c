
#include <stdio.h>
#include "smartpen.h"

int main (void)
{
        obex_t *handle = smartpen_connect(0x1cfb, 0x1020);
        char *changelist = smartpen_get_changelist(handle, 0);
        int rc;

        printf("Changelist: %s\n", changelist);
        printf("Done\n");

#if 0
        changelist = smartpen_get_peninfo(handle);

        printf("Peninfo: %s\n", changelist);
        printf("Done\n");
#endif

        FILE *out = fopen("data", "w");
#if 1
        rc = smartpen_get_guid(handle, out, "0x0bf11a726d11f3f3", 9194790000);
        if (!rc) {
            printf("get_guid fail\n");
        }
#else
        smartpen_get_paperreplay(handle, out, 0);
#endif
        fclose(out);

        smartpen_disconnect(handle);
        return 0;
}
