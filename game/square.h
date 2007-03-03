#ifndef SQUARE_H
#define SQUARE_H

namespace bovo {
    /** exception busy, square is already ocupied by a player */
    struct busy {};

    class square {
    public:
        square() {m_player=0;}
        unsigned short int player() const { return m_player; }
        bool empty() const { return m_player==0; }
        void setPlayer(const unsigned short int val) throw(busy) { if(m_player==0) m_player = val; else throw busy(); }
    private:
        unsigned short int m_player;
    };
}

#endif
