#include <stdio.h>
#include <math.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define POINTS_COUNT 5
#define COMP_COUNT 3
#define K_PARAM 1
#define D_PARAM 25.0f
#define L_LIMIT 2.0f
#define EPS 1e-4f

#define ST_DECLINE (-1)
#define ST_NORM 0
#define ST_GROWTH 1

typedef struct {
    int time;
    int type;
    int count;
    float value;
} Incident;

int main(void) {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif

    float x[COMP_COUNT][POINTS_COUNT] = {
        { 41.9f, 47.8f, 48.3f, 48.8f, 49.3f },
        { 57.4f, 55.4f, 53.4f, 51.4f, 54.8f },
        { 30.2f, 29.5f, 34.1f, 33.4f, 32.6f }
    };

    float q[COMP_COUNT][POINTS_COUNT] = { { 0.0f } };
    int state[COMP_COUNT][POINTS_COUNT] = { { 0 } };

    Incident incidents[POINTS_COUNT];
    int incident_count = 0;

    int c, i;

    printf("Контроль облачной инфраструктуры (Вариант 7, КТ1)\n\n");

    printf("Исходные данные телеметрии:\n");
    printf("Отсчет\tSSD\tAPP\tLOG\n");
    for (i = 0; i < POINTS_COUNT; i++) {
        printf("%d\t%.1f\t%.1f\t%.1f\n", i + 1, x[0][i], x[1][i], x[2][i]);
    }
    printf("\n");

    for (c = 0; c < COMP_COUNT; c++) {
        for (i = K_PARAM; i < POINTS_COUNT; i++) {
            q[c][i] = (x[c][i] - x[c][i - K_PARAM]) / D_PARAM;
        }
    }

    printf("Нормированное изменение Q при k = %d, D = %.0f:\n", K_PARAM, D_PARAM);
    printf("Отсчет\tQ(SSD)\t\tQ(APP)\t\tQ(LOG)\n");
    for (i = K_PARAM; i < POINTS_COUNT; i++) {
        printf("%d\t%+.3f\t\t%+.3f\t\t%+.3f\n", i + 1, q[0][i], q[1][i], q[2][i]);
    }
    printf("\n");

    for (c = 0; c < COMP_COUNT; c++) {
        for (i = K_PARAM + 1; i < POINTS_COUNT; i++) {
            float diff = q[c][i] - q[c][i - 1];
            if (diff > EPS) {
                state[c][i] = ST_GROWTH;
            } else if (diff < -EPS) {
                state[c][i] = ST_DECLINE;
            } else {
                state[c][i] = ST_NORM;
            }
        }
    }

    printf("Состояние компонентов:\n");
    printf("Отсчет\tSSD\t\tAPP\t\tLOG\n");
    for (i = 0; i < POINTS_COUNT; i++) {
        printf("%d", i + 1);
        for (c = 0; c < COMP_COUNT; c++) {
            const char* s = state[c][i] == ST_GROWTH ? "РОСТ    "
                          : state[c][i] == ST_DECLINE ? "СНИЖЕНИЕ"
                          : "НОРМА   ";
            printf("\t%s", s);
        }
        printf("\n");
    }
    printf("\n");

    for (i = 0; i < POINTS_COUNT; i++) {
        int problem = 0;
        float val = 0.0f;
        for (c = 0; c < COMP_COUNT; c++) {
            if (state[c][i] != ST_NORM) {
                problem++;
                if (fabsf(q[c][i]) > fabsf(val)) {
                    val = q[c][i];
                }
            }
        }
        if (problem * 2 > COMP_COUNT) {
            incidents[incident_count].time = i + 1;
            incidents[incident_count].type = 1;
            incidents[incident_count].count = problem;
            incidents[incident_count].value = val;
            incident_count++;
        }
    }

    printf("Журнал инцидентов (большинство компонентов в проблемном состоянии):\n");
    if (incident_count == 0) {
        printf("Инцидентов не обнаружено.\n");
    } else {
        for (i = 0; i < incident_count; i++) {
            printf("Отсчет %d: инцидент (тип %d, проблемных компонентов: %d, значение Q: %+.3f)\n",
                incidents[i].time, incidents[i].type, incidents[i].count, incidents[i].value);
        }
    }

    return 0;
}
