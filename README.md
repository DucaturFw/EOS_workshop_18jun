## EOS workshop

# Token exchange

1. Create token.

`cleos push action eosio.token create '[ "ducat.exch", "7000000000.0000 DUCAT"]' -p eosio.token`

1. Issue to user.

```
cleos push action eosio.token issue '[ "user", "100.0000 DUCAT", "" ]' -p ducat.exch
cleos get currency balance eosio.token user DUC # get_balance
```

1. Grant permissions for token transfers.

`cleos set account permission user active '{"threshold": 1,"keys": [{"key": "${EOS_PUBKEY}","weight": 1}],"accounts": [{"permission":{"actor":"ducat.exch","permission":"eosio.code"},"weight":1}]}' owner -p user@active`

1. Call exchange from user account.

`cleos push action ducat.exch exchange '["user", "100.0000 DUCAT", "ETH"]' -p user`

1. ???

1. See token exchange request in table.

`cleos get table ducat.exch ducat.exch exoffers`

### Token Exchange contract interface:

#### exchange(const account_name from, const asset &quantity, const std::string &blockchain, const std::string &to)

Transfers money from `from` account to contract's account and creates pendning request for exchange to `blockchain` on `to` address.
Prints id to the console.

#### expired(const uint64_t &id)

Returns money back if the specified exchange request is expired (by default, could be requested after 5 minutes since publication).

#### close(const uint64_t &id, const std::string &txid)

Marks exchange request as complete, sets txid from the target blockchain. Requires contract authorization for action, as update concerned internal.

#### transfer(const account_name token_master, const account_name to, const asset &quantity, const std::string &memo)

Transfer quantity of asset from external blockchains to user `to` from tokens holder `token_master`. Requires authorization/permissions for token_master.

### EOS js manipulation

#### eos_workshop

Application based on react demonstrates actions and HTTP API calls for EOS nodes.

## TODO: Add online manipulations (EOS, Scatter) and link to the video