#include <bluetooth/bluetooth.h>
#include <openobex/obex.h>

volatile int loop = 0;

typedef struct {
    obex_t *handle;
    int connid;
    int err;
    char *body;
    
} obex_state;

static obex_t *handle = NULL;

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
                    //memcpy(&state->connid, &hdata, 4);
                    printf("conn id: %d\n", hdata);
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

void syncml_obex_recv (obex_t *handle) {
    obex_object_t *obj;
    obex_headerdata_t hdata;
    uint32_t hlen;
    char *mimetype = "application/vnd.syncml+wbxml"; /* FIXME */

    printf("receiving!\n");

    obj = OBEX_ObjectNew(handle, OBEX_CMD_GET);
    OBEX_Request(handle, obj);
    //hdata.bs = obstate->connid;
    //hlen = strlen(obstate->connid);
    //OBEX_ObjectAddHeader(handle, obj, OBEX_HDR_CONNECTION, hdata, hlen, 0);
    
    hdata.bs = mimetype;
    hlen = strlen(mimetype);
    OBEX_ObjectAddHeader(handle, obj, OBEX_HDR_TYPE, hdata, hlen, 0);
    OBEX_HandleInput(handle, 0);
    
    /* do stuff with handle->body */
}

/* only needed as server */
/*
int syncml_obex_send_rsp (syncml_state *state, char *data, int len,
                          int code, char *contenttype) {
    g_assert_not_reached();
    return -1;
}*/

int syncml_obex_send_req (char *data, int len,
                          char *cmd, char *contenttype) {
    obex_t *handle = NULL;
    obex_object_t *obj;
    obex_state state;
    obex_headerdata_t hd;
    int size;

    if (!handle) {
        bdaddr_t dest;
        uint8_t channel = 6;

        handle = OBEX_Init(OBEX_TRANS_BLUETOOTH, obex_event, 0);
        state.handle = handle;
        state.body = NULL;
        if (!handle)
            perror("handle");
        OBEX_SetUserData(handle, &state);
        
        if (BtOBEX_TransportConnect(handle, BDADDR_ANY, &dest, channel) < 0) {
            printf("connect\n");
            return -1;
        }
        obj = OBEX_ObjectNew(handle, OBEX_CMD_CONNECT);
        hd.bs = "text/x-vCard";
        size = strlen(hd.bs);
        OBEX_ObjectAddHeader(handle, obj, OBEX_HDR_TYPE, hd, size, 0);
        //hd.bs = "x-obex/object-profile";
        //size = strlen(hd.bs);
        //OBEX_ObjectAddHeader(handle, obj, OBEX_HDR_NAME, hd, size, 0);

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
}
