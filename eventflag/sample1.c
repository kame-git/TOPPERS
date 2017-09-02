
#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"
#include "sample1.h"

#include <stdlib.h>
#include <time.h>

/*
 *  サービスコールのエラーのログ出力
 */
Inline void
svc_perror(const char *file, int_t line, const char *expr, ER ercd)
{
	if (ercd < 0) {
		t_perror(LOG_ERROR, file, line, expr, ercd);
	}
}

#define	SVC_PERROR(expr)	svc_perror(__FILE__, __LINE__, #expr, (expr))

#define RAND_LOW_LIMIT 1
#define RAND_UPPER_LIMIT 10

/*
 * 指定した範囲の乱数を生成
 */
int rand_range(int min, int max)
{
	return min + (int)(rand()*(max-min+1.0)/(1.0+RAND_MAX));
}

#define TASK_WAIT_MS 500

/*
 *  並行実行されるタスク
 */
void task1(intptr_t exinf)
{
	int bingo = (int)exinf;
	FLGPTN flgptn = INITIAL_FLAG_PATTERN;

	syslog(LOG_NOTICE, "task1 start.");

	for (;;) {
		int r;

		r = rand_range(RAND_LOW_LIMIT, RAND_UPPER_LIMIT);
		syslog(LOG_NOTICE, "task1: %2d", r);
		if (r == bingo) {
			syslog(LOG_NOTICE, "task1: match!");
			SVC_PERROR(set_flg(FLG_MATCH, flgptn | 0x01));
		}
		dly_tsk(TASK_WAIT_MS);
	}
}


void task2(intptr_t exinf)
{
	int bingo = (int)exinf;
	FLGPTN flgptn = INITIAL_FLAG_PATTERN;

	syslog(LOG_NOTICE, "task1 start.");

	for (;;) {
		int r;

		r = rand_range(RAND_LOW_LIMIT, RAND_UPPER_LIMIT);
		syslog(LOG_NOTICE, "task2: %2d", r);
		if (r == bingo) {
			syslog(LOG_NOTICE, "task2: match!");
			SVC_PERROR(set_flg(FLG_MATCH, flgptn | 0x02));
		}
		dly_tsk(TASK_WAIT_MS);
	}
}
void task3(intptr_t exinf)
{
	int bingo = (int)exinf;
	FLGPTN flgptn = INITIAL_FLAG_PATTERN;

	syslog(LOG_NOTICE, "task3 start.");

	for (;;) {
		int r;

		r = rand_range(RAND_LOW_LIMIT, RAND_UPPER_LIMIT);
		syslog(LOG_NOTICE, "task3: %2d", r);
		if (r == bingo) {
			syslog(LOG_NOTICE, "task3: match!");
			SVC_PERROR(set_flg(FLG_MATCH, flgptn | 0x04));
		}
		dly_tsk(TASK_WAIT_MS);
	}
}
/*
 *  メインタスク
 */
void main_task(intptr_t exinf)
{
	FLGPTN  flgptn;
	ER_UINT	ercd;

	syslog(LOG_NOTICE, "Sample program starts (exinf = %d).", (int_t) exinf);

	/*
	 *  シリアルポートの初期化
	 *
	 *  システムログタスクと同じシリアルポートを使う場合など，シリアル
	 *  ポートがオープン済みの場合にはここでE_OBJエラーになるが，支障は
	 *  ない．
	 */
	ercd = serial_opn_por(TASK_PORTID);
	if (ercd < 0 && MERCD(ercd) != E_OBJ) {
		syslog(LOG_ERROR, "%s (%d) reported by `serial_opn_por'.",
									itron_strerror(ercd), SERCD(ercd));
	}
	SVC_PERROR(serial_ctl_por(TASK_PORTID,
							(IOCTL_CRLF | IOCTL_FCSND | IOCTL_FCRCV)));

	/*
 	 *  メインループ
	 */
//	SVC_PERROR(serial_rea_dat(TASK_PORTID, &c, 1));

	srand((unsigned int)time(NULL));

	for (;;) {
		wai_flg(FLG_MATCH, 0x07, TWF_ANDW, &flgptn);
		//wai_flg(FLG_MATCH, 0x07, TWF_ORW, &flgptn);
		syslog(LOG_NOTICE, "main_task : BINGOooo!!!!.");
	}

	syslog(LOG_NOTICE, "Sample program ends.");
	SVC_PERROR(ext_ker());
	assert(0);
}
