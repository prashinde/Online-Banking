#include "server.h"

void transaction(c_sock *ns, int fd, unordered_map<unsigned long, cr_rec_t *> map)
{
	while(1) {
		c_trans_t t;
		ns->c_sock_read(fd, (void *)&t, sizeof(t));
		print_trans(&t, NULL);
		fflush(stdout);
	}
}

