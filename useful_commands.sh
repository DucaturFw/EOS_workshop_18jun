cleos wallet unlock
cleos create account eosio tester2 $EOS_PUBKEY $EOS_PUBKEY
cleos create account eosio ducat.exch $EOS_PUBKEY $EOS_PUBKEY

cleos push action eosio.token create '[ "ducat.exch", "7000000000.0000 DUCAT"]' -p eosio.token

eosiocpp -o ducat.token.wast ducat.token.cpp && eosiocpp -g ducat.token.abi  ducat.token.cpp
cleos set contract ducat.exch ~/eos/ducat.token -p ducat.exch

cleos push action eosio updateauth '{"account":"tester2","permission":"active","parent":"owner","auth":{"keys":[{"key":"${EOS_PUBKEY}", "weight":1}],"threshold":1,"accounts":[{"permission":{"actor":"ducat.exchange","permission":"eosio.code"},"weight":1}],"waits":[]}}' -p tester2@active
cleos set account permission tester1 active '{"threshold": 1,"keys": [{"key": "${EOS_PUBKEY}","weight": 1}],"accounts": [{"permission":{"actor":"ducat.exch","permission":"eosio.code"},"weight":1}]}' owner -p tester1@active

cleos get account ducat.exchange && cleos get account tester2 # show permissions on accounts

cleos push action eosio.token issue '[ "tester1", "100.0000 DUC", "" ]' -p ducat.exch
cleos get currency balance eosio.token tester2 DUC # get_balance

cleos push action ducat.exch exchange '["tester1", "50.0000 DUC", "ETH"]' -p tester1 # request
cleos get currency balance eosio.token ducat.exch DUC

cleos get table ducat.exch ducat.exch exoffer # show exchanges list

