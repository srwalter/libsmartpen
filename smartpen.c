/* vim: set sw=8 noet: */

#include <openobex/obex.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include <arpa/inet.h>

struct obex_state {
    obex_t *handle;
    int req_done;
    char *body;
    int body_len;
    
};

static void obex_requestdone (struct obex_state *state, obex_t *hdl,
                              obex_object_t *obj, int cmd, int resp)
{
	uint8_t header_id;
	obex_headerdata_t hdata;
	uint32_t hlen;

	switch (cmd & ~OBEX_FINAL) {
		case OBEX_CMD_GET:
			while (OBEX_ObjectGetNextHeader(hdl, obj, &header_id,
							&hdata, &hlen)) {
				if (header_id == OBEX_HDR_BODY ||
				    header_id == OBEX_HDR_BODY_END) {
					if (state->body)
						free(state->body);
					state->body = malloc(hlen);
					state->body_len = hlen;
					memcpy(state->body, hdata.bs, hlen);
					break;
				}
			}
			break;
	}

	state->req_done++;
}

static void obex_event (obex_t *hdl, obex_object_t *obj, int mode,
			int event, int obex_cmd, int obex_rsp)
{
	struct obex_state *state;
	obex_headerdata_t hd;
	int size;
	int rc;

	state = OBEX_GetUserData(hdl);

	if (event == OBEX_EV_PROGRESS) {
		hd.bq4 = 0;
		size = 4;
		rc = OBEX_ObjectAddHeader(hdl, obj, OBEX_HDR_CONNECTION,
					  hd, size, OBEX_FL_FIT_ONE_PACKET);
		if (rc < 0) {
			printf("oah fail %d\n", rc);
		}
		return;
	}

	if (obex_rsp != OBEX_RSP_SUCCESS && obex_rsp != OBEX_RSP_CONTINUE) {
		printf("FAIL %x %x\n", obex_rsp, event);
		state->req_done++;
		return;
	}

	switch (event) {
		case OBEX_EV_REQDONE:
			obex_requestdone(state, hdl, obj, obex_cmd, obex_rsp);
			break;

		default:
			printf("Funny event\n");
	}
}

obex_t *smartpen_connect(short vendor, short product)
{
	obex_t *handle;
	obex_object_t *obj;
	int i, num;
	struct obex_state *state;
	obex_interface_t *obex_intf;
	obex_headerdata_t hd;
	int size;

	handle = OBEX_Init(OBEX_TRANS_USB, obex_event, 0);
	if (!handle)
		goto out;

        num = OBEX_EnumerateInterfaces(handle);
	for (i=0; i<num; i++) {
		obex_intf = OBEX_GetInterfaceByIndex(handle, i);
		if (obex_intf->usb.idVendor == vendor &&
		    obex_intf->usb.idProduct == product) {
			break;
		}
	}
        if (i == num) {
		handle = NULL;
		goto out;
        }

        if (OBEX_InterfaceConnect(handle, obex_intf) < 0) {
		handle = NULL;
		goto out;
	}

	state = malloc(sizeof(struct obex_state));
	if (!state) {
		handle = NULL;
		goto out;
	}
	memset(state, 0, sizeof(struct obex_state));
        OBEX_SetUserData(handle, state);

        OBEX_SetTransportMTU(handle, 0x400, 0x400);

        obj = OBEX_ObjectNew(handle, OBEX_CMD_CONNECT);
        hd.bs = (unsigned char *)"LivescribeService";
        size = strlen((char*)hd.bs)+1;
        OBEX_ObjectAddHeader(handle, obj, OBEX_HDR_TARGET, hd, size, 0);

        if (OBEX_Request(handle, obj) < 0) {
		handle = NULL;
		goto out;
	}

        while (state->req_done < 1) {
            OBEX_HandleInput(handle, 10);
        }
out:
	return handle;
}

char *smartpen_get_changelist(obex_t *handle, int starttime)
{
	struct obex_state *state;
	int req_done;
	obex_object_t *obj;
	obex_headerdata_t hd;
	int size, i;
	glong num;
	char name[256];

	state = OBEX_GetUserData(handle);

	obj = OBEX_ObjectNew(handle, OBEX_CMD_GET);
	hd.bq4 = 0;
	size = 4;
	OBEX_ObjectAddHeader(handle, obj, OBEX_HDR_CONNECTION,
			     hd, size, OBEX_FL_FIT_ONE_PACKET);

	snprintf(name, sizeof(name), "changelist?start_time=%d", starttime);
	hd.bs = (unsigned char *)g_utf8_to_utf16(name, strlen(name),
						 NULL, &num, NULL);

	for (i=0; i<num; i++) {
		uint16_t *wchar = (uint16_t*)&hd.bs[i*2];
		*wchar = ntohs(*wchar);
	}
	size = (num+1) * sizeof(uint16_t);
	OBEX_ObjectAddHeader(handle, obj, OBEX_HDR_NAME, hd, size, OBEX_FL_FIT_ONE_PACKET);

	if (OBEX_Request(handle, obj) < 0)
		return NULL;

	req_done = state->req_done;
	while (state->req_done == req_done) {
		OBEX_HandleInput(handle, 10);
	}

	return state->body;
}

void smartpen_disconnect (obex_t *handle)
{
	struct obex_state *state;
	int req_done;
	obex_object_t *obj;
	obex_headerdata_t hd;
	int size;

	obj = OBEX_ObjectNew(handle, OBEX_CMD_DISCONNECT);
	hd.bq4 = 0;
	size = 4;
	OBEX_ObjectAddHeader(handle, obj, OBEX_HDR_CONNECTION,
			     hd, size, OBEX_FL_FIT_ONE_PACKET);

	if (OBEX_Request(handle, obj) < 0)
		return;

	state = OBEX_GetUserData(handle);
	req_done = state->req_done;
	while (state->req_done == req_done) {
		OBEX_HandleInput(handle, 10);
	}
}
