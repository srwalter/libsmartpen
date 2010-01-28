

cdef extern from "smartpen.h":
    ctypedef struct obex_t
    ctypedef struct FILE

    obex_t *smartpen_connect(short vendor, short product)
    char *smartpen_get_changelist(obex_t *handle, int start_time)
    void smartpen_disconnect (obex_t *handle)
    int smartpen_get_guid (obex_t *handle, FILE *out, char *guid,
			   long long int start_time)
    int smartpen_get_paperreplay (obex_t *handle, FILE *out,
				  long long int start_time)
    int smartpen_get_penletlist (obex_t *handle, FILE *out)
    char *smartpen_get_peninfo (obex_t *handle)

cdef class Smartpen:
    cdef obex_t *obex
    def __new__(self):
        self.obex = NULL

    def connect(self, vendor=0x1cfb, product=0x1020):
        self.obex = smartpen_connect(vendor, product)
        if (self.obex == NULL):
            raise RuntimeError("Failed to connect")

    def get_changelist(self, start_time=0):
        if (self.obex == NULL):
            raise RuntimeError("Not connected")

        return smartpen_get_changelist(self.obex, start_time)
