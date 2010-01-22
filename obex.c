#include <openobex/obex.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

volatile int loop = 0;

typedef struct {
    obex_t *handle;
    int connid;
    int err;
    char *body;
    
} obex_state;

static void obex_requestdone (obex_state *state, obex_t *hdl,
                              obex_object_t *obj, int cmd, int resp)
{
    uint8_t header_id;
    obex_headerdata_t hdata;
    uint32_t hlen;

    printf("Request done\n");
    
    switch (cmd) {
        case OBEX_CMD_CONNECT:
            printf("cmd connect\n");
            while (OBEX_ObjectGetNextHeader(hdl, obj, &header_id, 
                                            &hdata, &hlen)) {
                /* Get the connection identifier */
                printf("header %x\n", header_id);
                if (header_id == OBEX_HDR_CONNECTION) {
                    memcpy(&state->connid, &hdata, 4);
                    printf("conn id: %d\n", state->connid);
                }
            }
            break;

        case OBEX_CMD_PUT:
            printf("cmd put\n");
            while (OBEX_ObjectGetNextHeader(hdl, obj, &header_id, 
                                            &hdata, &hlen))
                printf("header %d\n", header_id);
            break;
            
        case OBEX_CMD_GET | OBEX_FINAL:
        case OBEX_CMD_GET:
            printf("cmd get\n");
            while (OBEX_ObjectGetNextHeader(hdl, obj, &header_id,
                                            &hdata, &hlen)) {
                printf("header %x\n", header_id);
                if (header_id == OBEX_HDR_BODY) {
                    printf("Body!\n\n%s\n\n", hdata.bs);
                    if (state->body)
                        free(state->body);
                    state->body = malloc(hlen);
                    memcpy(state->body, hdata.bs, hlen);
                    break;
                }
            }
            break;

        default:
            printf("Funny command\n");
    }
}

static void obex_event (obex_t *hdl, obex_object_t *obj, int mode,
        int event, int obex_cmd, int obex_rsp) {
    obex_state *state;

    printf("event!\n");

    state = OBEX_GetUserData(hdl);

    if (obex_rsp != OBEX_RSP_SUCCESS && obex_rsp != OBEX_RSP_CONTINUE) {
        printf("Got questionable response %x\n", obex_rsp);
    }
    
    switch (event) {
        case OBEX_EV_PROGRESS:
            printf("Progess\n");
        case OBEX_EV_REQDONE:
            obex_requestdone(state, hdl, obj, obex_cmd, obex_rsp);
            break;

        default:
            printf("Funny event\n");
    }
}

int syncml_obex_send_req (char *data, int len,
                          char *cmd, char *contenttype) {
    obex_t *handle = NULL;
    obex_object_t *obj;
    obex_state state;
    obex_headerdata_t hd;
    obex_interface_t *obex_intf;
    int size;
    int i, num_interfaces;

    if (!handle) {
        handle = OBEX_Init(OBEX_TRANS_USB, obex_event, 0);
        state.handle = handle;
        state.body = NULL;
        if (!handle)
            perror("handle");
        OBEX_SetUserData(handle, &state);
        
        num_interfaces = OBEX_EnumerateInterfaces(handle);
        for (i=0; i<num_interfaces; i++) {
            obex_intf = OBEX_GetInterfaceByIndex(handle, i);
            if (obex_intf->usb.idVendor == 0x1cfb &&
                obex_intf->usb.idProduct == 0x1020)
            {
                printf("Found Livescribe Pulse pen\n");
                break;
            }
        }
        if (i == num_interfaces) {
            return 0;
        }

        i = OBEX_InterfaceConnect(handle, obex_intf);
        printf("connect = %d\n", i);

        obj = OBEX_ObjectNew(handle, OBEX_CMD_CONNECT);
        hd.bs = "LiveScribeService";
        size = strlen(hd.bs);
        OBEX_ObjectAddHeader(handle, obj, OBEX_HDR_TARGET, hd, size, 0);

        if (OBEX_Request(handle, obj) < 0)
            printf("request");

        printf("foo1\n");
        while (OBEX_HandleInput(handle, 1) == 0);
        printf("foo1a\n");
    }

    obj = OBEX_ObjectNew(handle, OBEX_CMD_GET);
    hd.bq4 = 0;
    size = 4;
    OBEX_ObjectAddHeader(handle, obj, OBEX_HDR_CONNECTION, hd, size, OBEX_FL_FIT_ONE_PACKET);
    
    hd.bs = contenttype;
    size = strlen(contenttype)+1;
    OBEX_ObjectAddHeader(handle, obj, OBEX_HDR_TYPE, hd, size, OBEX_FL_FIT_ONE_PACKET);
    
    if (OBEX_Request(handle, obj) < 0)
        printf("foo2\n");
    OBEX_HandleInput(handle, 3);

    return 1;
}

int main (int argc, char **argv) {
    char *data = "This is some exciting data!";
    int ret;
    
    ret = syncml_obex_send_req(data, strlen(data), NULL, "x-obex/folder-listing");

    if (!ret)
        printf("ugh\n");

    /*while (!loop) {
        loop = 0;
    }*/
    return 0;
}
