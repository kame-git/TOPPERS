
#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"
#include "sample1.h"

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

/**
 * 周期ハンドラ
 */
void cyc_handler1(intptr_t exinf)
{
	/* フラグパターンの初期化 */
	FLGPTN flgptn = INITIAL_FLAG_PATTERN;

	/* フラグセット */
	SVC_PERROR(iset_flg(FLG_MATCH, flgptn | 0x01));
}

/**
 * 周期ハンドラ
 */
void cyc_handler2(intptr_t exinf)
{
	FLGPTN flgptn = INITIAL_FLAG_PATTERN;

	SVC_PERROR(iset_flg(FLG_MATCH, flgptn | 0x02));
}

/*
 *  メインタスク
 */
void task(intptr_t exinf)
{
	FLGPTN  flgptn;

	syslog(LOG_NOTICE, "task start.");

	for (;;) {
		/* イベントフラグの通知を待つ */
		wai_flg(FLG_MATCH, 0x03, TWF_ORW, &flgptn);
		//wai_flg(FLG_MATCH, 0x03, TWF_ANDW, &flgptn);
		syslog(LOG_NOTICE, "Flag state changed.");

		if (flgptn & 0x01)
			syslog(LOG_NOTICE, "Flag1 was set.");

		if (flgptn & 0x02)
			syslog(LOG_NOTICE, "Flag2 was set.");

		syslog(LOG_NOTICE, "");
	}
}
