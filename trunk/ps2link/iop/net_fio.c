/*********************************************************************
 * Copyright (C) 2003 Tord Lindstrom (pukko@home.se)
 * This file is subject to the terms and conditions of the PS2Link License.
 * See the file LICENSE in the main directory of this distribution for more
 * details.
 */

// Fu*k knows why net_fio & net_fsys are separated..

#include "ps2link.h"

#include "net_fio.h"

unsigned int remote_pc_addr = 0xffffffff;

#define PACKET_MAXSIZE 4096

static char send_packet[PACKET_MAXSIZE] __attribute__((aligned(16)));
static char recv_packet[PACKET_MAXSIZE] __attribute__((aligned(16)));

static int pko_fileio_sock = -1;
static int pko_fileio_active = 0;

//---------------------------------------------------------------------- 
//
void 
pko_close_socket(void)
{
    int ret;

    ret = disconnect(pko_fileio_sock);
    if (ret < 0) {
        printf("pko_file: disconnect returned error %d\n", ret);
    }
    pko_fileio_sock = -1;
}

//---------------------------------------------------------------------- 
//
void
pko_close_fsys(void)
{
    if (pko_fileio_sock > 0) {
        disconnect(pko_fileio_sock);
    }
    pko_fileio_active = 0;
    return;
}

//---------------------------------------------------------------------- 
// XXX: Hm, this func should behave sorta like pko_recv_bytes imho..
// i.e. check if it was able to send just a part of the packet etc..
static inline int 
pko_lwip_send(int sock, void *buf, int len, int flag)
{
    int ret;
    ret = send(sock, buf, len, flag);
    if (ret < 0) {
        dbgprintf("pko_file: lwip_send() error %d\n", ret);
        pko_close_socket();
        return -1;
    }
    else {
        return ret;
    }
}


//---------------------------------------------------------------------- 
// Do repetetive recv() calles until 'bytes' bytes are received
// or error returned
int pko_recv_bytes(int sock, char *buf, int bytes)
{
    int left;
    int len;

    left = bytes;

    while (left > 0) {
        len = recv(sock, &buf[bytes - left], left, 0);
        if (len < 0) {
            dbgprintf("pko_file: pko_recv_bytes error!!\n");
            return -1;
        }
        left -= len;
    }
    return bytes;
}


//---------------------------------------------------------------------- 
// Receive a 'packet' of the expected type 'pkt_type', and lenght 'len'
int pko_accept_pkt(int sock, char *buf, int len, int pkt_type)
{
    int length;
    pko_pkt_hdr *hdr;
    unsigned int hcmd;
    unsigned short hlen;


    length = pko_recv_bytes(sock, buf, sizeof(pko_pkt_hdr));
    if (length < 0) {
        dbgprintf("pko_file: accept_pkt recv error\n");
        return -1;
    }

    if (length < sizeof(pko_pkt_hdr)) {
        dbgprintf("pko_file: XXX: did not receive a full header!!!! "
                  "Fix this! (%d)\n", length);
    }
    
    hdr = (pko_pkt_hdr *)buf;
    hcmd = ntohl(hdr->cmd);
    hlen = ntohs(hdr->len);

    if (hcmd != pkt_type) {
        dbgprintf("pko_file: pko_accept_pkt: Expected %x, got %x\n", 
                  pkt_type, hcmd);
        return 0;
    }

    if (hlen > len) {
        dbgprintf("pko_file: pko_accept_pkt: hdr->len is too large!! "
               "(%d, can only receive %d)\n", hlen, len);
        return 0;
    }

    // get the actual packet data
    length = pko_recv_bytes(sock, buf + sizeof(pko_pkt_hdr), 
                            hlen - sizeof(pko_pkt_hdr));

    if (length < 0) {
        dbgprintf("pko_file: accept recv2 error!!\n");
        return 0;
    }

    if (length < (hlen - sizeof(pko_pkt_hdr))) {
        dbgprintf("pko_accept_pkt: Did not receive full packet!!! "
                  "Fix this! (%d)\n", length);
    }

    return 1;
}

//----------------------------------------------------------------------
//
int pko_open_file(char *path, int flags)
{
    pko_pkt_open_req *openreq;
    pko_pkt_file_rly *openrly;

    if (pko_fileio_sock < 0) {
        return -1;
    }

    dbgprintf("pko_file: file open req (%s, %x)\n", path, flags);

    openreq = (pko_pkt_open_req *)&send_packet[0];

    // Build packet
    openreq->cmd = htonl(PKO_OPEN_CMD);
    openreq->len = htons((unsigned short)sizeof(pko_pkt_open_req));
    openreq->flags = htonl(flags);
    strncpy(openreq->path, path, PKO_MAX_PATH);
    openreq->path[PKO_MAX_PATH - 1] = 0; // Make sure it's null-terminated

    if (pko_lwip_send(pko_fileio_sock, openreq, sizeof(pko_pkt_open_req), 0) < 0) {
        return -1;
    }

    if (!pko_accept_pkt(pko_fileio_sock, recv_packet, 
                        sizeof(pko_pkt_file_rly), PKO_OPEN_RLY)) {
        dbgprintf("pko_file: pko_open_file: did not receive OPEN_RLY\n");
        return -1;
    }

    openrly = (pko_pkt_file_rly *)recv_packet;
    
    dbgprintf("pko_file: file open reply received (ret %d)\n", ntohl(openrly->retval));

    return ntohl(openrly->retval);
}


//----------------------------------------------------------------------
//
int pko_close_file(int fd)
{
    pko_pkt_close_req *closereq;
    pko_pkt_file_rly *closerly;


    if (pko_fileio_sock < 0) {
        return -1;
    }

    dbgprintf("pko_file: file close req (fd: %d)\n", fd);

    closereq = (pko_pkt_close_req *)&send_packet[0];
    closerly = (pko_pkt_file_rly *)&recv_packet[0];

    closereq->cmd = htonl(PKO_CLOSE_CMD);
    closereq->len = htons((unsigned short)sizeof(pko_pkt_close_req));
    closereq->fd = htonl(fd);

    if (pko_lwip_send(pko_fileio_sock, closereq, sizeof(pko_pkt_close_req), 0) < 0) {
        return -1;
    }

    if(!pko_accept_pkt(pko_fileio_sock, (char *)closerly, 
                       sizeof(pko_pkt_file_rly), PKO_CLOSE_RLY)) {
        dbgprintf("pko_file: pko_close_file: did not receive PKO_CLOSE_RLY\n");
        return -1;
    }

    dbgprintf("pko_file: pko_close_file: close reply received (ret %d)\n", 
              ntohl(closerly->retval));

    return ntohl(closerly->retval);
}

//----------------------------------------------------------------------
//
int pko_lseek_file(int fd, unsigned int offset, int whence)
{
    pko_pkt_lseek_req *lseekreq;
    pko_pkt_file_rly *lseekrly;


    if (pko_fileio_sock < 0) {
        return -1;
    }

    dbgprintf("pko_file: file lseek req (fd: %d)\n", fd);

    lseekreq = (pko_pkt_lseek_req *)&send_packet[0];
    lseekrly = (pko_pkt_file_rly *)&recv_packet[0];

    lseekreq->cmd = htonl(PKO_LSEEK_CMD);
    lseekreq->len = htons((unsigned short)sizeof(pko_pkt_lseek_req));
    lseekreq->fd = htonl(fd);
    lseekreq->offset = htonl(offset);
    lseekreq->whence = htonl(whence);

    if(pko_lwip_send(pko_fileio_sock, lseekreq, sizeof(pko_pkt_lseek_req), 0) < 0) {
        return -1;
    }

    if(!pko_accept_pkt(pko_fileio_sock, (char *)lseekrly, 
                       sizeof(pko_pkt_file_rly), PKO_LSEEK_RLY)) {
        dbgprintf("pko_file: pko_lseek_file: did not receive PKO_LSEEK_RLY\n");
        return -1;
    }

    dbgprintf("pko_file: pko_lseek_file: lseek reply received (ret %d)\n", 
              ntohl(lseekrly->retval));

    return ntohl(lseekrly->retval);

}


//----------------------------------------------------------------------
//
int pko_write_file(int fd, char *buf, int length)
{
    pko_pkt_write_req *writecmd;
    pko_pkt_file_rly *writerly;
    int hlen;
    int writtenbytes;
    int nbytes;
    int retval;

    if (pko_fileio_sock < 0) {
        return -1;
    }

    dbgprintf("pko_file: file write req (fd: %d)\n", fd);

    writecmd = (pko_pkt_write_req *)&send_packet[0];
    writerly = (pko_pkt_file_rly *)&recv_packet[0];

    hlen = (unsigned short)sizeof(pko_pkt_write_req);
    writecmd->cmd = htonl(PKO_WRITE_CMD);
    writecmd->len = htons(hlen);
    writecmd->fd = htonl(fd);

    // Lwip/smap driver didnt like me sending a big chunk of data to 
    // transmit (probably need output buffer in driver), so I divide
    // the write request into smaller packets instead..
#if 0
    // Not fixed for htonl/s
    writecmd->nbytes = length;    

    send(pko_fileio_sock, writecmd, writecmd->len, 0);
    ret = send(pko_fileio_sock, buf, length, 0);
    if ((ret < 0) || (ret != length)) {
        dbgprintf("pko_file: write_file: lwip_send failed (should send %d, sent %d)\n", length, ret);
        return -1;
    }

    if(!pko_accept_pkt(pko_fileio_sock, (char *)writerly, 
                       sizeof(pko_pkt_file_rly), PKO_WRITE_RLY)) {
        dbgprintf("pko_file: pko_close_file: did not receive PKO_WRITE_RLY\n");
        return -1;
    }

    dbgprintf("pko_file: pko_write_file: write reply received (ret %d)\n", 
              writerly->retval);
    return writerly->retval;
#else
    // Divide the write request
    writtenbytes = 0;
    while (writtenbytes < length) {

        if ((length - writtenbytes) > PKO_MAX_WRITE_SEGMENT) {
            // Need to split in several read reqs
            nbytes = PKO_MAX_READ_SEGMENT;
        }
        else {
            nbytes = length - writtenbytes;
        }

        writecmd->nbytes = htonl(nbytes);
        // Copy data to the acutal packet
        memcpy(&send_packet[sizeof(pko_pkt_write_req)], &buf[writtenbytes],
               nbytes);

        // Sent the packet
        if (pko_lwip_send(pko_fileio_sock, writecmd, hlen + nbytes, 0) < 0) {
            return -1;
        }

        // Get reply
        if(!pko_accept_pkt(pko_fileio_sock, (char *)writerly, 
                           sizeof(pko_pkt_file_rly), PKO_WRITE_RLY)) {
            dbgprintf("pko_file: pko_write_file: "
                      "did not receive PKO_WRITE_RLY\n");
            return -1;
        }
        retval = ntohl(writerly->retval);

        dbgprintf("pko_file: wrote %d bytes (asked for %d)\n", 
                  retval, nbytes);

        if (retval < 0) {
            // Error
            dbgprintf("pko_file: pko_write_file: received error on write req (%d)\n",
                      retval);
            return retval;
        }

        writtenbytes += retval;
        if (retval < nbytes) {
            // EOF?
            break;
        }
    }
    return writtenbytes;
#endif
}

//----------------------------------------------------------------------
//
int pko_read_file(int fd, char *buf, int length)
{
    int readbytes;
    int nbytes;
    int i;
    pko_pkt_read_req *readcmd;
    pko_pkt_read_rly *readrly;


    if (pko_fileio_sock < 0) {
        return -1;
    }

    readcmd = (pko_pkt_read_req *)&send_packet[0];
    readrly = (pko_pkt_read_rly *)&recv_packet[0];
    readbytes = 0;

    readcmd->cmd = htonl(PKO_READ_CMD);
    readcmd->len = htons((unsigned short)sizeof(pko_pkt_read_req));
    readcmd->fd = htonl(fd);

    readbytes = 0;

    if (length < 0) {
        dbgprintf("pko_read_file: illegal req!! (whish to read < 0 bytes!)\n");
        return -1;
    }

    readcmd->nbytes = htonl(length);

    send(pko_fileio_sock, readcmd, sizeof(pko_pkt_read_req), 0);

    if(!pko_accept_pkt(pko_fileio_sock, (char *)readrly, 
                       sizeof(pko_pkt_read_rly), PKO_READ_RLY)) {
        dbgprintf("pko_file: pko_read_file: "
                  "did not receive PKO_READ_RLY\n");
        return -1;
    }

    nbytes = ntohl(readrly->nbytes);
    dbgprintf("pko_file: pko_read_file: Reply said there's %d bytes to read "
              "(wanted %d)\n", nbytes, length);

    // Now read the actual file data
    i = pko_recv_bytes(pko_fileio_sock, &buf[0], nbytes);
    if (i < 0) {
        dbgprintf("pko_file: pko_read_file, data read error\n");
        return -1;
    }
    return nbytes;
    return readbytes;
}



//----------------------------------------------------------------------
// Thread that waits for a PC to connect/disconnect/reconnect blah..
int
pko_file_serv(void *argv)
{
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int sock;
    int client_sock;
    int client_len;
    int ret;

    dbgprintf(" - PS2 Side application -\n");

    memset((void *)&server_addr, 0, sizeof(server_addr));
    // Should perhaps specify PC side ip..
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PKO_PORT);

    while ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        dbgprintf("pko_file: socket creation error (%d)\n", sock);
        return -1;
    }

    ret = bind(sock, (struct sockaddr *)&server_addr, 
                    sizeof(server_addr));
    if (ret < 0) {
        dbgprintf("pko_file: bind error (%d)\n", ret);
        disconnect(sock);
        return -1;
    }

    ret = listen(sock, 5);

    if (ret < 0) {
        dbgprintf("pko_file: listen error (%d)\n", ret);
        disconnect(sock);
        return -1;
    }

    // Active flag kinda sux, cause it wont be checked until a new client has
    // connected.. But it's better than nothing and good for now at least
    pko_fileio_active = 1;

    // Connection loop
    while(pko_fileio_active)
    {
        dbgprintf("Waiting for connection\n");

        client_len = sizeof(client_addr);
        client_sock = accept(sock, (struct sockaddr *)&client_addr, 
                             &client_len);
        if (client_sock < 0) {
            dbgprintf("pko_file: accept error (%d)", client_sock);
            continue;
        }

        dbgprintf("Client connected from %x\n", 
               client_addr.sin_addr.s_addr);

		remote_pc_addr = client_addr.sin_addr.s_addr;

        if (pko_fileio_sock > 0) {
            dbgprintf("Client reconnected\n");
            ret = disconnect(pko_fileio_sock);
            dbgprintf("close ret %d\n", ret);
        }
        pko_fileio_sock = client_sock;
    }

    if (pko_fileio_sock > 0) {
        disconnect(pko_fileio_sock);
    }

    disconnect(sock);

    ExitDeleteThread();
    return 0;
}
