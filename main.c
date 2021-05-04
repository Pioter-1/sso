#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

volatile long long G_BANK_ACCOUNT = 100000000;
pthread_mutex_t G_MUTEX_BA = PTHREAD_MUTEX_INITIALIZER;

void ms_sleep_rand(){

    unsigned ms = rand()%500;   //max sleep 2s
    struct timespec req;
    req.tv_sec  = (ms / 1000);
    req.tv_nsec = (ms % 1000 * 1000000);
    nanosleep(&req, NULL);
}


void* pay_in(void*)
{
    int no_of_pay_ins = 10;                 // 10 wplat po 100
    long long pay_in = 100;
    for (int i =0; i<no_of_pay_ins; ++i)
    {
        printf("Thread %ld pay_in no %d \n",pthread_self(), i);
        while(true)
        {
            int lock_res = pthread_mutex_trylock(&G_MUTEX_BA);
            if(lock_res == 0)
            {
                //got lock can proceed
                break;
            }
            else
            {
                printnf("Thread %ld cannot pay_in no %d going for a sleep\n");
                ms_sleep_rand();
            }
        }
        printf("Thread %ld bank account %lld \n", pthread_self(), G_BANK_ACCOUNT);
        //we can change value now
        long long local_bank_account = G_BANK_ACCOUNT;
        //Lets sleep a while
        ms_sleep_rand();
        //modify local value
        local_bank_acconut += pay_in;
        //change GLOBAL
        G_BANK_ACCOUNT = local_bank_account;
        pthread_mutex_unlock(&G_MUTEX_BA);
        printf("Thread %ld going to sleep before next operation \n", pthread_self());
        ms_sleep_rand();
    }
}



void* pay_out(void*)
{
    int no_of_payouts = 20;                    // 20 wyplat  po 50
    long long pay_out = 50;
    for (int i = 0; i < no_of_payouts; ++i)
    {
        printf("Thread %ld pay_out no %d \n", pthread_self(), i);
        while(true)
        {
            int lock_res = pthread_mutex_trylock(&G_MUTEX_BA);
            if(lock_res == 0)
            {
                //got lock can proceed
                break;
            }
            else
            {
                printf("Thread %ld cannot pay_out no %d going for a sleep\n");
                ms_sleep_rand();
            }
        }
        printf("Thread %ld bank account %lld \n", pthread_self(), G_BANK_ACCOUNT);
        //we can change value now
        long long local_bank_account = G_BANK_ACCOUNT;
        //lets sleep a while
        ms_sleep_rand();
        //modify local value
        local_bank_account -= pay_out;
        //change GLOBAL
        G_BANK_ACCOUNT = local_bank_account;
        pthread_mutex_unlock(&G_MUTEX_BA);
        printf("Thread %ld going to sleep before next operation \n",pthread_self());
        ms_sleep_rand();
    }
}

int main(int argc, char* argv[])
{
    pthread_t th_1,th_2,th_3,th_4;
    int rc_1,rc_2,rc_3,rc_4;

    printf("Operations starts BA %lld\n",G_BANK_ACCOUNT);

    pthread_create(&th_1, NULL, &pay_out, NULL );
    pthread_create(&th_2, NULL, &pay_in, NULL );
    pthread_create(&th_3, NULL, &pay_out, NULL );
    pthread_create(&th_4, NULL, &pay_in, NULL );
    pthread_join(th_1,NULL);
    pthread_join(th_2,NULL);
    pthread_join(th_3,NULL);
    pthread_join(th_4,NULL);

    printf("Operations ends BA %lld\n",G_BANK_ACCOUNT);

    return 0;
}
