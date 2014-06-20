/*
 * zmq_acc_frame.h
 *
 *  Created on: 22/05/2014
 *      Author: michael
 */

#ifndef ZMQ_ACC_FRAME_H_
#define ZMQ_ACC_FRAME_H_

#define BLACK "\x1b[22;30m"
#define RED "\x1b[22;31m"
#define GREEN "\x1b[22;32m"
#define BLUE "\x1b[22;34m"
#define RESET "\x1b[39m"
#define CLEAR "\x1b[2J"
#define LINE "\n===================================\n"
#define MOVE_CSR(n, m) "\x1b[" #n ";" #m "H"
#define ACK(str) GREEN str RESET
#define ERR(str) RED str RESET
#define INFO(str) BLUE str RESET
#define PRT_ERR(str) fprintf(stderr, ERR(str) "\n")
#define PRT_ACK(str) fprintf(stdout, ACK(str) "\n")
#define PRT_INFO(str) fprintf(stdout, INFO(str) "\n")
#define PRT_ERR_PARAM(str, mod, param) fprintf(stderr, ACK(str)mod, param)
#define PRT_I_D(str, num) fprintf(stdout, INFO(str) "\n", num)
#define PRT_I_S(str, s) fprintf(stdout, INFO(str) ":%s\n", s)
#define PRT_E_S(str, s) fprintf(stdout, ERR(str) ":%s", s)
#define PRT_INFO_B(str) PRT_INFO(" * " str)
#define LOG(l, i, t) if(l->log_enabled) tb_write_log(l->log_info, i, t)
#define LOG_ADD(l, i, s) if(l->log_enabled) tb_address(l, i, s)
#define LOG_E_NO(l, str, eno) tb_log_error_no(l->log_info, l->log_enabled, str, eno)
#define LOG_INFO(l, i) tb_log_info(l->log_info, l->log_enabled, i, LOG_INFO)
#define LOG_S_E_NO(s, str, eno) tb_log_session_info(s, str, LOG_ERR, eno)
#define LOG_S_INFO(s, str) tb_log_session_info(s, str, LOG_INFO, 0)

#define MS_ASSERT(b, m) if(!(b)){ fprintf(stdout, RED "Failed: %s\n" BLACK, m); return FALSE; }
#define MS_M_ASSERT(b) \
			if(!b()) { \
				fprintf(stdout, RED "Failed test:" #b "\n" BLACK); \
				num_fail++; \
			} \
			else{ \
				fprintf(stdout, GREEN "Passed test:" #b "\n" BLACK); \
				num_pass++; \
			}

#define MS_CLOSE() \
		fprintf(stdout, GREEN "Number Passed: %d\n" BLACK, num_pass); \
		fprintf(stderr, RED "Number Failed: %d\n" BLACK, num_fail)

#define MS_SETUP() int num_fail = 0; int num_pass = 0

#define TEST(N) gboolean N(); \
	\
		gboolean N() \
		{ \
			fprintf(stdout, #N "\n");

#endif /* ZMQ_ACC_FRAME_H_ */
