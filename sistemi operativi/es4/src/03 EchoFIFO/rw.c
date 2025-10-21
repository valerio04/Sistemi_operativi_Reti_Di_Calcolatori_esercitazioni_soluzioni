#include <unistd.h>
#include <errno.h>
#include "common.h"


int readOneByOne(int fd, char* buf, char separator) {

    int ret;

    /** [TO DO] READ THE MESSAGE THROUGH THE FIFO DESCRIPTOR
     *
     * Suggestions:
     * - you can read from a FIFO as from a regular file descriptor
     * - since you don't know the length of the message, just
     *   read one byte at a time from the socket
     * - leave the cycle when 'separator' ('\n') is encountered 
     * - repeat the read() when interrupted before reading any data
     * - return the number of bytes read
     * - reading 0 bytes means that the other process has closed
     *   the FIFO unexpectedly: this is an error that should be
     *   dealt with!
     **/
    int bytes_reads=0;
    char c;
    while(1){
        ret=read(fd,&c,1);
    if(ret==0)
        handle_error("[read one by one], fifo chiusa");
    if(ret==-1){
        if (errno == EINTR) continue;
        handle_error("[read one by one], errore read");

    }
     
    
    buf[bytes_reads]=c;
    bytes_reads++;

    if(c==separator || bytes_reads>=1023) break;
    };

    return bytes_reads;
    

    

    

}

void writeMsg(int fd, char* buf, int size) {

    int ret, bytes_sent=0;
    /** [TO DO] SEND THE MESSAGE THROUGH THE FIFO DESCRIPTOR
     *
     * Suggestions:
     * - you can write on the FIFO as on a regular file descriptor
     * - make sure that all the bytes have been written: use a while
     *   cycle in the implementation as we did for file descriptors!
     **/
    
    while(bytes_sent<size){
        ret=write(fd,buf+bytes_sent,size-bytes_sent);
        if (ret == -1) {
            if (errno == EINTR) continue; 
            handle_error("[writeMsg] errore write");
        }
        bytes_sent+=ret;
    }
    

}
