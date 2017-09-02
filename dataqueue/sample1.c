
#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"
#include "sample1.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

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
#define RAND_UPPER_LIMIT 9

/*
 * 指定した範囲の乱数を生成
 */
int rand_range(int min, int max)
{
	return min + (int)(rand()*(max-min+1.0)/(1.0+RAND_MAX));
}

#define HIST_MAX_NUM 100
static unsigned int hist[10];     /* ヒストグラム用データ */
static unsigned int hist_num;     /* 保存されているヒストグラムの数 */

/**
 *  * ヒストグラムにデータを保存
 *   */
int hist_add(unsigned int d)
{
    assert(d > 0 && d < 10);
    if (hist_num < HIST_MAX_NUM) {
        hist[d-1]++;
        hist_num++;
        return 0;
    } else 
        return -1;
}

/**
 *  * ヒストグラムのデータをクリア
 *   */
void hist_clear()
{
    memset(hist, 0, sizeof(hist));
    hist_num = 0;
}

/**
 *  * ヒストグラムを表示
 *   */
void hist_show()
{
    int i, j;
    
    for (i = 0; i < 9; i++) {
        printf("%d:", i+1);
        for (j = 0; j < hist[i]; j++)
            printf("*");
        printf("\n");
    }
}

/**
 *  メインタスク
 */
void task(intptr_t exinf)
{
	unsigned int count = 0;

	syslog(LOG_NOTICE, "task start.");

	srand((unsigned int)time(NULL));

	for (;;) {
		int r;

		/* データキューからデータを受信 */
		SVC_PERROR(rcv_dtq(DTQ_RAND, &r));
		hist_add(r);		/* データをヒストグラムに保存 */
		count++;
		if (count == HIST_MAX_NUM) {
			hist_show();	/* ヒストグラムを表示 */
			hist_clear();	/* ヒストグラムデータを削除 */
			syslog(LOG_NOTICE, "");
			count = 0;
		} 
	}
}

/**
 * 周期ハンドラ
 */
void rand_cyc_handler(intptr_t exinf)
{
	unsigned int i;

	/* ランダム値を取得 */
	i = rand_range(RAND_LOW_LIMIT, RAND_UPPER_LIMIT); 
	/* データをデータキューで送信 */
	SVC_PERROR(ipsnd_dtq(DTQ_RAND, i));
}


