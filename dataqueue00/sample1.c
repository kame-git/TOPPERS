
#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"
#include "sample1.h"

#include <string.h>

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
 *  タスク
 */
void task(intptr_t exinf)
{
	unsigned int i = 0;
	int r;
	char str[20];

	syslog(LOG_NOTICE, "task start.");

	for (;;) {
		/* データキューからデータを受信 */
		SVC_PERROR(rcv_dtq(DTQ1, &r));
		syslog(LOG_NOTICE, "receive data : %c", r);

		if (r != '\0') {
			str[i++] = r;
		} else {
			i = 0;
			syslog(LOG_NOTICE, "received string: %s", str);
		}
	}
}

/**
 * 周期ハンドラ
 */
void cyc_handler(intptr_t exinf)
{
	static unsigned int i = 0;
	char str[20] = "Hello World!";

	/* データをデータキューで送信 */
	SVC_PERROR(ipsnd_dtq(DTQ1, str[i]));
	i++;
	if (i > strlen(str))
		i = 0;
}


