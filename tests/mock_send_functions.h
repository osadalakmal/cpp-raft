
int sender_send(void* caller, void* udata, int peer, int type,
        const unsigned char* data, const int len);

void senders_new();

void* sender_new(void* address);

void* sender_poll_msg_data(void* s);

int sender_msgs_available(void* s);

void sender_set_raft(void* s, void* r);

void sender_poll_msgs(void* s);
