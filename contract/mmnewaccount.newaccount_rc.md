# CONTRACT FOR mmnewaccount::newaccount

## ACTION NAME: newaccount

### Parameters
Input parameters:

* `quantity` (amount of EOS token)
* `memo` (\<new account name\>:\<public key\>)

### Intent
INTENT. The intention of the `{{ newaccount }}` action is to create a new account.
0.1 EOS for Net, 0.1 EOS for CPU, 0.1 EOS as fee and the rest of `quantity` will be used for RAM.
This action can be executed only by contract account.

### Term
TERM. This Contract expires at the conclusion of code execution.