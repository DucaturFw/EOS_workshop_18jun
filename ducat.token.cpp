#include <utility>
#include <vector>
#include <string>
#include <eosiolib/eosio.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/contract.hpp>
#include <eosiolib/crypto.h>

using eosio::action;
using eosio::asset;
using eosio::const_mem_fun;
using eosio::indexed_by;
using eosio::name;
using eosio::permission_level;
using eosio::print;

class DUCExchanger : public eosio::contract
{
public:
  DUCExchanger(account_name self)
      : eosio::contract(self),
        exoffers(_self, _self)
  {}

  const static auto EXPIRATION_TIME = 5 * 60;

  //@abi action
  void exchange(const account_name from, const asset &quantity, const std::string &to_chain)
  {
    eosio_assert(quantity.is_valid(), "invalid quantity");
    eosio_assert(quantity.amount > 0, "must deposit positive quantity");

    action(
        permission_level{from, N(active)},
        N(eosio.token), N(transfer),
        std::make_tuple(from, _self, quantity, to_chain))
        .send();

    auto exchange_offer_itr = exoffers.emplace(from, [&](auto &offer) {
      offer.id = exoffers.available_primary_key();
      offer.duc_balance = quantity;
      offer.owner = from;
      offer.pubtime = eosio::time_point_sec(now());
    });
    print("Your exchange request has id: ");
    printn(exchange_offer_itr->id));
  }

  //@abi action
  void expired(const uint64_t &id)
  {
    auto exchange_offer_itr = exoffers.find( id );
    eosio_assert( exchange_offer_itr != exoffers.end(), "unknown exchange request id" );
    eosio_assert( exchange_offer_itr->pubtime != eosio::time_point_sec(0)
                  && eosio::time_point_sec(now()) > exchange_offer_itr->pubtime + EXPIRATION_TIME,
                  "exchange not expired" );

    action(
        permission_level{_self, N(active)},
        N(eosio.token), N(transfer),
        std::make_tuple(_self, exchange_offer_itr->owner, exchange_offer_itr->duc_balance, exchange_offer_itr->to_chain))
        .send();

    exoffers.modify( exchange_offer_itr, 0, [&]( auto& offer ) {
        offer.to_chain = "Failed due to expiration";
        offer.duc_balance -= offer.duc_balance; // set to 0
    });
  }

  //@abi action
  void close(const uint64_t &id)
  {
    require_auth( _self );
    auto exchange_offer_itr = exoffers.find( id );
    eosio_assert( exchange_offer_itr != exoffers.end(), "unknown exchange request id" );

    exoffers.modify( exchange_offer_itr, 0, [&]( auto& offer ) {
        offer.exchanged = true;
    });
    // Also, could be erased:
    // exoffers.erase( exchange_offer_itr );
  }

  /**
   * Transfer tokens from specified `duc_master` account to `to` account.
   * Requires permissions for actor.
   */
  //@abi action
  void transfer(const account_name duc_master, const account_name to,
                const asset &quantity, const std::string &memo)
  {
    eosio_assert(quantity.is_valid(), "invalid quantity");
    eosio_assert(quantity.amount > 0, "must deposit positive quantity");

    action(
        permission_level{duc_master, N(active)},
        N(eosio.token), N(transfer),
        std::make_tuple(duc_master, to, quantity, memo))
        .send();
  }

private:
  //@abi table exoffer i64
  struct exoffer
  {
    exoffer(account_name o = account_name()) : owner(o) {}

    uint64_t              id;
    account_name          owner;
    asset                 duc_balance;
    std::string           to_chain;
    eosio::time_point_sec pubtime;
    bool                  exchanged = false;

    bool is_empty() const { return !(duc_balance.amount); }

    uint64_t primary_key() const { return id; }
    uint32_t by_pubtime() const { return pubtime.sec_since_epoch(); }

    EOSLIB_SERIALIZE(exoffer, (owner)(duc_balance)(to_chain)(pubtime))
  };

  typedef eosio::multi_index<N(exoffer), exoffer> exoffer_index;

  exoffer_index exoffers;
};

EOSIO_ABI(DUCExchanger, (exchange)(close)(transfer)(expired))