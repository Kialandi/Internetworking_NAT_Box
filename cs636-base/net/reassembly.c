#include <xinu.h>

struct reassembly_entry  reassembly_table[REASSEMBLY_TABLE_MAX_SIZE];

uint8 reassembly_tab_size = 0;  // check later

void print_list(struct frag_desc * list);
void insert_node(struct frag_desc * frag_list, struct frag_desc* new_node);
void insert_frag_list(struct frag_desc * frag_list, byte* ipv6_header);
bool8 byteComp(byte* first, byte* second , uint8 len);
void appendReaTable(byte* ipv6_header);
struct frag_desc * checkReassemblyTable(byte* src_addr, byte * dest_addr, uint32 identif);

status reassembly(struct netpacket*  pkt){


	kprintf("reassembly_tab_size: %d\n", reassembly_tab_size);
	
	// check whether same src addr, dest addr and fragmentation identification
	struct base_header * ipv6_header = (struct base_header * ) pkt->net_payload;
	struct fragment_header * frag_header = (struct fragment_header *) (pkt->net_payload + IPV6_HDR_LEN); 
	byte * src_addr = ipv6_header->src;
	byte * dest_addr = ipv6_header->dest;
	uint32 identif = ntohl(frag_header->identif);
	
	struct frag_desc * frag_list = checkReassemblyTable(src_addr, dest_addr, identif);
	if (frag_list == NULL) {
		kprintf("frag_list is NULL after check\n");	 
		appendReaTable(ipv6_header);
        //	print_list(frag_list); 
	} else {
		kprintf("frag_list is NOT NULL\n");
		// insert into frag_list in the ascending order of offset		
		insert_frag_list(frag_list, ipv6_header); 
		// check M flag of fragment_header , if it is 0.  then we try to put all nodes into datagram. 
       		print_list(frag_list);
	 }
   	
// TO DO:
// check identification mismatch (DONE)

// check M flag in fragment header to check : if it is the last fragment, we will copy all the payload into Datagram

// check memory problem	
	
// enable specify IPV6 addresss. It can be deal later. 	
	return 0;
}

void appendReaTable(byte * ipv6_header) {

	uint16 headers_len = IPV6_HDR_LEN; // TODO: if there are some other headers between base_header and fragment header, this value should be changed, 
	struct base_header * temp_ipv6 = (struct base_header*) ipv6_header;
	struct fragment_header * frag_header = (struct fragment_header *) (ipv6_header + headers_len);
	byte * src_addr = temp_ipv6->src;
	byte * dest_addr = temp_ipv6->dest;
	uint32 identif = ntohl(frag_header->identif);

	struct reassembly_entry * entry = reassembly_table + reassembly_tab_size;
	memcpy(&(entry->src_addr), src_addr, IPV6_ASIZE);
	memcpy(&(entry->dest_addr), dest_addr, IPV6_ASIZE);
	entry->identif = identif;
	// copy perfragment headers of the first packet
	memcpy(&(entry->perfrag_headers), ipv6_header, headers_len);
	entry->headers_len = headers_len;		

	// construct a frag_list
	struct frag_desc * frag_list = (struct frag_desc *) getmem(sizeof(struct frag_desc));
	frag_list->payload = NULL;
        frag_list->prev = NULL;
	frag_list->next = NULL;
	
	insert_frag_list(frag_list, ipv6_header) ;
    
        entry->frag_list = frag_list;	
       	
	reassembly_tab_size++;
	kprintf("I am in append table, table size: %d\n", reassembly_tab_size);
}

void insert_frag_list(struct frag_desc * frag_list, byte* ipv6_header){
	struct  base_header * temp_ipv6 = (struct base_header  *) ipv6_header;
        // get offset from fragment header
	struct fragment_header * frag_header = (struct fragment_header *) (ipv6_header + IPV6_HDR_LEN); //TODO: Assuming only ipv6 base header ahead of fragment header.
        uint16 offset = ntohs(frag_header->offset);
    	kprintf("inser_frag_list: offset: %d\n", offset); 

 	// get payload following after the fragment header and the payload length for this payload  
	uint16 payload_len_in_header = ntohs(temp_ipv6->payload_len);
	kprintf("insert_frag_List: payload_len_in_header : %u\n", payload_len_in_header);
	uint16 payload_len  = payload_len_in_header - sizeof(struct fragment_header);  // TODO: if there are some other headers between baseheader and fragment header, update this line.
        char* payload_from = (char *) frag_header + sizeof(struct fragment_header);

	// construct new node
	struct frag_desc * new_node = (struct frag_desc* ) getmem(sizeof(struct frag_desc));
        new_node->offset = offset;
	new_node->payload_len = payload_len;
	new_node->payload = (char*) getmem(payload_len);
	memcpy(new_node->payload, payload_from, payload_len);	
	new_node->prev = NULL;
	new_node->next = NULL;    

	// insert the new node to list
	insert_node(frag_list, new_node);			
}

void print_list(struct frag_desc * list) {
	struct frag_desc * next = list->next;
	kprintf("printing list starting\n");
	while(next->payload != NULL) {
		kprintf("a node is printing:\n");
		kprintf("offset: %u\n", next->offset);
		kprintf("payload_len: %u\n", next->payload_len);
		next = next->next;
	}
	kprintf("printing list ending\n");
	
}

void insert_node(struct frag_desc * frag_list, struct frag_desc* new_node) {
	struct frag_desc * tail = frag_list->prev; // since most of packet will be in order, we start comparison from the end
	uint8 count = 0;
	while(tail != NULL && tail->payload != NULL) {
		if (count == 3) {
			break;
		}
		kprintf("in while loop\n");
		if (new_node->offset > tail->offset) {
			kprintf("I am in insert_node: new node offset > tail offset\n");
			// insert the node next tail
			new_node->prev = tail;
			new_node->next = tail->next;
			tail->next->prev= new_node; 	
			tail->next = new_node;

			return;
		}else if (new_node->offset < tail->offset) {
			tail = tail -> prev;
  		} 
		count++;
        }
	// Execution reaches here means the current tail is the dummy node or sentinel node of the list
	// so we should insert the new node after the dummy node
	if (tail == NULL) {  // in case of no node following the sentinel node
		kprintf("I am in insert_node: tail == NULL\n");
		new_node->prev = frag_list;
		new_node->next = frag_list;
		frag_list->prev= new_node; 	
		frag_list->next = new_node;
	} else {  // in case that there are real nodes following the sentinel node.  we traverse back to the beginning of the list , in this case one of the first few packets comes too late.
			
		new_node->prev = tail;
		new_node->next = tail->next;
		tail->next->prev = new_node;
		tail->next = new_node;

       	}	
	
	return;
 
}

struct frag_desc * checkReassemblyTable(byte* src_addr, byte * dest_addr, uint32 identif) {
	uint8 i;
	for (i = 0 ; i < reassembly_tab_size; i ++){
		struct reassembly_entry * temp = reassembly_table + i;
		if (byteComp(src_addr, temp->src_addr, IPV6_ASIZE) == TRUE && byteComp(dest_addr, temp->dest_addr, IPV6_ASIZE) == TRUE  && identif == temp->identif) {
	
    			kprintf("I find reassembly entry with identif : %u\n", temp->identif);
			return temp->frag_list;
        	}
        }

	return NULL;

}

bool8 byteComp(byte* first, byte* second , uint8 len) {
	uint8 i;
	for (i = 0; i < len && first[i] == second[i]; i++) {}
	
	if (i == len) return TRUE;

	return FALSE;
}

