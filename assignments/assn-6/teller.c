#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <pthread.h>
#include "teller.h"
#include "account.h"
#include "error.h"
#include "debug.h"
#include "branch.h"

/*
 * deposit money into an account
 */
int
Teller_DoDeposit(Bank *bank, AccountNumber accountNum, AccountAmount amount)
{
  sem_wait(&bank->teller_transfer_lock);
  assert(amount >= 0);

  if (amount == 0) {
   sem_post(&bank->teller_transfer_lock);
   return ERROR_SUCCESS;
  }

  DPRINTF('t', ("Teller_DoDeposit(account 0x%"PRIx64" amount %"PRId64")\n",
                accountNum, amount));

  Account *account = Account_LookupByNumber(bank, accountNum);

  if (account == NULL) {
    sem_post(&bank->teller_transfer_lock);
    return ERROR_ACCOUNT_NOT_FOUND;
  }
  sem_wait(&account->lock); // lock this account
  pthread_mutex_lock(&(bank->branches[(BranchID) (accountNum >> 32)].lock)); // lock this branch

  Account_Adjust(bank,account, amount, 1);

  sem_post(&account->lock); // unlock this account
  pthread_mutex_unlock(&(bank->branches[(BranchID) (accountNum >> 32)].lock)); // unlock this branch
  sem_post(&bank->teller_transfer_lock);
  return ERROR_SUCCESS;
}

/*
 * withdraw money from an account
 */
int
Teller_DoWithdraw(Bank *bank, AccountNumber accountNum, AccountAmount amount)
{
  sem_wait(&bank->teller_transfer_lock);
  assert(amount >= 0);

  if (amount == 0) {
   sem_post(&bank->teller_transfer_lock);
   return ERROR_SUCCESS;
  }

  DPRINTF('t', ("Teller_DoWithdraw(account 0x%"PRIx64" amount %"PRId64")\n",
                accountNum, amount));

  Account *account = Account_LookupByNumber(bank, accountNum);

  if (account == NULL) {
    sem_post(&bank->teller_transfer_lock);
    return ERROR_ACCOUNT_NOT_FOUND;
  }
  sem_wait(&account->lock); // lock this account
  pthread_mutex_lock(&(bank->branches[(BranchID) (accountNum >> 32)].lock)); // lock this branch
  if (amount > Account_Balance(account)) {
    sem_post(&account->lock); // unlock this account
    pthread_mutex_unlock(&(bank->branches[(BranchID) (accountNum >> 32)].lock)); // unlock this branch
    sem_post(&bank->teller_transfer_lock);
    return ERROR_INSUFFICIENT_FUNDS;
  }
  Account_Adjust(bank,account, -amount, 1);
  sem_post(&account->lock); // unlock this account
  pthread_mutex_unlock(&(bank->branches[(BranchID) (accountNum >> 32)].lock)); // unlock this branch
  sem_post(&bank->teller_transfer_lock);
  return ERROR_SUCCESS;
}

/*
 * do a tranfer from one account to another account
 */
int
Teller_DoTransfer(Bank *bank, AccountNumber srcAccountNum,
                  AccountNumber dstAccountNum,
                  AccountAmount amount)
{
  sem_wait(&bank->teller_transfer_lock);
  assert(amount >= 0);
  DPRINTF('t', ("Teller_DoTransfer(src 0x%"PRIx64", dst 0x%"PRIx64
                ", amount %"PRId64")\n",
                srcAccountNum, dstAccountNum, amount));

  Account *srcAccount = Account_LookupByNumber(bank, srcAccountNum);
  if (srcAccount == NULL) {
    sem_post(&bank->teller_transfer_lock);
    return ERROR_ACCOUNT_NOT_FOUND;
  }

  Account *dstAccount = Account_LookupByNumber(bank, dstAccountNum);
  if (dstAccount == NULL) {
    sem_post(&bank->teller_transfer_lock);
    return ERROR_ACCOUNT_NOT_FOUND;
  }

  if (amount > Account_Balance(srcAccount)) {
    sem_post(&bank->teller_transfer_lock);
    return ERROR_INSUFFICIENT_FUNDS;
  }
  if (dstAccountNum == srcAccountNum) {
    sem_post(&bank->teller_transfer_lock);
    return ERROR_SUCCESS;
  }

  if (amount == 0) {
    sem_post(&bank->teller_transfer_lock);
    return ERROR_SUCCESS;
  }

  /*
   * If we are doing a transfer within the branch, we tell the Account module to
   * not bother updating the branch balance since the net change for the
   * branch is 0.
   */
  int updateBranch = !Account_IsSameBranch(srcAccountNum, dstAccountNum);
  BranchID srcB_ID = (BranchID) (srcAccountNum >> 32);
  BranchID destB_ID = (BranchID) (dstAccountNum >> 32);
    // src - amount
  sem_wait(&srcAccount->lock);
  pthread_mutex_lock(&(bank->branches[srcB_ID].lock));

  Account_Adjust(bank, srcAccount, -amount, 1);

  sem_post(&srcAccount->lock);
  pthread_mutex_unlock(&(bank->branches[srcB_ID].lock));
  // dst + amount
  sem_wait(&dstAccount->lock);
  pthread_mutex_lock(&(bank->branches[destB_ID].lock));

  Account_Adjust(bank, dstAccount, amount, 1);

  sem_post(&dstAccount->lock);
  pthread_mutex_unlock(&(bank->branches[destB_ID].lock));

  sem_post(&bank->teller_transfer_lock);
  return ERROR_SUCCESS;
  // src - amount
  /*sem_wait(&srcAccount->lock);
  Account_Adjust(bank, srcAccount, -amount, 1);
  sem_post(&srcAccount->lock);
  // dst + amount
  sem_wait(&dstAccount->lock);
  Account_Adjust(bank, dstAccount, amount, 1);
  sem_post(&dstAccount->lock);
  sem_post(&bank->teller_transfer_lock);*/
  //Teller_DoWithdraw(bank, srcAccountNum, amount);
  //Teller_DoDeposit(bank, dstAccountNum, amount);
  //return ERROR_SUCCESS;
}
