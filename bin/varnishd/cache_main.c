/*
 * $Id$
 */

#include <stdio.h>
#include <assert.h>
#include <sys/time.h>

#include <event.h>

#include <cli.h>
#include <cli_priv.h>

#include "heritage.h"

static struct event ev_keepalive;

/*--------------------------------------------------------------------*/

static void
timer_keepalive(int a, short b, void *c)
{

	printf("%s(%d, %d, %p)\n", __func__, a, b, c);
	printf("Heeellloooo ?   Ohh bother...\n");
	exit (1);
}

static void
arm_keepalive(void)
{
	struct timeval tv;

	tv.tv_sec = 5;
	tv.tv_usec = 0;

	evtimer_del(&ev_keepalive);
	evtimer_add(&ev_keepalive, &tv);
}

/*--------------------------------------------------------------------*/

static void
cli_func_ping(struct cli *cli, char **av, void *priv __unused)
{
	time_t t;

	arm_keepalive();
	if (av[2] != NULL)
		cli_out(cli, "Got your %s\n", av[2]);
	time(&t);
	cli_out(cli, "PONG %ld\n", t);
}

/*--------------------------------------------------------------------*/

static struct cli_proto cli_proto[] = {
	{ CLI_PING },
	{ NULL }
};

void
child_main(void)
{
	struct event_base *eb;
	struct cli *cli;
	int i;

	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
	printf("Child starts\n");

	eb = event_init();
	assert(eb != NULL);

	cli = cli_setup(heritage.fds[0], heritage.fds[1], 0, cli_proto);

	evtimer_set(&ev_keepalive, timer_keepalive, NULL);
	arm_keepalive();

	i = event_dispatch();
	if (i != 0)
		printf("event_dispatch() = %d\n", i);

	printf("Child dies\n");
}

