# mmnewaccount
EOS account creator

This is a contract for https://account.manamine.net

### Install

```
cleos create account eosio mmnewaccount <public_key>
eosiocpp -o mmnewaccount/mmnewaccount.wast contract/mmnewaccount.cpp
cleos set contract mmnewaccount contract contract/mmnewaccount.wast contract/mmnewaccount.abi
```

### Usage

```
cleos transfer eosio mmnewaccount "3 EOS" "<new account name>:<public_key>"
```
or
```
cleos push action mmnewaccount newaccount '["3.0000 EOS", "<new_account_name>:<public_key>"]' -p mmnewaccount
```
