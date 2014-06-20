/*
 * zmq_acceptance.c
 *
 *  Created on: 22/05/2014
 *      Author: michael
 */

#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <zmq.h>
#include <assert.h>

#include "../include/zmq_acc_frame.h"

int
build_up(void **ctx, void **serv_sock, void **cli_sock);

int
tear_down(void *ctx, void *serv_sock, void *cli_sock);

int
build_up(void **ctx, void **serv_sock, void **cli_sock)
{
	*ctx = zmq_ctx_new();
	MS_ASSERT(*ctx, "ctx == NULL");

	PRT_INFO_B("Creating rep socket");
	*serv_sock = zmq_socket(*ctx, ZMQ_REP);
	MS_ASSERT(*serv_sock, "socket == NULL");

	PRT_INFO_B("Creating req socket");
	*cli_sock = zmq_socket(*ctx, ZMQ_REQ);
	MS_ASSERT(*cli_sock, "req == NULL");

	return TRUE;
}

int
tear_down(void *ctx, void *serv_sock, void *cli_sock)
{
	PRT_INFO_B("Closing req socket");
	int rc = zmq_close(cli_sock);
	MS_ASSERT(rc == 0, "zmq_close not returning 0");

	PRT_INFO_B("Closing rep socket");
	rc = zmq_close(serv_sock);
	MS_ASSERT(rc == 0, "zmq_close not returning 0");

	PRT_INFO_B("Terminating context");
	rc = zmq_ctx_term(ctx);
	MS_ASSERT(rc == 0, "zmq_ctx_term not returning 0");

	return TRUE;
}

#include "../include/zmq_acc_frame.h"

TEST(set_heartbeat)

	int rc;
	void *ctx, *serv_sock, *cli_sock;
	MS_ASSERT(build_up(&ctx, &serv_sock, &cli_sock), "build_up failed");

	int heartbeat = 5000;

	struct t_option_t option;
	option.option_ = ZMQ_SCTP_HB_INTVL;
	option.optval_ = &heartbeat;
	option.transport = "sctp";
	PRT_INFO_B("Setting heartbeat value to 5000");
	rc = zmq_setsockopt(serv_sock, ZMQ_TRANSPORT_OPTION, &option, sizeof(int));
	MS_ASSERT(rc == 0, "zmq_setsockopt on serv_sock not returning 0 "
			"on set heartbeat");

	int r_heartbeat = 0;

	struct t_option_t r_option;
	r_option.option_ = ZMQ_SCTP_HB_INTVL;
	r_option.optval_ = &r_heartbeat;
	r_option.transport = "sctp";
	PRT_INFO_B("Getting heartbeat value");
	int len = sizeof(int);
	rc = zmq_getsockopt(serv_sock, ZMQ_TRANSPORT_OPTION, &r_option, &len);
	PRT_I_D("Returned value = %d", *((int*)r_option.optval_));
	MS_ASSERT(rc == 0, "zmq_getsockopt not returning 0");
	MS_ASSERT(heartbeat == r_heartbeat, "heartbeat not equal to 5000");
	PRT_INFO_B("Binding rep socket");
	rc = zmq_bind(serv_sock, "sctp://127.0.0.1:4950");
	MS_ASSERT(rc == 0, "zmq_bind not returning 0");

	PRT_INFO_B("Connecting req socket");
	rc = zmq_connect(cli_sock, "sctp://127.0.0.1:4950");
	MS_ASSERT(rc == 0, "zmq_connect not returning 0");

	MS_ASSERT(tear_down(ctx, serv_sock, cli_sock), "tear_down failed");

	return TRUE;
}

TEST(set_address)
	int rc;
	void *ctx, *serv_sock, *cli_sock;
	MS_ASSERT(build_up(&ctx, &serv_sock, &cli_sock), "build_up failed");

	struct t_option_t option;
	option.option_ = ZMQ_SCTP_ADD_IP;
	option.optval_ = "192.168.0.11:5556";
	option.transport = "sctp";

	PRT_INFO_B("Attempting to add ip address");
	rc = zmq_setsockopt(serv_sock, ZMQ_TRANSPORT_OPTION, &option, 17);
	MS_ASSERT(rc == 0, "zmq_setsockopt not returning 0 on add ip");

	PRT_INFO_B("Binding socket");
	rc = zmq_bind(serv_sock, "sctp://192.168.0.10:5556");
	MS_ASSERT(rc == 0, "zmq_bind not returning 0");
	MS_ASSERT(tear_down(ctx, serv_sock, cli_sock), "tear_down failed");

	return TRUE;
}

TEST(set_incorrect_address)
	int rc;
	void *ctx, *serv_sock, *cli_sock;
	MS_ASSERT(build_up(&ctx, &serv_sock, &cli_sock), "build_up failed");

	struct t_option_t option;
	option.option_ = ZMQ_SCTP_ADD_IP;
	option.optval_ = "192168:5556";
	option.transport = "sctp";

	PRT_INFO_B("Attempting to add ip address");
	rc = zmq_setsockopt(serv_sock, ZMQ_TRANSPORT_OPTION, &option, 17);
	MS_ASSERT(rc == -1, "zmq_setsockopt not returning -1 on add invalid ip");

	MS_ASSERT(tear_down(ctx, serv_sock, cli_sock), "tear_down failed");

	return TRUE;
}

TEST(set_incorrect_heartbeat_neg)
	int rc;
	void *ctx, *serv_sock, *cli_sock;
	MS_ASSERT(build_up(&ctx, &serv_sock, &cli_sock), "build_up failed");

	int neg_heartbeat = -5000;

	double inv_heartbeat = 5000.34;
	struct t_option_t option;
	option.option_ = ZMQ_SCTP_HB_INTVL;
	option.optval_ = &neg_heartbeat;
	option.transport = "sctp";

	PRT_INFO_B("Attempting to set negative value for heartbeat");
	rc = zmq_setsockopt(serv_sock, ZMQ_TRANSPORT_OPTION, &option, sizeof(int));
	MS_ASSERT(rc == -1, "zmq_setsockopt not returning -1 on negative value");
	option.optval_ = &inv_heartbeat;

	int r_value = 0;
	struct t_option_t r_option;
	r_option.option_ = ZMQ_SCTP_HB_INTVL;
	r_option.optval_ = &r_value;
	r_option.transport = "sctp";

	PRT_INFO_B("Check if default value == -1");
	int len = sizeof(int);
	rc = zmq_getsockopt(serv_sock, ZMQ_TRANSPORT_OPTION, &r_option, &len);
	PRT_I_D("Returned value = %d", *((int*)r_option.optval_));
	MS_ASSERT(rc == 0, "zmq_getsockopt not returning 0");
	MS_ASSERT(r_value == -1, "heartbeat default not equal to -1");

	PRT_INFO_B("Attempting to set invalid value for heartbeat");
	rc = zmq_setsockopt(serv_sock, ZMQ_TRANSPORT_OPTION, &option, sizeof(double));
	MS_ASSERT(rc == -1, "zmq_setsockopt not returning -1 on invalid value");

	PRT_INFO_B("Check if default value == -1");
	rc = zmq_getsockopt(serv_sock, ZMQ_TRANSPORT_OPTION, &r_option, &len);
	PRT_I_D("Returned value = %d", *((int*)r_option.optval_));
	MS_ASSERT(rc == 0, "zmq_getsockopt not returning 0");
	MS_ASSERT(r_value == -1, "heartbeat default not equal to -1");

	MS_ASSERT(tear_down(ctx, serv_sock, cli_sock), "tear_down failed");
	return TRUE;
}

TEST(set_incorrect_heartbeat_double)
	int rc;
	void *ctx, *serv_sock, *cli_sock;
	MS_ASSERT(build_up(&ctx, &serv_sock, &cli_sock), "build_up failed");

	double inv_heartbeat = 5000.34;
	struct t_option_t option;
	option.option_ = ZMQ_SCTP_HB_INTVL;
	option.optval_ = &inv_heartbeat;
	option.transport = "sctp";

	PRT_INFO_B("Attempting to set negative value for heartbeat");
	rc = zmq_setsockopt(serv_sock, ZMQ_TRANSPORT_OPTION, &option, sizeof(double));
	MS_ASSERT(rc == -1, "zmq_setsockopt not returning -1 on negative value");
	option.optval_ = &inv_heartbeat;

	int r_value = 0;
	struct t_option_t r_option;
	r_option.option_ = ZMQ_SCTP_HB_INTVL;
	r_option.optval_ = &r_value;
	r_option.transport = "sctp";

	PRT_INFO_B("Check if default value == -1");
	int len = sizeof(int);
	rc = zmq_getsockopt(serv_sock, ZMQ_TRANSPORT_OPTION, &r_option, &len);
	PRT_I_D("Returned value = %d", *((int*)r_option.optval_));
	MS_ASSERT(rc == 0, "zmq_getsockopt not returning 0");
	MS_ASSERT(r_value == -1, "heartbeat default not equal to -1");

	MS_ASSERT(tear_down(ctx, serv_sock, cli_sock), "tear_down failed");
	return TRUE;
}

TEST(set_incorrect_heartbeat_null)
	MS_ASSERT(FALSE, "known failure");
	int rc;
	void *ctx, *serv_sock, *cli_sock;
	MS_ASSERT(build_up(&ctx, &serv_sock, &cli_sock), "build_up failed");

	double inv_heartbeat = 5000.34;
	struct t_option_t option;
	option.option_ = ZMQ_SCTP_HB_INTVL;
	option.optval_ = &inv_heartbeat;
	option.transport = "sctp";

	PRT_INFO_B("Attempting to set negative value for heartbeat");
	rc = zmq_setsockopt(serv_sock, ZMQ_TRANSPORT_OPTION, &option, sizeof(inv_heartbeat));
	MS_ASSERT(rc == -1, "zmq_setsockopt not returning -1 on negative value");
	option.optval_ = &inv_heartbeat;

	int r_value = 0;
	struct t_option_t r_option;
	r_option.option_ = ZMQ_SCTP_HB_INTVL;
	r_option.optval_ = &r_value;
	r_option.transport = "sctp";

	PRT_INFO_B("Check if default value == -1");
	int len = sizeof(int);
	rc = zmq_getsockopt(serv_sock, ZMQ_TRANSPORT_OPTION, &r_option, &len);
	PRT_I_D("Returned value = %d", *((int*)r_option.optval_));
	MS_ASSERT(rc == 0, "zmq_getsockopt not returning 0");
	MS_ASSERT(r_value == -1, "heartbeat default not equal to -1");

	MS_ASSERT(tear_down(ctx, serv_sock, cli_sock), "tear_down failed");
	return TRUE;
}

TEST(set_incorrect_heartbeat_char)
	int rc;
	void *ctx, *serv_sock, *cli_sock;
	MS_ASSERT(build_up(&ctx, &serv_sock, &cli_sock), "build_up failed");

	char inv_char = 'b';
	struct t_option_t option;
	option.option_ = ZMQ_SCTP_HB_INTVL;
	option.optval_ = &inv_char;
	option.transport = "sctp";

	PRT_INFO_B("Attempting to set negative value for heartbeat");
	rc = zmq_setsockopt(serv_sock, ZMQ_TRANSPORT_OPTION, &option, sizeof(inv_char));
	MS_ASSERT(rc == -1, "zmq_setsockopt not returning -1 on negative value");
	option.optval_ = &inv_char;

	int r_value = 0;
	struct t_option_t r_option;
	r_option.option_ = ZMQ_SCTP_HB_INTVL;
	r_option.optval_ = &r_value;
	r_option.transport = "sctp";

	PRT_INFO_B("Check if default value == -1");
	int len = sizeof(int);
	rc = zmq_getsockopt(serv_sock, ZMQ_TRANSPORT_OPTION, &r_option, &len);
	PRT_I_D("Returned value = %d", *((int*)r_option.optval_));
	MS_ASSERT(rc == 0, "zmq_getsockopt not returning 0");
	MS_ASSERT(r_value == -1, "heartbeat default not equal to -1");

	MS_ASSERT(tear_down(ctx, serv_sock, cli_sock), "tear_down failed");
	return TRUE;
}

TEST(set_rto)
int rc;
	void *ctx, *serv_sock, *cli_sock;
	MS_ASSERT(build_up(&ctx, &serv_sock, &cli_sock), "build_up failed");

	int rto = 2000;

	struct t_option_t option;
	option.option_ = ZMQ_SCTP_RTO;
	option.optval_ = &rto;
	option.transport = "sctp";
	PRT_INFO_B("Setting rto value to 2000");
	rc = zmq_setsockopt(serv_sock, ZMQ_TRANSPORT_OPTION, &option, sizeof(int));
	MS_ASSERT(rc == 0, "zmq_setsockopt on serv_sock not returning 0 "
			"on set rto");

	int r_rto = 0;

	struct t_option_t r_option;
	r_option.option_ = ZMQ_SCTP_RTO;
	r_option.optval_ = &r_rto;
	r_option.transport = "sctp";
	PRT_INFO_B("Getting rto value");
	int len = sizeof(int);
	rc = zmq_getsockopt(serv_sock, ZMQ_TRANSPORT_OPTION, &r_option, &len);
	PRT_I_D("Returned value = %d", *((int*)r_option.optval_));
	MS_ASSERT(rc == 0, "zmq_getsockopt not returning 0");
	MS_ASSERT(rto == r_rto, "rto not equal to 2000");
	PRT_INFO_B("Binding rep socket");
	rc = zmq_bind(serv_sock, "sctp://127.0.0.1:4950");
	MS_ASSERT(rc == 0, "zmq_bind not returning 0");

	PRT_INFO_B("Connecting req socket");
	rc = zmq_connect(cli_sock, "sctp://127.0.0.1:4950");
	MS_ASSERT(rc == 0, "zmq_connect not returning 0");

	MS_ASSERT(tear_down(ctx, serv_sock, cli_sock), "tear_down failed");

	return TRUE;
}

TEST(set_incorrect_rto_neg)
int rc;
	void *ctx, *serv_sock, *cli_sock;
	MS_ASSERT(build_up(&ctx, &serv_sock, &cli_sock), "build_up failed");

	int neg_rto = -2000;

	struct t_option_t option;
	option.option_ = ZMQ_SCTP_RTO;
	option.optval_ = &neg_rto;
	option.transport = "sctp";

	PRT_INFO_B("Attempting to set negative value for heartbeat");
	rc = zmq_setsockopt(serv_sock, ZMQ_TRANSPORT_OPTION, &option, sizeof(int));
	MS_ASSERT(rc == -1, "zmq_setsockopt not returning -1 on negative value");

	int r_value = 0;
	struct t_option_t r_option;
	r_option.option_ = ZMQ_SCTP_RTO;
	r_option.optval_ = &r_value;
	r_option.transport = "sctp";

	PRT_INFO_B("Check if set to default value == -1");
	int len = sizeof(int);
	rc = zmq_getsockopt(serv_sock, ZMQ_TRANSPORT_OPTION, &r_option, &len);
	PRT_I_D("Returned value = %d", *((int*)r_option.optval_));
	MS_ASSERT(rc == 0, "zmq_getsockopt not returning 0");
	MS_ASSERT(r_value == -1, "rto default not equal to -1");

	MS_ASSERT(tear_down(ctx, serv_sock, cli_sock), "tear_down failed");
	return TRUE;
}

TEST(set_incorrect_rto_double)
	void *ctx, *serv_sock, *cli_sock;
	MS_ASSERT(build_up(&ctx, &serv_sock, &cli_sock), "build_up failed");
	int rc;
	double inv_rto = 2000.34;

	struct t_option_t option;
	option.option_ = ZMQ_SCTP_RTO;
	option.optval_ = &inv_rto;
	option.transport = "sctp";

	PRT_INFO_B("Attempting to set negative value for heartbeat");
	rc = zmq_setsockopt(serv_sock, ZMQ_TRANSPORT_OPTION, &option, sizeof(double));
	MS_ASSERT(rc == -1, "zmq_setsockopt not returning -1 on negative value");
	option.optval_ = &inv_rto;

	int r_value = 0;
	struct t_option_t r_option;
	r_option.option_ = ZMQ_SCTP_RTO;
	r_option.optval_ = &r_value;
	r_option.transport = "sctp";

	PRT_INFO_B("Check if set to default value == -1");
	int len = sizeof(int);
	rc = zmq_getsockopt(serv_sock, ZMQ_TRANSPORT_OPTION, &r_option, &len);
	PRT_I_D("Returned value = %d", *((int*)r_option.optval_));
	MS_ASSERT(rc == 0, "zmq_getsockopt not returning 0");
	MS_ASSERT(r_value == -1, "rto default not equal to -1");

	MS_ASSERT(tear_down(ctx, serv_sock, cli_sock), "tear_down failed");
	return TRUE;
}

TEST(set_incorrect_rto_null)
	MS_ASSERT(FALSE, "known failure");
	void *ctx, *serv_sock, *cli_sock;
	MS_ASSERT(build_up(&ctx, &serv_sock, &cli_sock), "build_up failed");
	int rc, bla = 4000;

	struct t_option_t option;
	option.option_ = ZMQ_SCTP_RTO;
	option.transport = "sctp";

	MS_ASSERT(!(option.optval_), "optval not null");

	PRT_INFO_B("Attempting to set null value for rto");
	rc = zmq_setsockopt(serv_sock, ZMQ_TRANSPORT_OPTION, &option, sizeof(double));
	MS_ASSERT(rc == -1, "zmq_setsockopt not returning -1 on negative value");

	int r_value = 0;
	struct t_option_t r_option;
	r_option.option_ = ZMQ_SCTP_RTO;
	r_option.optval_ = &r_value;
	r_option.transport = "sctp";

	PRT_INFO_B("Check if set to default value == -1");
	int len = sizeof(int);
	rc = zmq_getsockopt(serv_sock, ZMQ_TRANSPORT_OPTION, &r_option, &len);
	PRT_I_D("Returned value = %d", *((int*)r_option.optval_));
	MS_ASSERT(rc == 0, "zmq_getsockopt not returning 0");
	MS_ASSERT(r_value == -1, "rto default not equal to -1");

	MS_ASSERT(tear_down(ctx, serv_sock, cli_sock), "tear_down failed");
	return TRUE;
}

TEST(set_incorrect_rto_char)
	void *ctx, *serv_sock, *cli_sock;

	MS_ASSERT(build_up(&ctx, &serv_sock, &cli_sock), "build_up failed");
	int rc;

	char val = 'b';
	struct t_option_t option;
	option.option_ = ZMQ_SCTP_RTO;
	option.optval_ = &val;
	option.transport = "sctp";

	PRT_INFO_B("Attempting to set negative value for heartbeat");
	rc = zmq_setsockopt(serv_sock, ZMQ_TRANSPORT_OPTION, &option, sizeof(char));
	MS_ASSERT(rc == -1, "zmq_setsockopt not returning -1 on negative value");

	int r_value = 0;
	struct t_option_t r_option;
	r_option.option_ = ZMQ_SCTP_RTO;
	r_option.optval_ = &r_value;
	r_option.transport = "sctp";

	PRT_INFO_B("Check if set to default value == -1");
	int len = sizeof(int);
	rc = zmq_getsockopt(serv_sock, ZMQ_TRANSPORT_OPTION, &r_option, &len);
	PRT_I_D("Returned value = %d", *((int*)r_option.optval_));
	MS_ASSERT(rc == 0, "zmq_getsockopt not returning 0");
	MS_ASSERT(r_value == -1, "rto default not equal to -1");

	MS_ASSERT(tear_down(ctx, serv_sock, cli_sock), "tear_down failed");
	return TRUE;
}

TEST(test_failover)
	MS_ASSERT(FALSE == TRUE, "Not testing");
	return TRUE;
}

int
main(void)
{
	MS_SETUP();

	MS_M_ASSERT(set_heartbeat);
	MS_M_ASSERT(set_address);
	MS_M_ASSERT(set_incorrect_address);
	MS_M_ASSERT(set_incorrect_heartbeat_neg);
	MS_M_ASSERT(set_incorrect_heartbeat_null);
	MS_M_ASSERT(set_incorrect_heartbeat_double);
	MS_M_ASSERT(set_incorrect_heartbeat_char);
	MS_M_ASSERT(set_rto);
	MS_M_ASSERT(set_incorrect_rto_neg);
	MS_M_ASSERT(set_incorrect_rto_null);
	MS_M_ASSERT(set_incorrect_rto_double);
	MS_M_ASSERT(set_incorrect_rto_char);
	MS_M_ASSERT(test_failover);
	MS_CLOSE();

	return 0;
}

