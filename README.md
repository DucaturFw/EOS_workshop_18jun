## EOS workshop

# Token exchange

1. Create token.

`cleos push action eosio.token create '[ "ducat.exch", "7000000000.0000 DUCAT"]' -p eosio.token`

2. Issue to user.

```
cleos push action eosio.token issue '[ "user", "100.0000 DUCAT", "" ]' -p ducat.exch
cleos get currency balance eosio.token user DUC # get_balance
```

3. Grant permissions for token transfers.

`cleos set account permission user active '{"threshold": 1,"keys": [{"key": "${EOS_PUBKEY}","weight": 1}],"accounts": [{"permission":{"actor":"ducat.exch","permission":"eosio.code"},"weight":1}]}' owner -p user@active`

4. Call exchange from user account.

`cleos push action ducat.exch exchange '["user", "100.0000 DUCAT", "ETH"]' -p user`

5. ???

6. See token exchange request in table.

`cleos get table ducat.exch ducat.exch exoffers`


### Token Exchange contract interface:

#### exchange(const account_name from, const asset &quantity, const std::string &to_chain)

Transfers money from `from` account to contract's account and creates pendning request for exchange to `to_chain`.
Prints id to the console.

#### expired(const uint64_t &id)

Returns money back if the specified exchange request is expired (by default, could be requested after 5 minutes since publication).

#### close(const uint64_t &id)

Marks exchange request as closed / exchanged. Requires contract authorization for action, as update concerned internal.

#### transfer(const account_name duc_master, const account_name to, const asset &quantity, const std::string &memo)

Transfer quantity of asset from external blockchains to user `to` from token's issuer `duc_master`. Requires authorization/permissions for duc_master.

## TODO: Add online manipulations (EOS, Scatter) and link to the video