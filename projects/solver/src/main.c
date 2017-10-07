#include <string.h>
#include "simplex.h"
#include "event.h"
#include "utils.h"
#include <time.h>

extern int omp_get_thread_num();
extern int omp_get_num_threads();
extern int omp_get_num_procs();

#define UPDATE_COUNT 10000


int main(int argc, const char** argv)
{

    // Adatok konvertálása.
    if (argc > 1 && !strcmp(argv[1], "-c"))
    {
        char data_file_name[] = "data/data_labdarugas.txt";

        // Sorok számának a meghatározása.
        uint32_t rows_count = get_file_rows_count(data_file_name);
        const uint32_t events_count = rows_count;

        // Memória lefoglalása az események számára.
        event_t events[events_count];

        // Adatok beolvasása.
        const uint32_t buffer_size = 255;
        char buffer[buffer_size];
        FILE* fp = fopen(data_file_name, "r");
        uint32_t event_counter = 0;
        while(fgets(buffer, buffer_size, (FILE*) fp))
        {
            uint32_t min;
            char day[10];
            char time[10];
            char title[200];

            strcpy(events[event_counter].no, strtok (buffer, "\t"));
            min = atoi(strtok (NULL, "\t"));
            min+=0;
            strcpy(title, strtok (NULL, "\t"));
            events[event_counter].odds[0] = strtod(strtok (NULL, "\t"), NULL);
            events[event_counter].odds[1] = strtod(strtok (NULL, "\t"), NULL);
            events[event_counter].odds[2] = strtod(strtok (NULL, "\t"), NULL);
            strcpy(day, strtok (NULL, "\t"));
            strcpy(time, strtok (NULL, "\t"));
            events[event_counter].hash = odds_to_hash(events[event_counter].odds);

            //event_print(&events[event_counter]);

            event_counter++;
        }

        fclose(fp);


        // Adatok rendezése.
        for (uint32_t i = 0; i < events_count; ++i)
        {
            for (uint32_t j = i + 1; j < events_count; ++j)
            {
                if (events[i].hash > events[j].hash)
                {
                    event_t a = events[i];
                    events[i] = events[j];
                    events[j] = a;
                }
            }
        }

        // Kiírás.
        uint32_t counter = 0;
        uint32_t count = 0;
        for (uint32_t i = 0; i < events_count; i++)
        {
            if (0 == events[i].odds[0] || 0 == events[i].odds[1] || 0 == events[i].odds[2])
            {
                continue;
            }

            if (0 == i || (0 < i && ((events[i-1].hash != events[i].hash) || (events[i-1].hash == events[i].hash && counter < 2))))
            {
                //event_print(&events[i]);
                printf("%u\n", events[i].hash);

                /*double d[3];
                hash_to_odds(events[i].hash, &d);
                printf("%s %.2lf %.2lf %.2lf %u\n", events[i].no, d[0], d[1], d[2], events[i].hash);*/

                count++;
                counter++;
                continue;
            }

            counter = 0;
        }
    }

    // Számolás.
    if (argc > 2 && !strcmp(argv[1], "-s"))
    {
        // Szorzókat tartalmazó fájl megnyitása.
        char data_file_name[] = "data/d";

        // Sorok számának a meghatározása.
        uint32_t rows_count = get_file_rows_count(data_file_name);
        const uint32_t odds_count = rows_count;

        // Memória lefoglalása az események számára.
        odd_t odds[odds_count];

        // Adatok beolvasása.
        const uint32_t buffer_size = 255;
        char buffer[buffer_size];
        FILE* fp = fopen(data_file_name, "r");
        uint32_t odd_counter = 0;
        while(fgets(buffer, buffer_size, (FILE*) fp))
        {
            odds[odd_counter].hash = atoi(buffer);
            hash_to_odds(odds[odd_counter].hash, &odds[odd_counter].odds);

            //odd_print(&odds[odd_counter]);

            odd_counter++;
        }

        fclose(fp);

        // Számolandó index beolvasása.
        uint32_t index = atoi(argv[2]);
        //index = min(1, odds_count);

        // Iterációk számának a meghatározása.
        //uint32_t total = nCr(odds_count - index - 1, 3);
        //printf("total: %i\n", total);

        // Mehet a munka.
        simplex_init();
        time_t start = time(NULL);
        time_t now = time(NULL);
        uint32_t total;
        uint32_t left;
        uint32_t found = 0;

        int threads = omp_get_num_procs();
        //printf("%d\n", threads);

        for (index = 0; index < odds_count - 3; index++)
        {
            total = nCr(odds_count - index - 1, 3);
            left = total;

            //printf("total: %i\n", total);
            #pragma omp parallel for num_threads(threads)
            for (uint32_t i = index+1; i < odds_count - 2; i++)
            {
                for (uint32_t j = i+1; j < odds_count - 1; j++)
                {
                    // Elvileg ezt kell majd párhuzamosítani.
                    for (uint32_t k = j+1; k < odds_count; k++)
                    {


                        tableau_t t;
                        init_tableau(&t, 4, 3);
                        reset_tableau(&t, &odds[index], &odds[i], &odds[j], &odds[k]);
                        simplex_solve(&t);

                        if (t.solution == OPTIMAL)
                        {
                            //simplex_print_solution(&t, false);
                            found++;
                            /*printf("%u %u %u %u\n", index, i, j, k);
                            printf("%.2lf %.2lf %.2lf | ", odds[index].odds[0], odds[index].odds[1], odds[index].odds[2]);
                            printf("%.2lf %.2lf %.2lf | ", odds[i].odds[0], odds[i].odds[1], odds[i].odds[2]);
                            printf("%.2lf %.2lf %.2lf | ", odds[j].odds[0], odds[j].odds[1], odds[j].odds[2]);
                            printf("%.2lf %.2lf %.2lf\n",  odds[k].odds[0], odds[k].odds[1], odds[k].odds[2]);*/
                        }


                        left--;
                        if (0 == left % UPDATE_COUNT)
                        {
                            char command[255];
                            //sprintf(command, "echo '\n\nspeed: %.0f\nleft: %i\nfound: %i' && cat /sys/class/thermal/thermal_zone1/temp && echo '' > oledfifo", ((double)(total-left)/(double)(time(NULL)-start)), left, found);
                            //sprintf(command, "/bin/bash -c \"cat <(echo '\n\nspeed: %.0f\nleft:  %i\nfound: %i' & cat /sys/class/thermal/thermal_zone1/temp & echo '\n') > oledfifo\"", ((double)(total-left)/(double)(time(NULL)-start)), left, found);

                            //sprintf(command, "printf '\nspeed: %.0f\nleft:  %i\nfound: %i\ntemp:  ' > info", ((double)(total-left)/(double)(time(NULL)-start)), left, found);
                            now = time(NULL);
                            sprintf(command, "printf '\nspeed: %.0f\nleft:  %i\nfound: %i\ntemp:  ' > info", ((double)(UPDATE_COUNT)/(double)(now-start)), left, found);
                            system(command);
                            system("cat /sys/class/thermal/thermal_zone1/temp >> info");
                            system("echo '' >> info");
                            system("cat info > oledfifo");
                            //printf("%s\n", command);

                            start = now;
                            //system("cat /sys/class/thermal/thermal_zone1/temp > oledfifo");
                            //system("echo '' > oledfifo");
                            //printf("speed: %.0f | left: %i | found: %i\n", ((double)(total-left)/(double)(time(NULL)-start)), left, found);
                        }
                        //printf("thread = %d\n", omp_get_thread_num());
                        //printf("tid:%d %u %u %u %u\n", omp_get_thread_num(), index, i, j, k);
                        //printf("%.2lf %.2lf %.2lf | ", odds[index].odds[0], odds[index].odds[1], odds[index].odds[2]);
                        //printf("%.2lf %.2lf %.2lf | ", odds[i].odds[0], odds[i].odds[1], odds[i].odds[2]);
                        //printf("%.2lf %.2lf %.2lf | ", odds[j].odds[0], odds[j].odds[1], odds[j].odds[2]);
                        //printf("%.2lf %.2lf %.2lf\n",  odds[k].odds[0], odds[k].odds[1], odds[k].odds[2]);*/
                    }
                }
            }
        }

    }

//    FILE *f;
    /*if ((f = fopen("data/d","wb")) == NULL)
    {
        printf("Error! opening file");
        exit(1);
    }

    for (uint32_t i = 0; i < 10; i++)
    {
        fwrite(&i, 4, 1, f);
    }

    fclose(f);*/

/*
    if ((f = fopen("data/d","rb")) == NULL)
    {
        printf("Error! opening file");
        exit(1);
    }
    uint32_t n, search = 30;
    bool found = false;
    while(1 == fread(&n,4,1,f) && (found=n != search));

    fclose(f);

    printf("%d %d\n",n, found);
*/

    /*
        int nthreads, tid;
        #pragma omp parallel private(nthreads, tid)
        {
            tid = omp_get_thread_num();
            printf("thread = %d\n", tid);
            if (tid == 0)
            {
                nthreads = omp_get_num_threads();
                printf("threads = %d\n", nthreads);
            }
        }
    */

    return EXIT_SUCCESS;

}
