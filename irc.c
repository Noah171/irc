#include "irc.h"

// Returns sockets connected to the irc
int irc_init(char * addr){
  
  int sockfd = -1;
  struct addrinfo hints;
  struct addrinfo * results = NULL;
  
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  for( int port = 6660; port <=6670; ++port){
    const int port_len = 5;
    char str_port[port_len];
    sprintf(str_port, "%d", port); // The port is needed in string format
    // initalize the 'results' linked list
    // Zero is the success number, so if this does not succeed then printer error and exit
    if(getaddrinfo(addr, str_port, &hints, &results) != 0){
      perror("In getaddrinfo");
    }
    // assign sockfd using the information in results acquired from getaddrinfo above.
    // Upon failure, socket returns -1, so print the error
    if((sockfd = socket(results->ai_family,
			results->ai_socktype,
			results->ai_protocol)) == -1){
      perror("In socket");
    }
    // Connect using the initialized socket.
    // If connection succeeds (return of connect is 0) CONTINUE 
    if(connect(sockfd, results->ai_addr,
	       results->ai_addrlen) == 0){
      break;
    }
    freeaddrinfo(results);
  }
  
  return sockfd;
}

// Returns bytes sent
size_t irc_send(int sockfd, char * str){
  
  const char* suffix = "\r\n";
  const size_t suffix_len = strlen(suffix);
  int sent_len = 0;
  int str_len = strlen(str);
  int final_sent_len = -1;

  // Because send does not guarentee that the entire message is sent at once, the amount
  // of data sent from the requested string is returned, it is up to us to return the
  /* rest. */
  do {
    
    // Give the address of where the unsent string begins (on first iteration will be the
    // string's beginning, index 0)
    char * unsent_string_start = str+sent_len;
    size_t unsent_string_length = strlen(unsent_string_start);
    
    char * send_str = malloc(BUFF_LEN);
    bzero(send_str, BUFF_LEN);
    // Construct the string of the message plus the suffix. str + sent_len and sent_str +
    // cur_len give the DESIRED ADDRESSES, because str[sent_len] gives *(str + sent_len)
    strncpy(send_str, unsent_string_start, BUFF_LEN);
    strncpy(send_str + unsent_string_length, suffix, BUFF_LEN);
    
    // The final length of the string to send being assigned. Declared at function top.
    final_sent_len = unsent_string_length + suffix_len;
    if((sent_len += send(sockfd, send_str, final_sent_len, 0)) == -1){
      perror("In send");
    }

    // This is required to ensure that the entire message is sent, because the suffix
    // length otherwise contributes to the number of characters sent, when it shouldn't
    // because the suffix isn't part of the actual message
    sent_len -= suffix_len; 
  }while(sent_len < str_len);
  
  return sent_len;
}

// Returns bytes received and copies message into buffer
size_t irc_recv(int sockfd, char * buff, size_t bufflen){

  int len_recvd = -1;
  unsigned short len_real = 0;
  // Due to network protocol the difference between the length recieved from the peer
  // and the length of the actual information is two. This is because information sent
  // through sockets is ended with "\r\n". The length recieved points to one byte after
  // this sequence, so, because we don't need the \r or \n, and the information
  // immediately preceeds \r, we take away two from the length to replace \r with a null
  // byte. This two represents the two taken away from the length recieved from our peer.
  const short recv_real_diff = 2;
  len_recvd = recv(sockfd, buff, bufflen, 0);
  if(len_recvd == -1){
    perror("In receiving");
  }

  len_real = len_recvd-recv_real_diff;
  buff[len_real] = '\0';
  
  return len_recvd;
}
