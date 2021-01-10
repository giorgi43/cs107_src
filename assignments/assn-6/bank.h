#ifndef _BANK_H
#define _BANK_H

#include <pthread.h>
#include <semaphore.h>
typedef struct Bank {
  unsigned int numberBranches;
  struct       Branch  *branches;
  struct       Report  *report;
  sem_t teller_transfer_lock;
  pthread_mutex_t report_transfer_lock; // for function Report_Transfer()
  // for function Report_DoReport()
  int workers_left; // number of workers left to finish job
  pthread_mutex_t lock; // for making operations on workers_left atomic
  sem_t next_day_lock;
} Bank;

#include "account.h"

int Bank_Balance(Bank *bank, AccountAmount *balance);

Bank *Bank_Init(int numBranches, int numAccounts, AccountAmount initAmount,
                AccountAmount reportingAmount,
                int numWorkers);

int Bank_Validate(Bank *bank);
int Bank_Compare(Bank *bank1, Bank *bank2);



#endif /* _BANK_H */
