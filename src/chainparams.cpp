// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2017 The PIVX developers
// Copyright (c) 2018 The LAPO developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "libzerocoin/Params.h"
#include "chainparams.h"
#include "random.h"
#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/thread.hpp>
#include <boost/assign/list_of.hpp>

using namespace std;
using namespace boost::assign;

struct SeedSpec6 {
    uint8_t addr[16];
    uint16_t port;
};

#include "chainparamsseeds.h"

/**
 * Main network
 */

//! Convert the pnSeeds6 array into usable address objects.
static void convertSeed6(std::vector<CAddress>& vSeedsOut, const SeedSpec6* data, unsigned int count)
{
    // It'll only connect to one or two seed nodes because once it connects,
    // it'll get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7 * 24 * 60 * 60;
    for (unsigned int i = 0; i < count; i++) {
        struct in6_addr ip;
        memcpy(&ip, data[i].addr, sizeof(ip));
        CAddress addr(CService(ip, data[i].port));
        addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

//   What makes a good checkpoint block?
// + Is surrounded by blocks with reasonable timestamps
//   (no blocks before with a timestamp after, none after with
//    timestamp before)
// + Contains no strange transactions

//2018-06-06 16:41:48 UpdateTip: 
// new best=00000005675e39f424018a5fb49f88db62da3b2802111405a6762dc3bff2c7fb  
// height=13453  log2_work=41.832572  tx=31683 
// timestamp=1528303327 date=2018-06-06 16:42:07 progress=1.000014  cache=179

static Checkpoints::MapCheckpoints mapCheckpoints =
    boost::assign::map_list_of
    (0, uint256("0x00000275ae0b7510476dda65b159fb117f5b5db470816c2711589bb8038bac93"))
    (13453, uint256("00000005675e39f424018a5fb49f88db62da3b2802111405a6762dc3bff2c7fb")); // lyra2z
//    (0, uint256("0x0000080552e8b0e79c26a1601c8716a772f7df9669c0a1b830d949d0b7bd7bea")); // quark
static const Checkpoints::CCheckpointData data = {
    &mapCheckpoints,
    1528303327, // * UNIX timestamp of last checkpoint block
    31683,    // * total number of transactions between genesis and last checkpoint
                //   (the tx=... number in the SetBestChain debug.log lines)
    2000        // * estimated number of transactions per day after checkpoint
};

static Checkpoints::MapCheckpoints mapCheckpointsTestnet =
    boost::assign::map_list_of
    (0, uint256("0x00000cdcec67bf42aa2f3eb9e727ccedbd2352546d0858dcbe46386610b4c4ba")); // lyra2z
//    (0, uint256("0x00000cdcec67bf42aa2f3eb9e727ccedbd2352546d0858dcbe46386610b4c4ba")); // quark
static const Checkpoints::CCheckpointData dataTestnet = {
    &mapCheckpointsTestnet,
    1522904889,
    0,
    250};

static Checkpoints::MapCheckpoints mapCheckpointsRegtest =
    boost::assign::map_list_of
    (0, uint256("0x00000d2bbf718e0d4ee2d36d14ff83dad49e66178717d7b5933719bca57a0cab")); // lyra2z
//    (0, uint256("0x000001733877023e9a2751258b8119e420e153377ffd21c996af58c8cdceede5")); // quark
static const Checkpoints::CCheckpointData dataRegtest = {
    &mapCheckpointsRegtest,
    1522904890,
    0,
    100};

libzerocoin::ZerocoinParams* CChainParams::Zerocoin_Params() const
{
    assert(this);
    static CBigNum bnTrustedModulus(zerocoinModulus);
    static libzerocoin::ZerocoinParams ZCParams = libzerocoin::ZerocoinParams(bnTrustedModulus);

    return &ZCParams;
}

class CMainParams : public CChainParams
{
public:
    CMainParams()
    {
        networkID = CBaseChainParams::MAIN;
        strNetworkID = "main";
        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 4-byte int at any alignment.
         */
        pchMessageStart[0] = 0x1a;
        pchMessageStart[1] = 0x20;
        pchMessageStart[2] = 0x2a;
        pchMessageStart[3] = 0x20;
        vAlertPubKey = ParseHex("0463c160476f87452235ea37b1a9a3a09d5ce841a920e29d2aea725a2ba0d406e3b71aaf5420f4e73ab00d3119ff64c83f0d55d1b9b4f0a594821c75d15b97ec39");
        nDefaultPort = 31714;
        bnProofOfWorkLimit = ~uint256(0) >> 20; // LAPO starting difficulty is 1 / 2^12
        nSubsidyHalvingInterval = 5167800;   // HALVING EVERY: 5167800 BLOCKS
        nSubsidyBudgetPercentage = 10;      // Must be less than 100
        nMaxReorganizationDepth = 100;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 0;
        nAvailableThreads = boost::thread::hardware_concurrency();
        nTargetTimespan = 60 * 60; // LAPO: 1 hr
        nTargetSpacing1 = 30;  // LAPO: 30 seconds
        nTargetSpacing2 = 60;  // LAPO: 60 seconds
        nMaturity = 100;
        nMasternodeCollateral = 1000000; // 1 million
        nMasternodeCountDrift = 20;
        
        nMaxMoneyOut = 25 * 1000000000LL * COIN; // 100 billion
        nLPCtxMinFee = 0.01 * COIN;             // 1 CENT or 10000 satoshis
        nMinTxFeePerK = 0.0001 * COIN;            // .01 CENT or 100 satoshis
        nMinRelayTxFeePerK = 0.0001 * COIN;       // .01 CENT or 100 satoshis

        /** Height or Time Based Activations **/
        nLastPOWBlock = 795500;
        nModifierUpdateBlock = 615800;
        nBlockEnforceSerialRange = 1; //Enforce serial range starting this block
        nBlockRecalculateAccumulators = -1; //Trigger a recalculation of accumulators
        nBlockFirstFraudulent = -1; //First block that bad serials emerged
        nBlockLastGoodCheckpoint = 0; //Last valid accumulator checkpoint
        nBlockEnforceInvalidUTXO = 1; //Start enforcing the invalid UTXO's

        /*
        ```
            python ~/gig8/GenesisH0/genesis.py -a lyra2z \
              -z "Switzerland and Japan in Fintech Accord" \
              -t 1522904888 -v 0 \
              -p 04653d832851900e4b7b82abf3e41952b63077b46c5cc62587dd59c8c1b3e50f0edea8048801497b33d8703fbab5c0b3819a4426b6e05f5fb396bd0ed2c586daa8
            ```
            04ffff001d010427537769747a65726c616e6420616e64204a6170616e20696e2046696e74656368204163636f7264
            algorithm: lyra2z
            merkle hash: d19bad4d4a36b21c6a96e44767cfe775a179a2ce63b80d1511c5af2eb048495e
            pszTimestamp: Switzerland and Japan in Fintech Accord
            pubkey: 04653d832851900e4b7b82abf3e41952b63077b46c5cc62587dd59c8c1b3e50f0edea8048801497b33d8703fbab5c0b3819a4426b6e05f5fb396bd0ed2c586daa8
            time: 1522904888
            bits: 0x1e0ffff0
            Searching for genesis hash..
            17113.0 hash/s, estimate: 69.7 hgenesis hash found!
            nonce: 1006553
            genesis hash: 00000275ae0b7510476dda65b159fb117f5b5db470816c2711589bb8038bac93
        */
        const char* pszTimestamp = "Switzerland and Japan in Fintech Accord";
        CMutableTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 0 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("04653d832851900e4b7b82abf3e41952b63077b46c5cc62587dd59c8c1b3e50f0edea8048801497b33d8703fbab5c0b3819a4426b6e05f5fb396bd0ed2c586daa8") << OP_CHECKSIG;
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime = 1522904888;
        genesis.nBits = 0x1e0ffff0;
        genesis.nNonce = 1006553;   // lyra2z
//        genesis.nNonce = 160867;   // quark

        hashGenesisBlock = genesis.GetHash();
        assert(genesis.hashMerkleRoot == uint256("0xd19bad4d4a36b21c6a96e44767cfe775a179a2ce63b80d1511c5af2eb048495e"));
        assert(hashGenesisBlock == uint256("0x00000275ae0b7510476dda65b159fb117f5b5db470816c2711589bb8038bac93"));  // lyra2z
//        assert(hashGenesisBlock == uint256("0x0000080552e8b0e79c26a1601c8716a772f7df9669c0a1b830d949d0b7bd7bea"));  // quark

        // Zerocoin, activated by default
        nZerocoinStartHeight = 1;
        nZerocoinStartTime = genesis.nTime;

        vSeeds.push_back(CDNSSeedData("lapo.io", "seeds.lapo.io"));             // Primary DNS Seeder
        vSeeds.push_back(CDNSSeedData("gig8.com", "seeds.lapo.gig8.com"));      // Secondary DNS Seeder

        // https://en.bitcoin.it/wiki/List_of_address_prefixes        
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 48);     // starts with L
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 63);     // starts with S
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 75);        // starts with X
        
        /* if tempted to change below consider https://github.com/bitcoin/bips/blob/master/bip-0043.mediawiki
            Because this scheme can be used to generate nodes for more cryptocurrencies at once, 
            or even something totally unrelated to cryptocurrencies, there's no point in using a 
            special version magic described in section "Serialization format" of BIP32. We suggest 
            to use always 0x0488B21E for public and 0x0488ADE4 for private nodes (leading to 
            prefixes "xpub" and "xprv" respectively).
        */
                
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();
        // 	BIP44 coin type is from https://github.com/satoshilabs/slips/blob/master/slip-0044.md
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x1a)(0x20)(0x10).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = true;
        fAllowMinDifficultyBlocks = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fSkipProofOfWorkCheck = false;
        fTestnetToBeDeprecatedFieldRPC = false;
        fHeadersFirstSyncingActive = false;

        nPoolMaxTransactions = 3;
        strSporkKey = "0450f7ce7ee8a3b3a6581ed9665399c32081528acbe7821a101a512ab518114dfa1a14bd9a70acb8bcc474e12512188769046e61e4041e6680ef2486c98bd3c09b";
        strObfuscationPoolDummyAddress = "D87q2gC9j6nNrnzCsg4aY6bHMLsT9nUhEw";
        nStartMasternodePayments = genesis.nTime + 60 * 60; // 1 hr after genesis

        /** Zerocoin */
        // See https://github.com/Zerocoin/libzerocoin/wiki/Generating-Zerocoin-parameters
        // See https://pivx.org/zpiv/
        // http://zerocoin.org/media/pdf/ZerocoinOakland.pdf
        zerocoinModulus = "25195908475657893494027183240048398571429282126204032027777137836043662020707595556264018525880784"
            "4069182906412495150821892985591491761845028084891200728449926873928072877767359714183472702618963750149718246911"
            "6507761337985909570009733045974880842840179742910064245869181719511874612151517265463228221686998754918242243363"
            "7259085141865462043576798423387184774447920739934236584823824281198163815010674810451660377306056201619676256133"
            "8441436038339044149526344321901146575444541784240209246165157233507787077498171257724679629263863563732899121548"
            "31438167899885040445364023527381951378636564391212010397122822120720357";
        nMaxZerocoinSpendsPerTransaction = 7; // Assume about 20kb each
        nMinZerocoinMintFee = 1 * CENT; //high fee required for zerocoin mints
        nMintRequiredConfirmations = 20; //the maximum amount of confirmations until accumulated in 19
        nRequiredAccumulation = 1;
        nDefaultSecurityLevel = 100; //full security level for accumulators
        nZerocoinHeaderVersion = 4; //Block headers must be this version once zerocoin is active
        nBudget_Fee_Confirmations = 6; // Number of confirmations for the finalization fee
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return data;
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CMainParams
{
public:
    CTestNetParams()
    {
        networkID = CBaseChainParams::TESTNET;
        strNetworkID = "test";
        pchMessageStart[0] = 0x1a;
        pchMessageStart[1] = 0x20;
        pchMessageStart[2] = 0x2a;
        pchMessageStart[3] = 0x21;
        vAlertPubKey = ParseHex("04c4f90914523877410f4ffc7d33946b124fea5fbbebc56781151c3c316da82f357ef6d0c43a612a6c2a8652569400056463a5685e20ea8ddcd526515a507139ca");
        nDefaultPort = 31715;
        nEnforceBlockUpgradeMajority = 51;
        nRejectBlockOutdatedMajority = 75;
        nToCheckBlockUpgradeMajority = 100;
        nMinerThreads = 0;
        nTargetTimespan = 60 * 60; // LAPO: 1 hr
        nTargetSpacing1 = 30;  // LAPO: 30 seconds
        nTargetSpacing2 = 60;  // LAPO: 60 seconds

        nLastPOWBlock = 1051200;
        nMaturity = 15;
        nMasternodeCountDrift = 4;
        nModifierUpdateBlock = 51197; //approx Mon, 17 Apr 2017 04:00:00 GMT
        nMaxMoneyOut = 100 * 1000000000LL * COIN; // 100 billion
        
        // nBlockEnforceSerialRange = 1; //Enforce serial range starting this block
        // nBlockRecalculateAccumulators = -1; //Trigger a recalculation of accumulators
        // nBlockFirstFraudulent = -1; //First block that bad serials emerged
        // nBlockLastGoodCheckpoint = 0; //Last valid accumulator checkpoint
        // nBlockEnforceInvalidUTXO = 1; //Start enforcing the invalid UTXO's
        
        /*
            ```
            python ~/gig8/GenesisH0/genesis.py -a lyra2z \
              -z "Switzerland and Japan in Fintech Accord" \
              -t 1522904889 -v 0 \
              -p 04653d832851900e4b7b82abf3e41952b63077b46c5cc62587dd59c8c1b3e50f0edea8048801497b33d8703fbab5c0b3819a4426b6e05f5fb396bd0ed2c586daa8
            ```
            04ffff001d010427537769747a65726c616e6420616e64204a6170616e20696e2046696e74656368204163636f7264
            algorithm: lyra2z
            merkle hash: d19bad4d4a36b21c6a96e44767cfe775a179a2ce63b80d1511c5af2eb048495e
            pszTimestamp: Switzerland and Japan in Fintech Accord
            pubkey: 04653d832851900e4b7b82abf3e41952b63077b46c5cc62587dd59c8c1b3e50f0edea8048801497b33d8703fbab5c0b3819a4426b6e05f5fb396bd0ed2c586daa8
            time: 1522904889
            bits: 0x1e0ffff0
            Searching for genesis hash..
            17220.0 hash/s, estimate: 69.3 hgenesis hash found!
            nonce: 4446819
            genesis hash: 00000cdcec67bf42aa2f3eb9e727ccedbd2352546d0858dcbe46386610b4c4ba
        */
        //! Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.nTime = 1522904889;
        genesis.nBits = 0x1e0ffff0;
        genesis.nNonce = 4446819;   // lyra2z
//        genesis.nNonce = 1064847;   // quark

        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("0x00000cdcec67bf42aa2f3eb9e727ccedbd2352546d0858dcbe46386610b4c4ba"));    // lyra2z
//        assert(hashGenesisBlock == uint256("0x00000f5b8c7b218e67a1af25bb8aa1f13e147077e52ee21b707b6608ab3d7f48"));  //quark

        // Zerocoin, activated by default
        nZerocoinStartHeight = 1;
        nZerocoinStartTime = genesis.nTime;

        vFixedSeeds.clear();
        vSeeds.clear();
        vSeeds.push_back(CDNSSeedData("lapo.io", "testnet-seeds.lapo.io"));             // Primary DNS Seeder
        vSeeds.push_back(CDNSSeedData("gig8.com", "testnet-seeds.lapo.gig8.com"));      // Secondary DNS Seeder

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 109); // Testnet lapo addresses start with 'k or m' hopefully 'l'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 125);  // Testnet lapo script addresses start with '9'
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 137);     // Testnet private keys start with 'x'
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();
        // Testnet lapo BIP44 coin type is '1' (All coin's testnet default)
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x00)(0x00)(0x01).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_test, ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fAllowMinDifficultyBlocks = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        nPoolMaxTransactions = 2;
        strSporkKey = "04fa6933467aa4ee62432e739c913179c7a320d413322edfde88597768f600bf8c3c5f44991fd5f0e563c6fd1a423fd284cd54ba56368d51d02f84fe127084c300";
        strObfuscationPoolDummyAddress = "y57cqfGRkekRyDRNeJiLtYVEbvhXrNbmox";
        nStartMasternodePayments = genesis.nTime + 60 * 60; // 1 hr after genesis
        nBudget_Fee_Confirmations = 3; // Number of confirmations for the finalization fee. We have to make this very short
                                       // here because we only have a 8 block finalization window on testnet
    }
    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataTestnet;
    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CTestNetParams
{
public:
    CRegTestParams()
    {
        networkID = CBaseChainParams::REGTEST;
        strNetworkID = "regtest";
        strNetworkID = "regtest";
        pchMessageStart[0] = 0x1a;
        pchMessageStart[1] = 0x20;
        pchMessageStart[2] = 0x2a;
        pchMessageStart[3] = 0x22;
        nSubsidyHalvingInterval = 150;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 1;
        nTargetTimespan = 60 * 60; // LAPO: 1 hour
        nTargetSpacing1 = 30;  // LAPO: 30 seconds
        nTargetSpacing2 = 60;  // LAPO: 60 seconds

        bnProofOfWorkLimit = ~uint256(0) >> 1;
        
        /*
            ```
            python ~/gig8/GenesisH0/genesis.py -a lyra2z \
              -z "Switzerland and Japan in Fintech Accord" \
              -t 1522904890 -v 0 \
              -p 04653d832851900e4b7b82abf3e41952b63077b46c5cc62587dd59c8c1b3e50f0edea8048801497b33d8703fbab5c0b3819a4426b6e05f5fb396bd0ed2c586daa8
            ```
            004ffff001d010427537769747a65726c616e6420616e64204a6170616e20696e2046696e74656368204163636f7264
            algorithm: lyra2z
            merkle hash: d19bad4d4a36b21c6a96e44767cfe775a179a2ce63b80d1511c5af2eb048495e
            pszTimestamp: Switzerland and Japan in Fintech Accord
            pubkey: 04653d832851900e4b7b82abf3e41952b63077b46c5cc62587dd59c8c1b3e50f0edea8048801497b33d8703fbab5c0b3819a4426b6e05f5fb396bd0ed2c586daa8
            time: 1522904890
            bits: 0x1e0ffff0
            Searching for genesis hash..
            15854.0 hash/s, estimate: 75.3 hgenesis hash found!
            nonce: 1357959
            genesis hash: 00000d2bbf718e0d4ee2d36d14ff83dad49e66178717d7b5933719bca57a0cab
        */
        genesis.nTime = 1522904890;
        genesis.nBits = 0x1e0ffff0;
        genesis.nNonce = 1357959; // lyra2z
//        genesis.nNonce = 1416129;   // quark

        hashGenesisBlock = genesis.GetHash();
        nDefaultPort = 31716;
        assert(hashGenesisBlock == uint256("0x00000d2bbf718e0d4ee2d36d14ff83dad49e66178717d7b5933719bca57a0cab"));    // lyra2z
//        assert(hashGenesisBlock == uint256("0x000001733877023e9a2751258b8119e420e153377ffd21c996af58c8cdceede5"));  // quark

        vFixedSeeds.clear(); //! Testnet mode doesn't have any fixed seeds.
        vSeeds.clear();      //! Testnet mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fAllowMinDifficultyBlocks = true;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;
    }
    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataRegtest;
    }
};
static CRegTestParams regTestParams;

/**
 * Unit test
 */
class CUnitTestParams : public CMainParams, public CModifiableParams
{
public:
    CUnitTestParams()
    {
        networkID = CBaseChainParams::UNITTEST;
        strNetworkID = "unittest";
        nDefaultPort = 31717;
        vFixedSeeds.clear(); //! Unit test mode doesn't have any fixed seeds.
        vSeeds.clear();      //! Unit test mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fAllowMinDifficultyBlocks = false;
        fMineBlocksOnDemand = true;
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        // UnitTest share the same checkpoints as MAIN
        return data;
    }

    //! Published setters to allow changing values in unit test cases
    virtual void setSubsidyHalvingInterval(int anSubsidyHalvingInterval) { nSubsidyHalvingInterval = anSubsidyHalvingInterval; }
    virtual void setEnforceBlockUpgradeMajority(int anEnforceBlockUpgradeMajority) { nEnforceBlockUpgradeMajority = anEnforceBlockUpgradeMajority; }
    virtual void setRejectBlockOutdatedMajority(int anRejectBlockOutdatedMajority) { nRejectBlockOutdatedMajority = anRejectBlockOutdatedMajority; }
    virtual void setToCheckBlockUpgradeMajority(int anToCheckBlockUpgradeMajority) { nToCheckBlockUpgradeMajority = anToCheckBlockUpgradeMajority; }
    virtual void setDefaultConsistencyChecks(bool afDefaultConsistencyChecks) { fDefaultConsistencyChecks = afDefaultConsistencyChecks; }
    virtual void setAllowMinDifficultyBlocks(bool afAllowMinDifficultyBlocks) { fAllowMinDifficultyBlocks = afAllowMinDifficultyBlocks; }
    virtual void setSkipProofOfWorkCheck(bool afSkipProofOfWorkCheck) { fSkipProofOfWorkCheck = afSkipProofOfWorkCheck; }
};
static CUnitTestParams unitTestParams;


static CChainParams* pCurrentParams = 0;

CModifiableParams* ModifiableParams()
{
    assert(pCurrentParams);
    assert(pCurrentParams == &unitTestParams);
    return (CModifiableParams*)&unitTestParams;
}

const CChainParams& Params()
{
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(CBaseChainParams::Network network)
{
    switch (network) {
    case CBaseChainParams::MAIN:
        return mainParams;
    case CBaseChainParams::TESTNET:
        return testNetParams;
    case CBaseChainParams::REGTEST:
        return regTestParams;
    case CBaseChainParams::UNITTEST:
        return unitTestParams;
    default:
        assert(false && "Unimplemented network");
        return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectParams(network);
    return true;
}
