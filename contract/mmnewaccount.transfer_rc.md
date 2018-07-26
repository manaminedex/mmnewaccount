# CONTRACT FOR mmnewaccount::transfer

## ACTION NAME: transfer

### Parameters
Input parameters:

* `from` (sender)
* `to` (this contract)
* `quantity` (amount of EOS token)
* `memo` (\<new account name\>:\<public key\>)

### Intent
INTENT. The intention of the `{{ transfer }}` action is to pay EOS tokens to create a new account. Fee is 0.1 EOS.

### Term
TERM. This Contract expires at the conclusion of code execution.