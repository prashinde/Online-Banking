#include "client.h"
/*class c_sock {
	struct sockaddr_in server_addr;
	int sock;
public:
	int c_sock_addr(char *ip, int port);
	int c_sock_connect();
	int c_sock_read(char *buffer, size_t len);
	int c_sock_write(char *buffer, size_t len);
	void c_sock_close();
};*/

int c_sock :: c_sock_addr(char *ip, int port)
{
	int rc;

	this->sock = socket(AF_INET, SOCK_STREAM, 0);
	if(this->sock == -1) {
		cr_log << "Unable to create a socket.:" << errno << endl;
		return errno;
	}

	this->server_addr.sin_family = AF_INET;
	
	rc = inet_aton(ip, &(this->server_addr.sin_addr));
	if(rc == 0) {
		cr_log << "Server ip address invalid: " << ip << endl;
		return -EINVAL;
	}

	this->server_addr.sin_port = htons(port);
	return 0;
}

int c_sock::c_sock_connect()
{
	int rc;

	rc = connect(this->sock, (struct sockaddr *)&(this->server_addr), sizeof(this->server_addr));
	if (rc < 0) {
		cr_log << "Server not connected." << endl;
		return rc;
	}

	return 0;
}

ssize_t c_sock::c_sock_read(void *buffer, size_t len)
{
	return recv(this->sock, buffer, len, 0);
}

ssize_t c_sock::c_sock_write(void *buffer, size_t len)
{
	return send(this->sock, buffer, len, 0);
}

void c_sock::c_sock_close()
{
	close(this->sock);
}
