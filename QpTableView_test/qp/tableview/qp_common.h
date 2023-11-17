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

typedef struct LABEL_STYLE
{
    Qt::Alignment align;

    QFont fnt;

    QColor color;

    LABEL_STYLE()
        :
          align( Qt::AlignCenter ),
          color(QColor())
    {

    }
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

}
QT_END_NAMESPACE

QT_END_HEADER

#endif // QP_COMMON_H
