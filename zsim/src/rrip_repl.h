#ifndef RRIP_REPL_H_
#define RRIP_REPL_H_

#include "repl_policies.h"

// Static RRIP
class SRRIPReplPolicy : public ReplPolicy {
    protected:
        uint32_t* array;
        uint32_t numLines;
        uint32_t candidates;
        uint32_t rpvMax;

    public:
        // add member methods here, refer to repl_policies.h
        SRRIPReplPolicy(uint32_t _numLines, uint32_t _candidates, uint32_t _rpvMax)
            : numLines(_numLines), candidates(_candidates), rpvMax(_rpvMax) {
            array = gm_calloc<uint32_t>(numLines);  // Allocate memory for RRPVs
        }


        void update(uint32_t id, const MemReq* req) {
            array[id] = 0;    //update called on hit, SRRIP-HP updates hits to 0
        }


        void replaced(uint32_t id) {  // sets RRPV value of newly evicted line to rpvMax-1
            array[id] = rpvMax-1;
        }

        //Cache Miss:
        //(i) search for first ‘3’ from left
        //(ii) if ‘3’ found go to step (v)
        //(iii) increment all RRPVs (only candidates)
        //(iv) goto step (i)
        //(v) replace block and set RRPV to ‘2’

        template <typename C> inline uint32_t rank(const MemReq* req, C cands) {
            while (true)
            {
                for (auto ci = cands.begin(); ci != cands.end(); ci.inc())
                {
                    if (array[*ci] == rpvMax)
                    {
                    return *ci;
                    }
                }
                // if no rpv == 3, increment all rpv in candidate list
                for (auto ci = cands.begin(); ci != cands.end(); ci.inc())
                {
                    array[*ci]++;
                }
            }
            return -1; //return invalid index if failure but shouldn't get here
        }

        ~SRRIPReplPolicy() {
            gm_free(array);
        }
        DECL_RANK_BINDINGS;
};
#endif // RRIP_REPL_H_
