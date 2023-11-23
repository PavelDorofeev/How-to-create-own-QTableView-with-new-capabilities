#ifndef QP_COMMON_H
#define QP_COMMON_H

#include <QModelIndex>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

namespace qp
{
typedef struct CELL_NODES{

    static const int NODE_UNDEFINED ;

    int left; // 0,1,2
    int right;
    int top;
    int bottom;

    bool visible;

    CELL_NODES( )
        :
          top( NODE_UNDEFINED ),
          bottom( NODE_UNDEFINED ),
          left( NODE_UNDEFINED ),
          right ( NODE_UNDEFINED ),
          visible( false )
    {

    }

    CELL_NODES( int num1 )
        :
          left( num1 ),
          top( num1 ),
          right( num1 ),
          bottom( num1 ),
          visible( true )
    {

    }

    CELL_NODES( int num1, int num2)
        :
          left( num1),
          top( num1),
          right( num2),
          bottom( num1),
          visible( true )
    {

    }

    CELL_NODES( int num1, int num2 , int line1, int line2 )
        :
          left( num1),
          top( line1),
          right( num2),
          bottom( line2),
          visible( true )
    {

    }

    bool isEmpty()
    {
        if( left == NODE_UNDEFINED ||
                top == NODE_UNDEFINED ||
                right == NODE_UNDEFINED ||
                bottom == NODE_UNDEFINED )
        {
            return true;
        }

        return false;
    }

};

//typedef struct FONT_PARS{

//    int weight;
//    int pointSize;
//    int pixelSize;


//    FONT_PARS()
//        :
//          weight( QFont::Normal ),
//          pointSize( -1 ),
//          pixelSize( -1 )
//    {

//    }

//};

#ifndef QT_NO_DEBUG_STREAM
Q_GUI_EXPORT QDebug operator<<(QDebug, const QColor &);
#endif
#ifndef QT_NO_DATASTREAM
Q_GUI_EXPORT QDataStream &operator<<(QDataStream &, const QColor &);
Q_GUI_EXPORT QDataStream &operator>>(QDataStream &, QColor &);
#endif

typedef struct CELL_STYLE
{
public:
    Qt::Alignment align;

    QFont font;

    QColor color;

    CELL_STYLE()
        :
          align( Qt::AlignCenter ),
          color(QColor(Qt::gray)),
          font (QFont())
    {

    }


#ifndef QT_NO_DATASTREAM
//inline QDataStream& operator>>(QDataStream& s, CELL_STYLE& p)
//{
//    s >> " align " >> p.align >> " color: " >> p.color  >> " font:" >> p.fnt;
//    return s;
//}

//inline QDataStream& operator<<(QDataStream& s, const CELL_STYLE& p)
//{
//    s << " align ";// << p.align << " color: " << p.color  << " font:" <<p.fnt;
//    return s;
//}
#endif
private:
#ifndef QT_NO_DATASTREAM
    friend Q_GUI_EXPORT QDataStream &operator<<(QDataStream &, const CELL_STYLE &);
    friend Q_GUI_EXPORT QDataStream &operator>>(QDataStream &, CELL_STYLE &);
#endif

};



typedef enum FIELD_TYPE
{
    LABEL_FLD=-2,
    UNDEFINED_FLD=-1,
    UNKNOWN_VALUE = UNDEFINED_FLD
};

typedef enum SECTION_TYPE
{
    MODEL_FIELD =1,
    LABEL_FIELD
};

typedef struct SECTION
{
    SECTION_TYPE type;
    QModelIndex idx;

    SECTION() :
        type(MODEL_FIELD),
        idx(QModelIndex())
    {

    }

};

typedef struct aaa
{
    int xNum;
    int line;
    //int logicalNumber;
    SECTION_TYPE type;
    Qt::Orientation moveOrientation;
    bool handle;

    aaa()
        :
          xNum( UNKNOWN_VALUE),
          line( UNKNOWN_VALUE),
          //logicalNumber( UNKNOWN_VALUE),
          type( MODEL_FIELD),
          moveOrientation( Qt::Horizontal),
          handle ( false)
    {

    }
};


typedef struct CELL
{

    CELL() : line(qp::UNKNOWN_VALUE), xNum(qp::UNKNOWN_VALUE)
    {
    }

    CELL( const int &line, const int &xNum):  line(line), xNum(xNum)
    {
    }

    CELL &operator=(const CELL &other)
    {
        line = other.line; xNum = other.xNum; return *this;
    }

    int line;
    int xNum;
};

Q_INLINE_TEMPLATE bool operator==(const CELL &p1, const CELL &p2)
{
    return p1.line == p2.line && p1.xNum == p2.xNum;
}

Q_INLINE_TEMPLATE bool operator!=(const CELL &p1, const CELL &p2)
{
    return !(p1 == p2);
}

Q_INLINE_TEMPLATE bool operator<(const CELL &p1, const CELL &p2)
{
    return p1.line < p2.line || (!(p2.line < p1.line) && p1.xNum < p2.xNum);
}

Q_INLINE_TEMPLATE bool operator>(const CELL &p1, const CELL &p2)
{
    return p2 < p1;
}

Q_INLINE_TEMPLATE bool operator<=(const CELL &p1, const CELL &p2)
{
    return !(p2 < p1);
}

Q_INLINE_TEMPLATE bool operator>=(const CELL &p1, const CELL &p2)
{
    return !(p1 < p2);
}
Q_OUTOFLINE_TEMPLATE CELL qMakePair(const int &x, const int &y)
{
    return CELL(x, y);
}

#ifndef QT_NO_DATASTREAM
inline QDataStream& operator>>(QDataStream& s, CELL& p)
{
    s >> p.line >> p.xNum;
    return s;
}

inline QDataStream& operator<<(QDataStream& s, const CELL& p)
{
    s << p.line << p.xNum;
    return s;
}
#endif

// ----------------------------------------------------------------------------------
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QP_COMMON_H
