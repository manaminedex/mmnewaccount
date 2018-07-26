/**
 *  @file
 *  @copyright defined in LICENSE.txt
 */
#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/public_key.hpp>
#include "abieos_numeric.hpp"

using namespace std;
using namespace eosio;

class mmnewaccount : public contract {
public:

   mmnewaccount(account_name self) : contract(self) {}

   void transfer(account_name from, account_name to, asset quantity, string memo) {
      if (from == _self || to != _self)
         return;

      if (from == N(eosio.ram) || from == N(eosio.stake) || memo == "sell ram" || memo == "unstake")
         return;

      if (from == N(manaminemain))  // Accept EOS from master account
         return;

      newaccount(quantity, memo);
   }

   //@abi action
   void newaccount(asset quantity, string memo) {
      eosio_assert(quantity.symbol == CORE_SYMBOL, "Only accept EOS asset");
      eosio_assert(quantity.is_valid(), "Invalid quantity");

      asset netcost(1000); // NET (0.1 EOS)
      asset cpucost(1000); // CPU (0.1 EOS)
      asset fee(1000);     // fee (0.1 EOS)
      asset ramcost = quantity - netcost - cpucost - fee;   // RAM = quantity - NET - CPU - fee

      eosio_assert(ramcost.amount > 0, "RAM cost is not sufficient");

      // memo : "<newacntname>:<public_key>"
      memo.erase(memo.begin(), find_if(memo.begin(), memo.end(), [](int ch) {
         return !isspace(ch);
      }));
      memo.erase(find_if(memo.rbegin(), memo.rend(), [](int ch) {
         return !isspace(ch);
      }).base(), memo.end());

      auto sep_pos = memo.find(':');
      eosio_assert(sep_pos != string::npos, "memo must be separated with colon");

      string acntname_str = memo.substr(0, sep_pos);
      eosio_assert(acntname_str.length() == 12, "account name length must be 12");
      account_name newacntname = string_to_name(acntname_str.c_str());

      string pubkey_str = memo.substr(sep_pos + 1);
      eosio_assert(pubkey_str.length() == 53 || pubkey_str.length() == 57,
                   "public key length must be 53(K1) or 57(R1)");

      const abieos::public_key pubkey = abieos::string_to_public_key(pubkey_str);
      array<char,33> pubkey_char;
      copy(pubkey.data.begin(), pubkey.data.end(), pubkey_char.begin());

      authority authkey = {
         .threshold = 1,
         .keys = {
            // vector<key_weight>
            {
               // eosio::public_key
               {
                  // eosio::public_key { .type, .data }
                  //(uint8_t)abieos::key_type::k1, pubkey_char
                  (uint8_t)pubkey.type, pubkey_char
               },
               // weight_type
               1
            }
         },
         .accounts = {},
         .waits = {}
      };

      action(
         permission_level{ _self, N(active) },
         N(eosio), N(newaccount),
         make_tuple(_self, newacntname, authkey, authkey)
      ).send();

      action(
         permission_level{ _self, N(active)},
         N(eosio), N(buyram),
         make_tuple(_self, newacntname, ramcost)
      ).send();

      action(
         permission_level{ _self, N(active)},
         N(eosio), N(delegatebw),
         make_tuple(_self, newacntname, netcost, cpucost, true)
      ).send();

      print("{\"account\":\"", name{newacntname}, "\",\"ramcost\":\"", ramcost, "\"}");
   }

private:
   struct key_weight {
      eosio::public_key key;
      weight_type       weight;
   };
   struct permission_level_weight {
      permission_level  permission;
      weight_type       weight;
   };
   struct wait_weight {
      uint32_t    wait_sec;
      weight_type weight;
   };
   struct authority {
      uint32_t                         threshold;
      vector<key_weight>               keys;
      vector<permission_level_weight>  accounts;
      vector<wait_weight>              waits;
   };
};

#define EOSIO_ABI_EX( TYPE, MEMBERS ) \
extern "C" { \
   void apply( uint64_t receiver, uint64_t code, uint64_t action ) { \
      if( action == N(onerror) ) { \
         /* onerror is only valid if it is for the "eosio" code account and authorized by "eosio"'s "active permission */ \
         eosio_assert(code == N(eosio), "onerror action's are only valid from the \"eosio\" system account"); \
      } \
      auto self = receiver; \
      if( (code == N(eosio.token) && action == N(transfer)) || \
          (code == self && action == N(newaccount)) || \
          (action == N(onerror))  ) { \
         TYPE thiscontract( self ); \
         if( action == N(newaccount) ) \
            require_auth(self); \
         switch( action ) { \
            EOSIO_API( TYPE, MEMBERS ) \
         } \
         /* does not allow destructor of thiscontract to run: eosio_exit(0); */ \
      } \
   } \
} \

EOSIO_ABI_EX(mmnewaccount, (transfer)(newaccount))
