/**********************************
 * FILE NAME: MP2Node.cpp
 *
 * DESCRIPTION: MP2Node class definition
 **********************************/
#include "MP2Node.h"

unsigned int get_transaction_id()
{
	static unsigned int trans_id = 0;
	trans_id++;
	assert(trans_id < MAX_TRANS);
	return trans_id;
}


/**
 * constructor
 */
MP2Node::MP2Node(Member *memberNode, Params *par, EmulNet * emulNet, Log * log, Address * address) {
	this->memberNode = memberNode;
	this->par = par;
	this->emulNet = emulNet;
	this->log = log;
	ht = new HashTable();
	trans = (transaction_t*)calloc(MAX_TRANS,sizeof(transaction_t));
	//type = (MessageType*)calloc(MAX_TRANS, sizeof(MessageType));
	//memset(type,10, sizeof(int)* MAX_TRANS);
	this->memberNode->addr = *address;
}

/**
 * Destructor
 */
MP2Node::~MP2Node() {
	delete ht;
	delete memberNode;
	free(trans);

}

/**
 * FUNCTION NAME: updateRing
 *
 * DESCRIPTION: This function does the following:
 * 				1) Gets the current membership list from the Membership Protocol (MP1Node)
 * 				   The membership list is returned as a vector of Nodes. See Node class in Node.h
 * 				2) Constructs the ring based on the membership list
 * 				3) Calls the Stabilization Protocol
 */
void MP2Node::updateRing() {
	/*
	 * Implement this. Parts of it are already implemented
	 */
	vector<Node> curMemList;
	bool change = false;

	/*
	 *  Step 1. Get the current membership list from Membership Protocol / MP1
	 */
	curMemList = getMembershipList();

	/*
	 * Step 2: Construct the ring
	 */
	// Sort the list based on the hashCode
	sort(curMemList.begin(), curMemList.end());
	this->ring = curMemList;

	/*
	 * Step 3: Run the stabilization protocol IF REQUIRED
	 */
	// Run stabilization protocol if the hash table size is greater than zero and if there has been a changed in the ring
}

/**
 * FUNCTION NAME: getMemberhipList
 *
 * DESCRIPTION: This function goes through the membership list from the Membership protocol/MP1 and
 * 				i) generates the hash code for each member
 * 				ii) populates the ring member in MP2Node class
 * 				It returns a vector of Nodes. Each element in the vector contain the following fields:
 * 				a) Address of the node
 * 				b) Hash code obtained by consistent hashing of the Address
 */
vector<Node> MP2Node::getMembershipList() {
	unsigned int i;
	vector<Node> curMemList;
	for ( i = 0 ; i < this->memberNode->memberList.size(); i++ ) {
		Address addressOfThisMember;
		int id = this->memberNode->memberList.at(i).getid();
		short port = this->memberNode->memberList.at(i).getport();
		memcpy(&addressOfThisMember.addr[0], &id, sizeof(int));
		memcpy(&addressOfThisMember.addr[4], &port, sizeof(short));
		curMemList.emplace_back(Node(addressOfThisMember));
	}
	return curMemList;
}

/**
 * FUNCTION NAME: hashFunction
 *
 * DESCRIPTION: This functions hashes the key and returns the position on the ring
 * 				HASH FUNCTION USED FOR CONSISTENT HASHING
 *
 * RETURNS:
 * size_t position on the ring
 */
size_t MP2Node::hashFunction(string key) {
	std::hash<string> hashFunc;
	size_t ret = hashFunc(key);
	return ret%RING_SIZE;
}


void MP2Node::init_trans(int transID, MessageType type, string key, string value, int start_time)
{
	assert(transID < MAX_TRANS);
	trans[transID].type = type;
	trans[transID].success = 0;
	trans[transID].fail = 0;
	trans[transID].state = STARTED;
	trans[transID].start_time = start_time;
	strcpy(trans[transID].key, key.c_str());
	strcpy(trans[transID].value, value.c_str());

}
/**
 * FUNCTION NAME: clientCreate
 *
 * DESCRIPTION: client side CREATE API
 * 				The function does the following:
 * 				1) Constructs the message
 * 				2) Finds the replicas of this key
 * 				3) Sends a message to the replica
 */
void MP2Node::clientCreate(string key, string value) {
	 
	vector<Node> addr_vec= findNodes(key);
	unsigned int transID = get_transaction_id();
	for (int replica = 0; replica < 3;replica++)
	{
		Message* msg = new Message(transID, this->memberNode->addr,
			CREATE, key, value, (ReplicaType) replica);
		string payload = msg->toString();
		const char* char_payload = payload.c_str();

		emulNet->ENsend(&(this->memberNode->addr),
						&addr_vec[replica].nodeAddress,
						  (char*)char_payload,
			              payload.size());

		printf("Node %d, sending Create key: %s, value: %s to node: %d at time:%d \n",
			*((int*)(&memberNode->addr.addr)),
			key.c_str(),
			value.c_str(),
			addr_vec[replica].nodeAddress.addr[0],
			par->getcurrtime());

	}

	/*Initalize pending read responses*/
	init_trans(transID, CREATE, key,value, par->getcurrtime());

}

/**
 * FUNCTION NAME: clientRead
 *
 * DESCRIPTION: client side READ API
 * 				The function does the following:
 * 				1) Constructs the message
 * 				2) Finds the replicas of this key
 * 				3) Sends a message to the replica
 */
void MP2Node::clientRead(string key){

	vector<Node> addr_vec = findNodes(key);
	unsigned int transID = get_transaction_id();
	for (int replica = 0; replica < 3;replica++)
	{

		Message* msg = new Message(transID, this->memberNode->addr,
			READ, key);
		string payload = msg->toString();
		const char* char_payload = payload.c_str();

		emulNet->ENsend(&(this->memberNode->addr),
			&addr_vec[replica].nodeAddress,
			(char*)char_payload,
			payload.size());

		printf("Node %d, trans_id:%d, sending read request key: %s, to node: %d at time:%d \n",
			*((int*)(&memberNode->addr.addr)),
			transID,
			key.c_str(),
			addr_vec[replica].nodeAddress.addr[0],
			par->getcurrtime());

	}

	/*Initalize pending read responses*/
	init_trans(transID,READ, key,"NA", par->getcurrtime());

}

/**
 * FUNCTION NAME: clientUpdate
 *
 * DESCRIPTION: client side UPDATE API
 * 				The function does the following:
 * 				1) Constructs the message
 * 				2) Finds the replicas of this key
 * 				3) Sends a message to the replica
 */
void MP2Node::clientUpdate(string key, string value){
	vector<Node> addr_vec = findNodes(key);
	unsigned int transID = get_transaction_id();
	for (int replica = 0; replica < 3;replica++)
	{
		Message* msg = new Message(transID, this->memberNode->addr,
			UPDATE, key, value, (ReplicaType)replica);
		string payload = msg->toString();
		const char* char_payload = payload.c_str();

		emulNet->ENsend(&(this->memberNode->addr),
			&addr_vec[replica].nodeAddress,
			(char*)char_payload,
			payload.size());

		printf("Node %d, sending update key: %s, value: %s to node: %d at time:%d \n",
			*((int*)(&memberNode->addr.addr)),
			key.c_str(),
			value.c_str(),
			addr_vec[replica].nodeAddress.addr[0],
			par->getcurrtime());

	}

	/*Initalize pending update responses*/
	init_trans(transID, UPDATE, key,value, par->getcurrtime());

}

/**
 * FUNCTION NAME: clientDelete
 *
 * DESCRIPTION: client side DELETE API
 * 				The function does the following:
 * 				1) Constructs the message
 * 				2) Finds the replicas of this key
 * 				3) Sends a message to the replica
 */
void MP2Node::clientDelete(string key){
	vector<Node> addr_vec = findNodes(key);
	unsigned int transID = get_transaction_id();
	for (int replica = 0; replica < 3;replica++)
	{

		Message* msg = new Message(transID, this->memberNode->addr,
			DELETE, key);
		string payload = msg->toString();
		const char* char_payload = payload.c_str();

		emulNet->ENsend(&(this->memberNode->addr),
			&addr_vec[replica].nodeAddress,
			(char*)char_payload,
			payload.size());

		printf("Node %d, trans_id:%d, sending delete request key: %s, to node: %d at time:%d \n",
			*((int*)(&memberNode->addr.addr)),
			transID,
			key.c_str(),
			addr_vec[replica].nodeAddress.addr[0],
			par->getcurrtime());

	}

	/*Initalize delete read responses*/
	init_trans(transID, DELETE, key,"NA", par->getcurrtime());

}

/**
 * FUNCTION NAME: createKeyValue
 *
 * DESCRIPTION: Server side CREATE API
 * 			   	The function does the following:
 * 			   	1) Inserts key value into the local hash table
 * 			   	2) Return true or false based on success or failure
 */
bool MP2Node::createKeyValue(string key, string value, ReplicaType replica) {
	// Insert key, value, replicaType into the hash table

	bool status = this->ht->create(key,value);
		   	 
	return status;
}

/**
 * FUNCTION NAME: readKey
 *
 * DESCRIPTION: Server side READ API
 * 			    This function does the following:
 * 			    1) Read key from local hash table
 * 			    2) Return value
 */
string MP2Node::readKey(string key) {
	// Read key from local hash table and return value
	string read = ht->read(key);
	if (read!= string(""))
	{
		Entry* ent1 = new Entry(read);
		return ent1->value;
	}
	else
	{
		return read;
	}

}

/**
 * FUNCTION NAME: updateKeyValue
 *
 * DESCRIPTION: Server side UPDATE API
 * 				This function does the following:
 * 				1) Update the key to the new value in the local hash table
 * 				2) Return true or false based on success or failure
 */
bool MP2Node::updateKeyValue(string key, string value, ReplicaType replica) {

	// Update key in local hash table and return true or false

	bool status = this->ht->update(key, value);

	return status;

}

/**
 * FUNCTION NAME: deleteKey
 *
 * DESCRIPTION: Server side DELETE API
 * 				This function does the following:
 * 				1) Delete the key from the local hash table
 * 				2) Return true or false based on success or failure
 */
bool MP2Node::deletekey(string key) {

	// Delete the key from the local hash table
	bool status = this->ht->deleteKey(key);

	return status;
}


void MP2Node::send_reply(Address to_addr, int trans_id, bool success, string key)
{
	Message* msg = new Message(trans_id, this->memberNode->addr, REPLY, success);
	string payload = msg->toString();
	const char* char_payload = payload.c_str();

	emulNet->ENsend(&(this->memberNode->addr),
		&to_addr,
		(char*)char_payload,
		payload.size());

	printf("Node %d, sending create/write/delete/read fail reply  trans id: %d, key:%s, to node: %d at time:%d, status:%d \n",
		*((int*)(&memberNode->addr.addr)),
		trans_id,
		key.c_str(),
		to_addr.addr[0],
		par->getcurrtime(),
	    success);

}

void MP2Node::check_timeout_failure()
{
	for (int i = 0;i < MAX_TRANS;i++)
	{
		if(trans[i].state == STARTED && 
		  trans[i].start_time + FAIL_TIMEOUT < par->getcurrtime())
		{ 
			trans[i].state = COMPLETED;
			trans[i].fail = 3 - trans[i].success;
			printf("Node %d, timing out trans id: %d, key:%s, at time:%d, start time:%d \n",
				*((int*)(&memberNode->addr.addr)),
				i,
				trans[i].key,
				par->getcurrtime(),
				trans[i].start_time);

			if (trans[i].type == CREATE)
				log->logCreateFail(&memberNode->addr, true, i, string(trans[i].key), string(trans[i].value));
			if (trans[i].type == UPDATE)
				log->logUpdateFail(&memberNode->addr, true, i, string(trans[i].key), string(trans[i].value));
			if (trans[i].type == DELETE)
				log->logDeleteFail(&memberNode->addr, true, i, string(trans[i].key));
			if (trans[i].type == READ)
				log->logReadFail(&memberNode->addr, true, i, string(trans[i].key));

			stabilizationProtocol(trans[i].key, trans[i].value);
		}

	}


}


void MP2Node::send_read_reply(Address to_addr, int trans_id, string read_value, string key)
{

	Message* msg = new Message(trans_id, this->memberNode->addr, read_value);

	string payload = msg->toString();
	const char* char_payload = payload.c_str();

	emulNet->ENsend(&(this->memberNode->addr),
		&to_addr,
		(char*)char_payload,
		payload.size());

	printf("Node %d, sending read reply  trans id: %d, key:%s, to node: %d at time:%d, value:%s \n",
		*((int*)(&memberNode->addr.addr)),
		trans_id,
		key.c_str(),
		to_addr.addr[0],
		par->getcurrtime(),
		read_value.c_str());
}

StatusType MP2Node::check_quorm(int transID, bool success, string value)
{
	StatusType status = DONE;
	success == true ? trans[transID].success++ : trans[transID].fail++;
	if (trans[transID].state == COMPLETED)
	{
		status = DONE;
	}

	else if (trans[transID].success > 1)
	{
		status = PASS;
		trans[transID].state = COMPLETED;
	}
	else if (trans[transID].fail > 1)
	{
		status = FAIL;
		trans[transID].state = COMPLETED;
	}
	else if(trans[transID].fail <= 1 && trans[transID].success <= 1)
	{
		status = PENDING;

	}
		
	printf("Node %d, quorm status at co-ordinator trans id: %d, success:%d, fail:%d, status:%d \n",
		*((int*)(&memberNode->addr.addr)), transID, trans[transID].success, trans[transID].fail, status );
	return status;
}


/**
 * FUNCTION NAME: checkMessages
 *
 * DESCRIPTION: This function is the message handler of this node.
 * 				This function does the following:
 * 				1) Pops messages from the queue
 * 				2) Handles the messages according to message types
 */
void MP2Node::checkMessages() {
	/*
	 * Implement this. Parts of it are already implemented
	 */
	char * data;
	int size;

	/*
	 * Declare your local variables here
	 */

	// dequeue all messages and handle them
	while ( !memberNode->mp2q.empty() ) {
		/*
		 * Pop a message from the queue
		 */
		data = (char *)memberNode->mp2q.front().elt;
		size = memberNode->mp2q.front().size;
		memberNode->mp2q.pop();
		string message(data, data + size);
		Message msg(message);
		Entry* ent=NULL;
		bool result;
		string read_value;

		printf("Node %d, received message type: %d, trans_id:%d, from: %d at time:%d \n",
			memberNode->addr.addr[0],
			msg.type,
			msg.transID,
			msg.fromAddr.addr[0],
			par->getcurrtime());


		switch (msg.type)
		{
		case CREATE:
			ent = new Entry(msg.value, par->getcurrtime(), msg.replica);
			result = createKeyValue(msg.key, ent->convertToString(), msg.replica);
			send_reply(msg.fromAddr, msg.transID, result, msg.key);
			if(!result)
			{
				printf("Node %d, create request failed!! key : %s, tran_id: %d at time:%d \n",
					*((int*)(&memberNode->addr.addr)),
					msg.key.c_str(),
					msg.transID,
					par->getcurrtime());
				log->logCreateFail(&memberNode->addr, false, msg.transID, msg.key, msg.value);

			}
			else
			{
				log->logCreateSuccess(&memberNode->addr, false, msg.transID, msg.key, msg.value);

			}

			break;
		case READ:
			read_value = readKey(msg.key);
			if (read_value!= string("") )
			{
				log->logReadSuccess(&memberNode->addr, false, msg.transID, msg.key, read_value);
				send_read_reply(msg.fromAddr, msg.transID, read_value,msg.key);
			}
			else
			{
				log->logReadFail(&memberNode->addr, false, msg.transID, msg.key);
				printf("Node %d, read request failed!! key : %s, tran_id: %d at time:%d \n",
					*((int*)(&memberNode->addr.addr)),
					msg.key.c_str(),
					msg.transID,
					par->getcurrtime());
				send_reply(msg.fromAddr, msg.transID, false, msg.key);

			}
			
			break;
		case UPDATE:
			ent = new Entry(msg.value, par->getcurrtime(), msg.replica);
			result = updateKeyValue(msg.key, ent->convertToString(), msg.replica);
			send_reply(msg.fromAddr, msg.transID, result, msg.key);
			if (!result)
			{
				printf("Node %d, update request failed!! key : %s, tran_id: %d at time:%d \n",
					*((int*)(&memberNode->addr.addr)),
					msg.key.c_str(),
					msg.transID,
					par->getcurrtime());
				log->logUpdateFail(&memberNode->addr, false, msg.transID, msg.key, msg.value);

			}
			else
			{
				log->logUpdateSuccess(&memberNode->addr, false, msg.transID, msg.key, msg.value);

			}
			break;
		case DELETE:
			result = deletekey(msg.key);
			send_reply(msg.fromAddr, msg.transID, result, msg.key);
			if (!result)
			{
				printf("Node %d, delete request failed!! key : %s, tran_id: %d at time:%d \n",
					*((int*)(&memberNode->addr.addr)),
					msg.key.c_str(),
					msg.transID,
					par->getcurrtime());
				log->logDeleteFail(&memberNode->addr, false, msg.transID, msg.key);
			}
			else
			{
				log->logDeleteSuccess(&memberNode->addr, false, msg.transID, msg.key);
			}
			break;
		case REPLY:
			if (msg.success)
			{
				StatusType status = check_quorm(msg.transID,true,msg.value);
				MessageType msg_type = trans[msg.transID].type ;
				assert(msg_type < READREPLY && msg_type >= CREATE);
				if (status == PASS)
				{
					if (msg_type == CREATE)
						log->logCreateSuccess(&memberNode->addr, true, msg.transID, string(trans[msg.transID].key), string(trans[msg.transID].value));
					if (msg_type == UPDATE)
						log->logUpdateSuccess(&memberNode->addr, true, msg.transID, string(trans[msg.transID].key), string(trans[msg.transID].value));
					if (msg_type == DELETE)
						log->logDeleteSuccess(&memberNode->addr, true, msg.transID, string(trans[msg.transID].key));
					
									   
				}
			}
			else
			{
				MessageType msg_type = trans[msg.transID].type;
				assert(msg_type < READREPLY&& msg_type >= CREATE);
				StatusType status = check_quorm(msg.transID, false, msg.value);
				if (status == FAIL)
				{
					if (msg_type == CREATE)
						log->logCreateFail(&memberNode->addr, true, msg.transID, string(trans[msg.transID].key), string(trans[msg.transID].value));
					if (msg_type == UPDATE)
						log->logUpdateFail(&memberNode->addr, true, msg.transID, string(trans[msg.transID].key), string(trans[msg.transID].value));
					if (msg_type == DELETE)
						log->logDeleteFail(&memberNode->addr, true, msg.transID, string(trans[msg.transID].key));
					if (msg_type == READ)
						log->logReadFail(&memberNode->addr, true, msg.transID, string(trans[msg.transID].key));
				}
			}
			break;

		case READREPLY:
		{
			StatusType status = check_quorm(msg.transID, true, msg.value);
			strcpy(trans[msg.transID].value, msg.value.c_str());
			if (status==PASS)
			{
				log->logReadSuccess(&memberNode->addr, true, msg.transID, string(trans[msg.transID].key), msg.value);
			}
		}
			break;
		default:
			assert(0);

		}


		}
		

		check_timeout_failure();

	/*
	 * This function should also ensure all READ and UPDATE operation
	 * get QUORUM replies
	 */
}

/**
 * FUNCTION NAME: findNodes
 *
 * DESCRIPTION: Find the replicas of the given keyfunction
 * 				This function is responsible for finding the replicas of a key
 */
vector<Node> MP2Node::findNodes(string key) {
	size_t pos = hashFunction(key);
	vector<Node> addr_vec;
	if (ring.size() >= 3) {
		// if pos <= min || pos > max, the leader is the min
		if (pos <= ring.at(0).getHashCode() || pos > ring.at(ring.size()-1).getHashCode()) {
			addr_vec.emplace_back(ring.at(0));
			addr_vec.emplace_back(ring.at(1));
			addr_vec.emplace_back(ring.at(2));
		}
		else {
			// go through the ring until pos <= node
			for (int i=1; i<ring.size(); i++){
				Node addr = ring.at(i);
				if (pos <= addr.getHashCode()) {
					addr_vec.emplace_back(addr);
					addr_vec.emplace_back(ring.at((i+1)%ring.size()));
					addr_vec.emplace_back(ring.at((i+2)%ring.size()));
					break;
				}
			}
		}
	}
	return addr_vec;
}

/**
 * FUNCTION NAME: recvLoop
 *
 * DESCRIPTION: Receive messages from EmulNet and push into the queue (mp2q)
 */
bool MP2Node::recvLoop() {
    if ( memberNode->bFailed ) {
    	return false;
    }
    else {
    	return emulNet->ENrecv(&(memberNode->addr), this->enqueueWrapper, NULL, 1, &(memberNode->mp2q));
    }
}

/**
 * FUNCTION NAME: enqueueWrapper
 *
 * DESCRIPTION: Enqueue the message from Emulnet into the queue of MP2Node
 */
int MP2Node::enqueueWrapper(void *env, char *buff, int size) {
	Queue q;
	return q.enqueue((queue<q_elt> *)env, (void *)buff, size);
}
/**
 * FUNCTION NAME: stabilizationProtocol
 *
 * DESCRIPTION: This runs the stabilization protocol in case of Node joins and leaves
 * 				It ensures that there always 3 copies of all keys in the DHT at all times
 * 				The function does the following:
 *				1) Ensures that there are three "CORRECT" replicas of all the keys in spite of failures and joins
 *				Note:- "CORRECT" replicas implies that every key is replicated in its two neighboring nodes in the ring
 */
void MP2Node::stabilizationProtocol(string key, string value) {

	updateRing();
	clientCreate(key, value);

}
