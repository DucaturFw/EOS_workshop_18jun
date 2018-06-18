#include <utility>
#include <vector>
#include <string>
#include <eosiolib/eosio.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/contract.hpp>
#include <eosiolib/crypto.h>

using eosio::indexed_by;
using eosio::const_mem_fun;
using eosio::asset;
using eosio::permission_level;
using eosio::action;
using eosio::print;
using eosio::name;

class DUCExchanger : public eosio::contract {
   public:

      DUCExchanger(account_name self)
      :eosio::contract(self),
        exoffers(_self, _self)
      {}

      //@abi action
      void exchange( const account_name from, const asset& quantity, const std::string& to_chain ) { 
        //  require_auth( from );

         eosio_assert( to_chain == "ETH" || to_chain == "NEO" || to_chain == "eth" || to_chain == "neo",
                       "wrong target chain to export" );
         eosio_assert( quantity.is_valid(), "invalid quantity" );
         eosio_assert( quantity.amount > 0, "must deposit positive quantity" );

         auto itr = exoffers.find(from);
         if( itr == exoffers.end() ) {
            itr = exoffers.emplace(_self, [&](auto& offer){
               offer.owner = from;
            });
         }

         action(
            permission_level{ from, N(active) },
            N(eosio.token), N(transfer),
            std::make_tuple(from, _self, quantity, to_chain)
         ).send();

         exoffers.modify( itr, 0, [&]( auto& offer ) {
            offer.duc_balance = quantity;
            offer.to_chain = to_chain;
            offer.pubtime = eosio::time_point_sec(now());
         });
      }

   private:

      //@abi table exoffer i64
      struct exoffer {
         exoffer( account_name o = account_name() ):owner(o) {}

         account_name          owner;
         asset                 duc_balance;
         std::string           to_chain;
         eosio::time_point_sec pubtime;

         bool is_empty() const { return !( duc_balance.amount ); }

         uint64_t primary_key() const { return owner; }
         uint32_t by_pubtime() const { return pubtime.sec_since_epoch(); }

         EOSLIB_SERIALIZE( exoffer, (owner)(duc_balance)(to_chain)(pubtime) )
      };

      typedef eosio::multi_index< N(exoffer), exoffer> exoffer_index;

      exoffer_index     exoffers;
};

EOSIO_ABI( DUCExchanger, (exchange) )