#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#define BUF 1024

int main(int argc, char **argv)
{
    struct sockaddr_rc serv_addr = { 0 }, cli_addr = { 0 };
        // this is needed so that it can work properly with accpet!!!
    int s, client, bytes_read;
    socklen_t opt = sizeof(cli_addr);
    char buf[BUF];

    // allocate socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // bind socket to port 1 of the first available 
    // local bluetooth adapter
    serv_addr.rc_family = AF_BLUETOOTH;
    serv_addr.rc_bdaddr = *BDADDR_ANY;
    serv_addr.rc_channel = (uint8_t) 1;
    bind(s, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    // put socket into listening mode
    listen(s, 1);

    // accept one connection
    client = accept(s, (struct sockaddr *)&cli_addr, &opt);

    ba2str( &cli_addr.rc_bdaddr, buf );
    fprintf(stderr, "accepted connection from %s\n", buf);

    // enter a loop for sending and receiving messages
    while(1) {
        memset(buf, 0, sizeof(buf));
        bytes_read = read(client, buf, sizeof(buf));
        if( bytes_read <= 0 ) {
            perror("Failed to receive message");
            break;
        }
        printf("Received: %s\n", buf);

        // send a message
        if(write(client, buf, sizeof(buf)) < 0) {
            perror("Failed to send message");
            break;
        }
    }

    close(client);
    close(s);
    return 0;
}
