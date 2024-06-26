#include "cellenumerator.h"
#include "solid.h"

namespace sdfibm {

void CellEnumerator::_next()
{
    int icur = m_queue.front();
    forAll(m_c2c[icur], _)
    {
        label inb = m_c2c[icur][_];
        if (m_ct[inb] == CELL_TYPE::UNVISITED) // unvisited
        {
            int n_v_inside = CountVertexInside(inb, pred_);

            if (n_v_inside==0)
            {
                m_ct[inb] = CELL_TYPE::ALL_OUTSIDE;
                is_[m_ct[inb]].insert(inb);
                continue;
            }

            // at this point the cell is both unvisited and intersected
            m_queue.push(inb);
            if (n_v_inside == m_c2p[icur].size())
                m_ct[inb] = CELL_TYPE::ALL_INSIDE;
            else if (pred_(m_cc[inb]))
                m_ct[inb] = CELL_TYPE::CENTER_INSIDE;
            else
                m_ct[inb] = CELL_TYPE::CENTER_OUTSIDE;
            is_[m_ct[inb]].insert(inb);
        }
    }
}

int CellEnumerator::CountVertexInside(int icell, const Predicate& p) const
{
    int n_v_inside = 0;
    forAll(m_c2p[icell], _)
    {
        int nodeid = m_c2p[icell][_];
        n_v_inside += p(m_pp[nodeid]);
    }
    return n_v_inside;
}

CellEnumerator::CellEnumerator(const Foam::fvMesh& mesh, const Predicate& pred, int seed) :
    MeshInfo(mesh),
    m_ct(mesh.nCells(), CELL_TYPE::UNVISITED),
    pred_(pred)
{
    if (CountVertexInside(seed, pred_) == 0)
    {
        seed = -1;
        forAll(m_cc, icell)
        {
            if (CountVertexInside(icell, pred_) > 0)
            {
                seed = icell;
                break;
            }
        }
    }

    if (seed >= 0)
    {
        m_queue.push(seed);

        if (CountVertexInside(seed, pred_) == m_c2p[seed].size())
            m_ct[seed] = CELL_TYPE::ALL_INSIDE;
        else
            if (pred_(m_cc[seed]))
                m_ct[seed] = CELL_TYPE::CENTER_INSIDE;
            else
                m_ct[seed] = CELL_TYPE::CENTER_OUTSIDE;
        is_[m_ct[seed]].insert(seed);
    }
}

}
