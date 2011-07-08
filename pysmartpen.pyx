
cdef extern from "smartpen.h" nogil:
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

cdef extern from "stdio.h":
    FILE *fopen(char *fn, char *mode)
    void fclose(FILE *f)

cdef class Smartpen:
    cdef obex_t *obex
    def __new__(self):
        self.obex = NULL

    def connect(self, vendor=0x1cfb, product=0x1030):
        if self.obex != NULL:
            raise RuntimeError("Already connected")
        self.obex = smartpen_connect(vendor, product)
        if (self.obex == NULL):
            raise RuntimeError("Failed to connect")

    def get_changelist(self, start_time=0):
        cdef char *list

        if (self.obex == NULL):
            raise RuntimeError("Not connected")

        list = smartpen_get_changelist(self.obex, start_time)
        if list != NULL:
            return list
        raise RuntimeError("Failed to get changelist")

    def get_guid(self, filename, guid, start_time=0):
        cdef FILE *f
        cdef int rc

        if (self.obex == NULL):
            raise RuntimeError("Not connected")

        f = fopen(filename, "w")
        if (f == NULL):
            raise IOError("Failed to open %s" % filename)
        rc = smartpen_get_guid(self.obex, f, guid, start_time)
        fclose(f)
        return rc

    def get_paperreplay(self, filename, start_time=0):
        cdef FILE *f
        cdef int rc
        cdef long long int c_time

        c_time = start_time

        if (self.obex == NULL):
            raise RuntimeError("Not connected")

        f = fopen(filename, "w")
        if (f == NULL):
            raise IOError("Failed to open %s" % filename)
        with nogil:
            rc = smartpen_get_paperreplay(self.obex, f, c_time)
        fclose(f)
        return rc

    def get_info(self):
        if (self.obex == NULL):
            raise RuntimeError("Not connected")
        return smartpen_get_peninfo(self.obex)

    def disconnect(self):
        if (self.obex == NULL):
            raise RuntimeError("Not connected")
        smartpen_disconnect(self.obex)
        self.obex = NULL
