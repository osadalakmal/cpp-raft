#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <stdbool.h>
#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <deque>
#include "CuTest.h"

#include "raft.h"
#include "raft_server.h"

typedef struct {
    void* data;
    int len;
    /* what type of message is it? */
    int type;
    /* who sent this? */
    int sender;
} msg_t;

typedef struct {
	std::deque<msg_t*>* outbox;
    std::deque<msg_t*>* inbox;
    RaftServer* raft;
} sender_t;

static sender_t** __senders = NULL;
static int __nsenders = 0;

void senders_new()
{
    __senders = NULL;
    __nsenders = 0;
}

int sender_send(void* caller, void* udata, int peer, int type,
        const unsigned char* data, int len)
{
    sender_t* me = (sender_t*)caller;
    msg_t* m;

    m = (msg_t*)malloc(sizeof(msg_t));
    m->type = type;
    m->len = len;
    m->data = malloc(len);
    m->sender = reinterpret_cast<RaftServer*>(udata)->get_nodeid();
    memcpy(m->data,data,len);
    me->outbox->push_back(m);

    if (__nsenders > peer)
    {
        __senders[peer]->inbox->push_back(m);
    }

    return 0;
}

void* sender_new(void* address)
{
    sender_t* me;

    me = (sender_t*) malloc(sizeof(sender_t));
    me->outbox = new std::deque<msg_t*>();
    me->inbox = new std::deque<msg_t*>();
    __senders = (sender_t**)realloc(__senders,sizeof(sender_t*) * (++__nsenders));
    __senders[__nsenders-1] = me;
    return me;
}

void* sender_poll_msg_data(void* s)
{
    sender_t* me = (sender_t*)s;
    msg_t* msg;

    void* retVal = !me->outbox->empty() ? me->outbox->back()->data : NULL;
    if (!me->outbox->empty()) me->outbox->pop_back();
    return retVal;
}

void sender_set_raft(void* s, void* r)
{
    sender_t* me = (sender_t*)s;
    me->raft = reinterpret_cast<RaftServer*>(r);
}

int sender_msgs_available(void* s)
{
    sender_t* me = (sender_t*)s;

    return !me->inbox->empty();
}

void sender_poll_msgs(void* s)
{
    sender_t* me = (sender_t*)s;
    msg_t* m;

    while (!me->inbox->empty())
    {
    	m = me->inbox->back();
        switch (m->type)
        {
            case RAFT_MSG_APPENDENTRIES:
                me->raft->recv_appendentries(m->sender, (MsgAppendEntries*) m->data);
                break;
            case RAFT_MSG_APPENDENTRIES_RESPONSE:
                me->raft->recv_appendentries_response(m->sender, (msg_appendentries_response_t*) m->data);
                break;
            case RAFT_MSG_REQUESTVOTE:
                me->raft->recv_requestvote(m->sender, (msg_requestvote_t*) m->data);
                break;
            case RAFT_MSG_REQUESTVOTE_RESPONSE:
                me->raft->recv_requestvote_response(m->sender, (msg_requestvote_response_t*) m->data);
                break;
            case RAFT_MSG_ENTRY:
                me->raft->recv_entry(m->sender, (msg_entry_t*) m->data);
                break;
            case RAFT_MSG_ENTRY_RESPONSE:
                //me->raft->recv_entry_response(m->sender, m->data);
                break;

        }
        me->inbox->pop_back();
    }
}

