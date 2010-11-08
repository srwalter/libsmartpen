
#include <stdio.h>
#include "smartpen.h"

int main (void)
{
        obex_t *handle = smartpen_connect(0x1cfb, 0x1030);
        char *changelist;
        int rc;

        if (!handle)
            return 1;

        changelist = smartpen_get_changelist(handle, 0);

        printf("Changelist: %s\n", changelist);
        printf("Done\n");

#if 1
        changelist = smartpen_get_peninfo(handle);

        printf("Peninfo: %s\n", changelist);
        printf("Done\n");

        FILE *out = fopen("data", "w");
#if 1
        rc = smartpen_get_guid(handle, out, "0x0bf11a726d11f3f3", 0);
        if (!rc) {
            printf("get_guid fail\n");
        }
#else
        smartpen_get_paperreplay(handle, out, 0);
#endif
        fclose(out);
#endif

        smartpen_disconnect(handle);
        return 0;
}
