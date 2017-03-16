
#include "mqttmsg.h"

struct s_mqttmsg_info
{
    char message_buffer[MAXSIZE];
    map<e_mqttmsg_id, size_t> offset_map;

public:
    s_mqttmsg_info();
    const char* get_message_pointer(e_mqttmsg_id code) const;
};

static s_mqttmsg_info s_msginfo;

// ----------------------------------------------------------------
// internal functions
// ----------------------------------------------------------------

s_mqttmsg_info::s_mqttmsg_info()
{
    memset(message_buffer, 0x00, sizeof(message_buffer));

    size_t cpos = 0;
    size_t maxpos = sizeof(message_buffer) / sizeof(message_buffer[0]);

    char* cptr = message_buffer;

    #define MAP_ITEM(code, name) do { \
        ASSERT(cpos < maxpos); \
        string message = name; \
        offset_map[code] = cpos; \
        size_t mlen = message.length(); \
        memcpy(cptr, message.c_str(), mlen); cptr += mlen; \
        *cptr = 0; cptr += 1; \
        cpos += mlen + 1; \
    } while(0);

    #include "mqttmap.def"
    #undef MAP_ITEM
}

const char* s_mqttmsg_info::get_message_pointer(e_mqttmsg_id code) const
{
    map<e_mqttmsg_id, size_t>::const_iterator iter = offset_map.find(code);
    if (iter == offset_map.end()) return NULL;

    return message_buffer + iter->second;
}

// ----------------------------------------------------------------
// utility functions
// ----------------------------------------------------------------

string get_message_name(e_mqttmsg_id code)
{
    string msgname;
    switch(code)
    {
        #define MAP_ITEM(code, name) case code: msgname = name; break;
        #include "mqttmap.def"
        #undef MAP_ITEM
        default: msgname = "unknown-topic"; break;
    }

    return msgname;
}

e_mqttmsg_id get_message_id(const string& msgname)
{
    do {
        #define MAP_ITEM(code, name) if (0 == msgname.compare(name)) return code;
        #include "mqttmap.def"
        #undef MAP_ITEM
    } while(0);

    return MSG_INVALID;
}

const char* get_message_buffer(e_mqttmsg_id code)
{
    return s_msginfo.get_message_pointer(code);
}
