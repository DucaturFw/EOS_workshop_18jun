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
        exchanges(_self, _self)
  {}

  const static auto EXPIRATION_TIME = 5 * 60;

  //@abi action
  void exchange(const account_name from, const asset &quantity, const std::string &blockchain, const std::string& to)
  {
    eosio_assert(quantity.is_valid(), "invalid quantity");
    eosio_assert(quantity.amount > 0, "must deposit positive quantity");

    action(
        permission_level{from, N(active)},
        N(eosio.token), N(transfer),
        std::make_tuple(from, _self, quantity, blockchain))
        .send();

    auto exchange_offer_itr = exchanges.emplace(from, [&](auto &offer) {
      offer.id = exchanges.available_primary_key();
      offer.amount = quantity;
      offer.from = from;
      offer.to = to;
      offer.pubtime = eosio::time_point_sec(now());
      offer.blockchain = blockchain;
    });
    print("Your exchange request has id: ");
    printn(exchange_offer_itr->id);
  }

  //@abi action
  void expired(const uint64_t &id)
  {
    auto exchange_offer_itr = exchanges.find( id );
    eosio_assert( exchange_offer_itr != exchanges.end(), "unknown exchange request id" );
    eosio_assert( exchange_offer_itr->pubtime != eosio::time_point_sec(0)
                  && eosio::time_point_sec(now()) > exchange_offer_itr->pubtime + EXPIRATION_TIME,
                  "exchange not expired" );
    eosio_assert( exchange_offer_itr->txid == "", "Already exchanged" );

    action(
        permission_level{_self, N(active)},
        N(eosio.token), N(transfer),
        std::make_tuple(_self, exchange_offer_itr->from, exchange_offer_itr->amount, exchange_offer_itr->blockchain + exchange_offer_itr->to))
        .send();

    exchanges.modify( exchange_offer_itr, 0, [&]( auto& offer ) {
        offer.txid = "Failed due to expiration";
        offer.amount -= offer.amount; // set to 0
    });
  }

  //@abi action
  void close(const uint64_t &id, const std::string &txid)
  {
    require_auth( _self );
    auto exchange_offer_itr = exchanges.find( id );
    eosio_assert( exchange_offer_itr != exchanges.end(), "unknown exchange request id" );

    exchanges.modify( exchange_offer_itr, 0, [&]( auto& offer ) {
        offer.txid = txid;
    });
  }

  /**
   * Transfer tokens from specified `duc_master` account to `to` account.
   * Requires permissions for actor.
   */
  //@abi action
  void transfer(const account_name token_master, const account_name to,
                const asset &quantity, const std::string &memo)
  {
    eosio_assert(quantity.is_valid(), "invalid quantity");
    eosio_assert(quantity.amount > 0, "must deposit positive quantity");

    action(
        permission_level{token_master, N(active)},
        N(eosio.token), N(transfer),
        std::make_tuple(token_master, to, quantity, memo))
        .send();
  }

private:
  //@abi table exchanges i64
  struct exchange
  {
    uint64_t              id;
    account_name          from;
    std::string           to;
    asset                 amount;
    std::string           blockchain;
    eosio::time_point_sec pubtime;
    std::string           txid;

    uint64_t primary_key() const { return id; }

    bool is_empty() const { return !(amount.amount); }

    EOSLIB_SERIALIZE(exchange, (id)(from)(to)(amount)(blockchain)(pubtime)(txid))
  };

  typedef eosio::multi_index<N(exchange), exchange> exchange_index;

  exchange_index exchanges;
};

EOSIO_ABI(DUCExchanger, (exchange)(expired)(close)(transfer))