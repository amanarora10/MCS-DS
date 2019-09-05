/**********************************
 * FILE NAME: MP1Node.cpp
 *
 * DESCRIPTION: Membership protocol run by this Node.
 * 				Header file of MP1Node class.
 **********************************/

#ifndef _MP1NODE_H_
#define _MP1NODE_H_

#include "stdincludes.h"
#include "Log.h"
#include "Params.h"
#include "Member.h"
#include "EmulNet.h"
#include "Queue.h"

/**
 * Macros
 */
#define TREMOVE 20
#define TFAIL 5
#define TGOSSIP 1

#define P_NGR_SELECT 100 //Proability of picking a neighor (max 100) 

//Max. neighbors to send gossip
#define MAX_NEIGHBORS 5
/*
 * Note: You can change/add any functions in MP1Node.{h,cpp}
 */

/* */

typedef struct Record_t {
	int  id;
	short port;
	int state; /*invalid, Alive, Failed*/
	long heartbeat;
	long timestamp;
}Record_t;

/**
 * Message Types
 */
enum MsgTypes{
    JOINREQ,
    JOINREP,
	GOSSIP,
    DUMMYLASTMSGTYPE
};

/**
 * Message Types
 */
typedef enum {
	INVALID = 0,
	VALID,
	FAILED,
	REMOVED
}Statetype;
/**
 * STRUCT NAME: MessageHdr
 *
 * DESCRIPTION: Header and content of a message
 */
typedef struct MessageHdr {
	enum MsgTypes msgType;
}MessageHdr;

/**
 * STRUCT NAME: JOINREP message
 *
 * DESCRIPTION: Header and content of Gossip message
 */
typedef struct Joinrep_t {
	MessageHdr header;
	Record_t* memberList;
}Joinrep_t;


/**
 * STRUCT NAME: GOSSIP message
 *
 * DESCRIPTION: Header and content of Gossip message
 */

typedef struct Gossip_t {
	MessageHdr header;
	vector<MemberListEntry> memberList;
}Gossip_t;


/**
 * CLASS NAME: MP1Node
 *
 * DESCRIPTION: Class implementing Membership protocol functionalities for failure detection
 */
class MP1Node {
private:
	EmulNet *emulNet;
	Log *log;
	Params *par;
	Member *memberNode;
	char NULLADDR[6];
	Record_t* table;
	int last_gossip_time;
public:
	MP1Node(Member *, Params *, EmulNet *, Log *, Address *);
	Member * getMemberNode() {
		return memberNode;
	}
	int recvLoop();
	static int enqueueWrapper(void *env, char *buff, int size);
	void nodeStart(char *servaddrstr, short serverport);
	int initThisNode(Address *joinaddr);
	int introduceSelfToGroup(Address *joinAddress);
	int finishUpThisNode();
	void nodeLoop();
	void checkMessages();
	bool recvCallBack(void *env, char *data, int size);
	void nodeLoopOps();
	int isNullAddress(Address *addr);
	Address getJoinAddress();
	void initMemberListTable(Member *memberNode);
	void initMembertable(Record_t** table);
	void update_list(Record_t* table, int current_time, int id, long heartbeat, bool logging, int state);
	void send_joinrep(EmulNet* ent, Address* dest, Address* src, Record_t* memberList);
	void send_gossip(EmulNet* ent, Address* dest, Address* src, Record_t* table);
	void merge_tables(Record_t* rx_table, Record_t* current_table);
	void printAddress(Address *addr);
	int select_neighbor(int*, Address* addr);
	void check_heartbeat(Record_t* table);
	virtual ~MP1Node();
};

#endif /* _MP1NODE_H_ */
