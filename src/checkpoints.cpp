// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2017 The Lemanum Project www.lemanum.io
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

    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        (  0,     uint256("0x0000059d973811ff6a9c9d57fd43f3c9b48f6ee1439225af1fc4def9d932dd0b"))
        ( 280,    uint256("0x558eef5ec3a5da6a7e4071c7ef0fef5752d62a95d37d72794e0beaa8470521d4"))
        ( 1000,   uint256("0xdfe49c78f988787426fdab060ecf173241544482500e82940f6cbc9f57020a9f"))
        ( 2500,   uint256("0xc38a3b9a8b1ff11aed8b6edb846334adb069831f6ea0dae2565528120ee6281a"))
        ( 5835,   uint256("0x74255be4dd4b54e45ecd4f7a000f1afe95a47d854666fc73f4784e0d87c8e0be"))
        ( 7853,   uint256("0xae2f2497989ce8c4035f0d02b5b5f4fc9756753a9d43959bf6108e46f375c6db"))
        ( 10000,  uint256("0x068eedf7395f1215d00d1b698f0a8f4b25cd42650fd00e90a0a4f953d46dcfdb")) 
        ( 12500,  uint256("0x493aa5e660443e95fc879f44b14f23b32a3643f75e52a7df098201653e198f70"))
        ( 19000,  uint256("0x1465f3dbdf0b64131cbbd074dd13e7bac342079dadf5e34d82061c4174e378a9"))
        ( 25000,  uint256("0x7d4f830d809a308436d743568b0678945340d89536ba0e3971cb1b080714531b"))
        ( 28000,  uint256("0x1b946dc7fca7d55fccd51f9ddea7ef53ed7a4d70de4810d5683ff0226b3bb608"))
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
