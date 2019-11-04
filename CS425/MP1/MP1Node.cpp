/**********************************
 * FILE NAME: MP1Node.cpp
 *
 * DESCRIPTION: Membership protocol run by this Node.
 * 				Definition of MP1Node class functions.
 **********************************/

#include "MP1Node.h"

/*
 * Note: You can change/add any functions in MP1Node.{h,cpp}
 */

/**
 * Overloaded Constructor of the MP1Node class
 * You can add new members to the class if you think it
 * is necessary for your logic to work
 */
MP1Node::MP1Node(Member *member, Params *params, EmulNet *emul, Log *log, Address *address) {
	for( int i = 0; i < 6; i++ ) {
		NULLADDR[i] = 0;
	}
	this->memberNode = member;
	this->emulNet = emul;
	this->log = log;
	this->par = params;
	this->memberNode->addr = *address;
}

/**
 * Destructor of the MP1Node class
 */
MP1Node::~MP1Node() {}

/**
 * FUNCTION NAME: recvLoop
 *
 * DESCRIPTION: This function receives message from the network and pushes into the queue
 * 				This function is called by a node to receive messages currently waiting for it
 */
int MP1Node::recvLoop() {
    if ( memberNode->bFailed ) {
    	return false;
    }
    else {
    	return emulNet->ENrecv(&(memberNode->addr), enqueueWrapper, NULL, 1, &(memberNode->mp1q));
    }
}

/**
 * FUNCTION NAME: enqueueWrapper
 *
 * DESCRIPTION: Enqueue the message from Emulnet into the queue
 */
int MP1Node::enqueueWrapper(void *env, char *buff, int size) {
	Queue q;
	return q.enqueue((queue<q_elt> *)env, (void *)buff, size);
}

/**
 * FUNCTION NAME: nodeStart
 *
 * DESCRIPTION: This function bootstraps the node
 * 				All initializations routines for a member.
 * 				Called by the application layer.
 */
void MP1Node::nodeStart(char *servaddrstr, short servport) {
    Address joinaddr;
    joinaddr = getJoinAddress();

    // Self booting routines
    if( initThisNode(&joinaddr) == -1 ) {
#ifdef DEBUGLOG
        log->LOG(&memberNode->addr, "init_thisnode failed. Exit.");
#endif
        exit(1);
    }

    if( !introduceSelfToGroup(&joinaddr) ) {
        finishUpThisNode();
#ifdef DEBUGLOG
        log->LOG(&memberNode->addr, "Unable to join self to group. Exiting.");
#endif
        exit(1);
    }

    return;
}

/**
 * FUNCTION NAME: initThisNode
 *
 * DESCRIPTION: Find out who I am and start up
 */
int MP1Node::initThisNode(Address *joinaddr) {
	/*
	 * This function is partially implemented and may require changes
	 */
	int id = *(int*)(&memberNode->addr.addr);
	int port = *(short*)(&memberNode->addr.addr[4]);

	memberNode->bFailed = false;
	memberNode->inited = true;
	memberNode->inGroup = false;
    // node is up!
	memberNode->nnb = 0;
	memberNode->heartbeat = 0;
	memberNode->pingCounter = TFAIL;
	memberNode->timeOutCounter = -1;
    initMemberListTable(memberNode);
	initMembertable(&table);
	this->last_gossip_time = par->getcurrtime();
    return 0;
}

/**
 * FUNCTION NAME: introduceSelfToGroup
 *
 * DESCRIPTION: Join the distributed system
 */
int MP1Node::introduceSelfToGroup(Address *joinaddr) {
	MessageHdr *msg;
#ifdef DEBUGLOG
    static char s[1024];
#endif

    if ( 0 == memcmp((char *)&(memberNode->addr.addr), (char *)&(joinaddr->addr), sizeof(memberNode->addr.addr))) {
        // I am the group booter (first process to join the group). Boot up the group
#ifdef DEBUGLOG
        log->LOG(&memberNode->addr, "Starting up group...");
#endif
        memberNode->inGroup = true;
    }
    else {
        size_t msgsize = sizeof(MessageHdr) + sizeof(joinaddr->addr) + sizeof(long) + 1;
        msg = (MessageHdr *) malloc(msgsize * sizeof(char));

        // create JOINREQ message: format of data is {struct Address myaddr}
        msg->msgType = JOINREQ;
        memcpy((char *)(msg+1), &memberNode->addr.addr, sizeof(memberNode->addr.addr));
        memcpy((char *)(msg+1) + 1 + sizeof(memberNode->addr.addr), &memberNode->heartbeat, sizeof(long));

#ifdef DEBUGLOG
        sprintf(s, "Trying to join...");
        log->LOG(&memberNode->addr, s);
#endif

        // send JOINREQ message to introducer member
        emulNet->ENsend(&memberNode->addr, joinaddr, (char *)msg, msgsize);

        free(msg);
    }

    return 1;

}

/**
 * FUNCTION NAME: finishUpThisNode
 *
 * DESCRIPTION: Wind up this node and clean up state
 */
int MP1Node::finishUpThisNode(){
   /*
    * Your code goes here
    */
	return 1;
}

/**
 * FUNCTION NAME: nodeLoop
 *
 * DESCRIPTION: Executed periodically at each member
 * 				Check your messages in queue and perform membership protocol duties
 */
void MP1Node::nodeLoop() {
    if (memberNode->bFailed) {
    	return;
    }

    // Check my messages
    checkMessages();

    // Wait until you're in the group...
    if( !memberNode->inGroup ) {
    	return;
    }

    // ...then jump in and share your responsibilites!
    nodeLoopOps();

    return;
}

/**
 * FUNCTION NAME: checkMessages
 *
 * DESCRIPTION: Check messages in the queue and call the respective message handler
 */
void MP1Node::checkMessages() {
    void *ptr;
    int size;

    // Pop waiting messages from memberNode's mp1q
    while ( !memberNode->mp1q.empty() ) {
    	ptr = memberNode->mp1q.front().elt;
    	size = memberNode->mp1q.front().size;
    	memberNode->mp1q.pop();
    	recvCallBack((void *)memberNode, (char *)ptr, size);
    }
    return;
}


void MP1Node::update_list(Record_t* table, int current_time, int id, long heartbeat, bool logging, int state)
{
	short port = 0;
	Address remote_addr;

	memcpy(&(remote_addr.addr[0]), &id, sizeof(int));
	memcpy(&(remote_addr.addr[4]), &port, sizeof(short));

	//char* addr = address->addr;
	//memcpy(&id, &addr[0], sizeof(int));
	//memcpy(&port, &addr[4], sizeof(short));

	//To do: State needs to be fixed
	if ((table[id].state != INVALID &&
		table[id].heartbeat < heartbeat) ||
		table[id].state == INVALID
		)
	  {
		if (table[id].state == INVALID)
		{
			table[id].state = VALID;
			if (logging == true)
			{
				log->logNodeAdd(&(this->memberNode->addr), &remote_addr);
				printf("Node %d, adding new node :%d at time: %d\n",
					*((int*)(&memberNode->addr.addr)),
					remote_addr.addr[0],
					par->getcurrtime());
			}
		}
		if (table[id].state == VALID)
		{
			table[id].heartbeat = heartbeat;
			table[id].timestamp = current_time;
		}
	  }



	if (state == REMOVED)
		{
			table[id].state = REMOVED;
			log->logNodeRemove(&(this->memberNode->addr), &remote_addr);
			printf("Node %d, removing node :%d at time: %d\n",
				*((int*)(&memberNode->addr.addr)),
				remote_addr.addr[0],
				par->getcurrtime());
		}
		assert(table[id].port == port);


}

/*void MP1Node::update_list(int current_time, vector<MemberListEntry> *list, Address* address, long heartbeat)
{
	long id=0;
	short port = 0;
	char* addr = address->addr;
	memcpy(&id, &addr[0], sizeof(int));
	memcpy(&port, &addr[4], sizeof(short));

	for (int j = 0;j < list->size();j++)
	{
		if (list->at(j).getid() == id)
		{
			if (list->at(j).getheartbeat() < heartbeat)
			{
				list->at(j).setheartbeat(heartbeat);
				list->at(j).settimestamp(current_time);
			}
			else
			{
				return;
			}

		}

	}
	MemberListEntry* entry = new MemberListEntry(id,port,heartbeat,current_time);
	list->push_back(*entry);
	
}*/


void MP1Node::send_joinrep(EmulNet* ent, Address* dest, Address* src, Record_t* table)
{
	char* joinrep;
	MessageHdr header;
	int id = 0;
	//Update the table with self hearbeat
	char* addr = src->addr;
	memcpy(&id, &addr[0], sizeof(int));
	update_list(table, par->getcurrtime(), id, this->memberNode->heartbeat, true, VALID);
	
	header.msgType = JOINREP;
	int size = sizeof(MessageHdr) + sizeof(Record_t)*(par->EN_GPSZ+1);
	joinrep = (char*)calloc(1,size);
	assert(joinrep != NULL);
	memcpy(joinrep, &header, sizeof(MessageHdr));
	memcpy(joinrep+sizeof(MessageHdr), (char*)table, sizeof(Record_t) * (par->EN_GPSZ+1));
	
	//	for (int i = 0; i < memberList.size(); i++)		
//		joinrep->memberList.push_back(memberList[i]);
	ent->ENsend(src, dest, joinrep, size );

}

void MP1Node::send_gossip(EmulNet* ent, Address* dest, Address* src, Record_t* table)
{
	char* gossip;
	MessageHdr header;
	int id = 0;
	//Update the table with self hearbeat
	char* addr = src->addr;
	memcpy(&id, &addr[0], sizeof(int));
	update_list(table, par->getcurrtime(), id, this->memberNode->heartbeat, false, VALID);

	header.msgType = GOSSIP;
	int size = sizeof(MessageHdr) + sizeof(Record_t) * (par->EN_GPSZ + 1);
	gossip = (char*)calloc(1, size);
	assert(gossip != NULL);
	memcpy(gossip, &header, sizeof(MessageHdr));
	memcpy(gossip + sizeof(MessageHdr), (char*)table, sizeof(Record_t) * (par->EN_GPSZ + 1));

//	for (int i = 0; i < memberList.size(); i++)		
//	joinrep->memberList.push_back(memberList[i]);

	ent->ENsend(src, dest, gossip, size);

}

void MP1Node::check_heartbeat(Record_t* table)
{
	int max = 0, node =-1;
	for (int i = 0;i <= par->EN_GPSZ;i++)
	{
		if (table[i].state != INVALID)
		{
			int delta = par->getcurrtime() - table[i].timestamp;
			if (delta > max)
			{
				max = delta;
				node = i;
			}
			if (delta > TFAIL && table[i].state == VALID)
			{
				table[i].state = FAILED;
				printf("Node %d: delay:%d at time: %d for node:%d- marked failed\n",
					*(int*)(&memberNode->addr.addr), delta,
					par->getcurrtime(), i);
				//log->LOG(&memberNode->addr, "Node failed at time. node: %d delay:%d",i,delta);


			}
			if ((delta > TFAIL + TREMOVE) && (table[i].state == FAILED))
			{
				Address removed_node;
				short port = 0;
				table[i].state = REMOVED;
				printf("Node %d: delay:%d at time: %d for node:%d- removed\n",
					*(int*)(&memberNode->addr.addr), delta,
					par->getcurrtime(), i);
				memcpy(&removed_node.addr[0], &i, sizeof(int));
				memcpy(&removed_node.addr[4], &port, sizeof(short));

				log->logNodeRemove(&(this->memberNode->addr),&removed_node);

			}
		}
	}
	printf("Node %d, max delay:%d at time: %d for node:%d \n",
		*(int*)(&memberNode->addr.addr), max,
			par->getcurrtime(),node);
	
}

/**
 * FUNCTION NAME: recvCallBack
 *
 * DESCRIPTION: Message handler for different message types
 */
void MP1Node::merge_tables(Record_t* rx_table, Record_t* current_table)

{
	for (int i = 0;i <= par->EN_GPSZ;i++)
	{
		if (rx_table[i].state != INVALID && 
			rx_table[i].state != FAILED &&
			rx_table[i].state != REMOVED)
		{
			update_list(this->table,par->getcurrtime(),i, rx_table[i].heartbeat,true, rx_table[i].state);
			check_heartbeat(this->table);
		}


	}


}

int MP1Node::select_neighbor(int* neighbors, Address* addr)
{
	Record_t* record = this->table;
	srand(time(NULL));
	int n = 0;
	for (int i = 0;i <= par->EN_GPSZ;i++)
	{
		if (record[i].state == VALID && // Check if the entry is valid
			i!= int(addr->addr[0]) && // Do not send to yourself
			((rand() % 100) < P_NGR_SELECT) // Pick a neighor with probabilty
			)
		{
			//memcpy(&neighbors[n].addr[0],&record[i].id,sizeof(int));
			//memcpy(&neighbors[n].addr[4],&record[i].port, sizeof(short));
					
			neighbors[n] = i;
			n = n + 1;
		}
		
	}

	return n;
	
}
/**
 * FUNCTION NAME: recvCallBack
 *
 * DESCRIPTION: Message handler for different message types
 */
bool MP1Node::recvCallBack(void *env, char *data, int size ) {

	enum MsgTypes msgid;
	int id = 0;
	Record_t* rx_table = NULL;
	Address* address = new Address();
	long heartbeat = 0;
	msgid = *((MsgTypes *)data);
	Member* memberNode = (Member*)env;
	switch (msgid)
	{
	case JOINREQ:
		memcpy(address->addr, (char*)((MessageHdr*)data + 1), sizeof(memberNode->addr.addr));
		memcpy(&heartbeat, (char*)((MessageHdr*)data + 1) + 1 + sizeof(memberNode->addr.addr), sizeof(long));
#ifdef DEBUGLOG
		log->logNodeAdd(&(memberNode->addr), address);
		printf("Node %d, adding new node :%d at time: %d\n",
			*((int*)(&memberNode->addr.addr)),
			address->addr[0],
			par->getcurrtime());
#endif
		//update_list(par->getcurrtime(), &(memberNode->memberList), address, heartbeat);

		memcpy(&id, &(address->addr[0]), sizeof(int));
		update_list(this->table, par->getcurrtime(), id, heartbeat,false, VALID);
		send_joinrep(emulNet, address, &memberNode->addr, this->table);
		break;

	case JOINREP:
		memberNode->inGroup = true;
		rx_table = (Record_t*) ((char*)data + sizeof(MsgTypes)) ;
		merge_tables(rx_table, this->table);
		break;
	case GOSSIP:
		rx_table = (Record_t*)((char*)data + sizeof(MsgTypes));
		merge_tables(rx_table, this->table);
		break;
     
	default:
		assert(0);

	}



	return 1;
}

/**
 * FUNCTION NAME: nodeLoopOps
 *
 * DESCRIPTION: Check if any node hasn't responded within a timeout period and then delete
 * 				the nodes
 * 				Propagate your membership list
 */
void MP1Node::nodeLoopOps() {
   
	/*Check if its time to Gossip*/
	if (par->getcurrtime() > this->last_gossip_time + TGOSSIP)
	{
		int* neighbors = (int*)calloc(par->EN_GPSZ+1,sizeof(int));
		this->last_gossip_time = par->getcurrtime();
		int n = 0;short port = 0;
		Address dest;
		//Select random neighbors to send gossip
		n = select_neighbor(neighbors, &memberNode->addr);
		for (int i = 0;i < n;i++)
		{   
			//Address dest;
			memcpy(&dest.addr[0], &neighbors[i], sizeof(int));
			memcpy(&dest.addr[4], &port, sizeof(short));
			printf("Node %d, Sending gossip to:%d at time: %d\n",
				  *((int*)(&memberNode->addr.addr)),
				  neighbors[i],
				  par->getcurrtime());
			send_gossip(emulNet, &dest, &memberNode->addr, table);
		}
		free(neighbors);
	}

	this->memberNode->heartbeat++;

    return;
}

/**
 * FUNCTION NAME: isNullAddress
 *
 * DESCRIPTION: Function checks if the address is NULL
 */
int MP1Node::isNullAddress(Address *addr) {
	return (memcmp(addr->addr, NULLADDR, 6) == 0 ? 1 : 0);
}

/**
 * FUNCTION NAME: getJoinAddress
 *
 * DESCRIPTION: Returns the Address of the coordinator
 */
Address MP1Node::getJoinAddress() {
    Address joinaddr;

    memset(&joinaddr, 0, sizeof(Address));
    *(int *)(&joinaddr.addr) = 1;
    *(short *)(&joinaddr.addr[4]) = 0;

    return joinaddr;
}

/**
 * FUNCTION NAME: initMemberListTable
 *
 * DESCRIPTION: Initialize the membership list
 */
void MP1Node::initMemberListTable(Member *memberNode) {
	memberNode->memberList.clear();
	memberNode->memberList.reserve(par->EN_GPSZ+1);
	}
/**
 * FUNCTION NAME: initMemberTable
 *
 * DESCRIPTION: Initialize the membership list
 */
void MP1Node::initMembertable(Record_t** table) {
	int size = par->EN_GPSZ + 1;
	*table = (Record_t*)calloc(size, sizeof(Record_t));

}


/**
 * FUNCTION NAME: printAddress
 *
 * DESCRIPTION: Print the Address
 */
void MP1Node::printAddress(Address *addr)
{
    printf("%d.%d.%d.%d:%d \n",  addr->addr[0],addr->addr[1],addr->addr[2],
                                                       addr->addr[3], *(short*)&addr->addr[4]) ;    
}
