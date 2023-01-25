
#include <string.h>
#include <stdlib.h>
#include "uart.h"
#include "chip_vm014.h"
#include "devdescr.h"
#include "chip.h"
#include "io.h"

int _getpid_r(struct _reent * a) {
	return 0;
}

int _kill_r(struct _reent * a, int b, int c) {
	return 0;
}

_PTR _calloc_r(struct _reent * a, size_t b, size_t c)
{
	return 0;
}


typedef struct Buffer {
	char str[100];
	int len;
} Buffer;

// BURK functions
// структура интегратора (угол)
typedef struct Integ {
  // выходной сигнал
  double s;
  // дельта - дискретность интегратора
  unsigned long h;
  // предыдущее значение
  double t_before;
} Integ;

// структура сумматора
typedef struct Sum {
  // выходной сигнал от сумматора
  double u;
} Sum;

// структура коэффициента усиления Gain
typedef struct Gain {
  // выходной сигнал от коэффициента усиления
  double u;
} Gain;

// структура насыщения Saturation
typedef struct Saturation {
  // выходной сигнал от насыщения
  double u;
} Saturation;

// структура дискретного интегратора
typedef struct Disc_Integ {
  // переменные используемые внутри дискретного интегратора
  double t_before;
  // выходной сигнал от дискретного интегратора
  double u;

} Disc_Integ;

// структура реле
typedef struct Relay {
  // сигнал от реле
  double u;

} Relay;

// структура Ф от тау
typedef struct F_ot_tau {
  // переменные для тау
  // сигнал сохранённый в тау
  double u;
  // Время ВКЛючения двигателя
  unsigned long t_on;
  // Время ВЫКЛючения двигателя
  unsigned long t_off;
  // Логическая переменная (Работает ли двигатель?) - ДА, НЕТ
  // По умолчанию - НЕТ - не работает
  //bool is_engine_work = false (0);
  int is_engine_work;
  // Счётчик, сколько раз запускался двигатель
  // По умолчанию 0, так как ни разу еще не запускался
  unsigned long cnt_start;

} F_ot_tau;

//default setting function
void Setting (Integ *pI, Disc_Integ *p_DI, Sum *pSum2, Sum *pSum3, Gain *pG, Saturation *pSat, Relay *pR, F_ot_tau *pF)
{
	pI-> s = 0;
	pI-> h = 0;
	pI-> t_before = 0;

	p_DI-> t_before = 0;
	p_DI-> u = 0;

	pSum2-> u = 0;

	pSum3-> u = 0;

	pG-> u = 0;

	pSat-> u = 0;

	pR-> u = 0;

	pF-> cnt_start = 0;
	pF-> is_engine_work = 0;
	pF-> t_off = 0;
	pF-> t_on= 0;
	pF-> u=0;
}
// Глобальные переменные
// переменная типа структуры интегратора (угла)
Integ integ;
// переменная типа структуры сумматора
Sum sum2;  // суммирует 2 сигнала
Sum sum3;  // суммирует 3 сигнала
// переменная типа структуры коэффициента усиления
Gain gain;
// переменная типа структуры насыщения
Saturation sat;
// переменная типа структуры дискретного интегратора
Disc_Integ disc_integrator;
// переменная типа структуры реле
Relay relay;
// переменная типа структуры Ф от тау
F_ot_tau ft;

// интегратор (угол)
void integratorIn(Integ *pI, double in_u, unsigned long t)
{
  pI->h = t - pI->t_before;
  pI->s += in_u * pI->h * 0.001;
  pI->t_before = t;
}

/* Дискретный интегратор */
/* Подаём на вход сигнал */
// in_u - значение сигнала на входе в интегратор
// t - текущее время
void disc_integratorIn(Disc_Integ *p_DI, double in_u, unsigned long t) {

  // Параметры дискретного интегратора
  // k - коэффициент
  double K = 1;
  // Шаг дискретизации
  unsigned long T = 10;

  // Если мы ждали больше шага дискретизации
  if (t - p_DI->t_before >= T) {
    // Обновляем значение интеграла
    p_DI->u += in_u * K * T * 0.001;  // мс
    // Обновляем сохранённое время
    p_DI->t_before = t;
  }
  // Если еще не прождали нужный шаг дискретизации
  // То ничего не делаем, и выводим предыдущий результат
  // выход
}

// Summator
void summatorIn2(Sum *pSum2, double u1, double u2) {
  // Значение сигнала, полученного Сумматором
  pSum2->u = u1 + u2;
}


void summatorIn3(Sum *pSum3, double u1, double u2, double u3) {
  // Значение сигнала, полученного Сумматором
  pSum3->u = u1 + u2 + u3;
}


// Gain
void gainIn(Gain *pG, double in_u) {
  double multiplier = 0.1;
  pG->u = in_u * multiplier;
}


// Saturation
void saturationIn(Saturation *pSat, double in_u) {
  // Верхняя граница сигнала
  double upper = 0.25;
  // Нижняя граница сигнала
  double lower = -0.25;

  if (in_u >= upper) {
    pSat->u = upper;
  } else if (in_u <= lower) {
    pSat->u = lower;
  } else {
    pSat->u = in_u;
  }
}

// Relay
void relayIn(Relay *pR, double in_u, double sp, double an) {

  // Порог срабатывания реле
  double positive = 0.125;
  double negative = -0.125;

  if (sp < 0.08 && an < 0.4) {
    positive = 0.03;
    negative = -0.03;
  }

  // Преобразуем сигнал либо к -1, либо 0, либо 1
  if (in_u >= positive) {
    pR->u = 1;
  } else if (in_u <= negative) {
    pR->u = -1;
  } else {
    // u принадлежит интервалу (negative, positive )
    pR->u = 0;
  }
}

// Regulator

void regulatorIn(unsigned long t, double speed)
{
  integratorIn (&integ, speed, t);
  summatorIn2(&sum2, -integ.s, -disc_integrator.u);
  gainIn(&gain, sum2.u);
  saturationIn(&sat, gain.u);
  summatorIn3(&sum3, -speed, sat.u, -relay.u);
  disc_integratorIn(&disc_integrator, sum3.u, t);
  relayIn(&relay, disc_integrator.u, speed, integ.s);
}

/* Ф от тау */
// F_ot_tau
//  tau - задержку на включение двигателя в [мс]
void f_ot_tauIn(F_ot_tau *pF, unsigned long t, double in_u) {
  // Параметр тау
  unsigned long tau = 30;

  // Проверяем работает ли двигатель
  // Если работает
  if (pF->is_engine_work) {
    // Находим время работы двигателя
    // Это текущее время минус время запуска двигателя
    unsigned long wake_time = t - pF->t_on;
    // Если  время работы двигателя >30мс
    if (wake_time > tau) {
      // Нужно проверить значение сигнала
      // Если значение сигнала не равно сигналу при включении двигателя
      if (in_u != pF->u) {
        // Нужно выключить двигатель
        pF->is_engine_work = 0;
        // Задаём время выключения двигателя - теущее
        pF->t_off = t;
        // сохранить значение сигнала, ставим именно 0
        // Потому что у нас ступенчатый вид сигнала
        // И должна быть его 0 фаза
        // Не может быть чтобы сигнал был,
        // например 0.5 и сразу стал -0.5
        pF->u = 0;
      }
    }
  } else {
    // Двигатель не работает
    // Проверяем запускался ли он хоть раз
    // Если еще не запускался
    if (pF->cnt_start == 0) {
      // То нам не нужно вычислять время спячки двигателя
      // И как только сигнал будет не нулевой
      // Включаем двигатель
      // если текущее значение сигнала НЕ нулевое
      if (in_u != 0) {
        // Включаем двигатель
        pF->is_engine_work = 1;
        // Увеличиваем счётчик запусков двигателей
        pF->cnt_start += 1;
        // сохраняем время запуска двигателя - текущее время
        pF->t_on = t;
        // сохранить значение сигнала двигателя при старте
        // - то есть текущее значенеи сигнала
        pF->u = in_u;
      }
    } else {
      // Двигатель УЖЕ запускался
      // Нужно проверить значение сигнала
      // Если сигнал НЕ нулевой
      if (in_u != 0) {
        // Включаем двигатель
        pF->is_engine_work = 1;
        // Увеличиваем счётчик запусков двигателей
        pF->cnt_start += 1;
        // сохраняем время запуска двигателя - текущее время
        pF->t_on = t;
        // сохранить значение сигнала двигателя при старте
        // - то есть текущее значение сигнала
        pF->u = in_u;
      }
    }
  }
}



int parse(unsigned long * t, double *speed, char data[100])
{
	char * sep = NULL;
	char t_str[100];
	char speed_str[100];

	sep = strchr(data, ';');

	if (sep != NULL)
	{
		strncpy(t_str, data, (sep - data) / sizeof(char));
		*t = atoi(t_str);
		strcpy(speed_str, (sep + 1));
		*speed = atof(speed_str);
		return 1;
	}

	return 0;
}

void SystemRun(unsigned long t, double speed) {  // Делаем прогон через систему
  // Передаём на вход регулятора значение времени, скорости и угла
  regulatorIn(t, speed);
  // В Ф(тау) передаём текущее время и выхлоп регулятора
  f_ot_tauIn(&ft, t, relay.u);
}
// Что получаем на выходе из системы
double SystemOut() {
  // Система заканчивается Ф от тау
  return ft.u;
}

void outPrintln() {
 // Serial.println(SystemOut());
	uart_send_number(1, SystemOut());
}

int main(void) {

	volatile UART_UNIT * uart1 = (UART_UNIT*) 0xBFF00800;
	uart1 -> IIRF = 0xff; //fifo
	uart1 -> IIRF = 0xff;

uart_baud_set(1, 26); // baud 57600 bit/s
uart1 -> LCR = 3; //  8 bit data, no parity, 1 stop bit // uart1->LCR = (1<<0)|(1<<1)

	unsigned long t = 0;
	double speed = 0;

	Buffer buf;
	char qt_data[100];
	// flag that we readed data
	int readed_data = 0;
	// char for read data
	char c = 0;

	Setting(&integ, &disc_integrator, &sum2, &sum3, &gain, &sat, &relay, &ft);

	uart_send_string(1, (unsigned char *)"OK");

	while (1) {
		if (uart_check_in(1)) {
			c = uart_read_byte(1);
			if (c == '\n') {
				// Cut the data from QT
				buf.str[buf.len] = '\0';
				// Copy data from buffer
				strcpy(qt_data, buf.str);
				// Clear buffer
				buf.str[0] = '\0';
				buf.len = 0;
				// up flag that we readed data
				readed_data = 1;

			} else {
				// data don't ended
				// add symbol to the string
				buf.str[buf.len] = c;
				buf.len++;
			}
		}

		if (readed_data) {
			// down flag for new read data
			readed_data = 0;
			// parse data
			if (parse(&t, &speed, qt_data))
			{
				// system process run
				SystemRun(t, speed);
				outPrintln();

			}else {

				//send old data
			     }
		}

	}
	return 0;
}

