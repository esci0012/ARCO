// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"


static const int nCheckpointSpan = 500;

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    //
    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    //
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        ( 0,   uint256("0x00000393a7de08ce23b3882ae7b5c1567e83bda4849ed24b52610a9b2541c6c9") ) // genesis
        ( 10000,   uint256("0x0000000000063417ad195c561ec8b6b894211c6d3fe122e797dc77e6d0ade650") ) // end of PoW
        ( 300000,   uint256("0x062b8908da998ae7df5b7b805fcc8967a9697ec720ac3afcf2e6d20ab90914cc") )
        ( 400000,   uint256("0xc00ba4a72b66085cb573c7b1d1a34732eb2a10d1a5d2df9db5629c2d50593d84") )
        ( 500000,   uint256("0x1a7fca1992590ac712252afa566df72560b2802a5661e5f37fe8ff661a8107a6") )
        ( 1000000,   uint256("0xa4ce3f7c12eccd78c797d29be6e5ac0ceed6685854970db7845a144461be5aab") )
        ( 1500000,   uint256("0xd7e5029ca9befa88d59b185701818219ec3ec0a1ef73188d39edaef287c044b5") )
        ( 1800000,   uint256("0xd815ae37bce7e01407fd31b7e56ea53a83241a0b243793246491133e8af7ccf5") )
        ( 2000000,   uint256("0x1bdb357438611a9b0c38270977294a5b597645e85a1d8e744f08afdd0244558f") )
        ( 2153782,   uint256("0x65ebd05b0f53743c82447e1451d3168c5e06df81396afa95dc296cbd78eebc9e") )
        ( 2217538,   uint256("0xf7e7e290f5d1ca707630b78f8f33cb89f58a427ddc48f3ba99584a4f35cd8aa6") )
        ( 2261540,   uint256("0x650bac3288e6aa452a12bc8f874ffce6b1fc44ed0e80d20aca9df4b0bacff97d") )
        ( 2351393,   uint256("0x96cd4d12a00c26139ba7be246f23f7a8ec3b72ff6901600d5662d5a7bc104faa") )
        ( 2401111,   uint256("0x040304917804bf41efd36b9a3a7b3588005fb04903176dbff58ebc970095c7bf") )
        ( 2523564,   uint256("0xf60b1f44a71b023035fcc1065314c6c30a50a100816c4cf358c651fbd6214f9a") )

    ;

    // TestNet has no checkpoints
    static MapCheckpoints mapCheckpointsTestnet;

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        if (checkpoints.empty())
            return 0;
        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }

    // Automatically select a suitable sync-checkpoint
    const CBlockIndex* AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;
        // Search backward for a block within max span and maturity window
        while (pindex->pprev && pindex->nHeight + nCheckpointSpan > pindexBest->nHeight)
            pindex = pindex->pprev;
        return pindex;
    }

    // Check against synchronized checkpoint
    bool CheckSync(int nHeight)
    {
        const CBlockIndex* pindexSync = AutoSelectSyncCheckpoint();

        if (nHeight <= pindexSync->nHeight)
            return false;
        return true;
    }
}
