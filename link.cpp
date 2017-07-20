#include <QtGui>

#include "link.h"
#include "node.h"
#include <QDebug>
#include <QFont>


EdgeLabel::EdgeLabel(Link *link , int size, QString labelText)
: QGraphicsTextItem( 0 )
{
    qDebug()<< "EdgeLabel:: creating new edgelabel and attaching it to link";
    setPlainText( labelText );
    setParentItem( link ); //auto disables child items like this, when link is disabled.
    m_Link = link;
    m_size = size;
    this->setFont( QFont ("Courier", size, QFont::Light, true) );
    setZValue( ZValueEdgeLabel );
}


EdgeLabel::~EdgeLabel()
{
}

void EdgeLabel::Replot()
{
    qreal offset = this->toPlainText().count()* this->font().pointSize() / 2 ;
    double x =  ( m_Link->fromNode()->x() + m_Link->toNode()->x() ) / 2.0 - offset;
    double y =  ( m_Link->fromNode()->y() + m_Link->toNode()->y() ) / 2.0 - this->font().pointSize();
    this->setPos(x,y);
}


Link::Link()
{
    m_PenSize = 1;
}

Link::Link(NodeItem *fromNode, NodeItem *toNode)
{
    myFromNode = fromNode;
    myToNode = toNode;

    myFromNode->addLink(this);
    myToNode->addLink(this);

//    setFlags(QGraphicsItem::ItemIsSelectable);
    setZValue(-1);

    setColor( QColor(207,207,2,100) );
    setPenSize( 1.0 );
    trackNodes();
    setFlags( ItemIsMovable | ItemIsSelectable | ItemSendsScenePositionChanges );
}

Link::~Link()
{
    myFromNode->removeLink(this);
    myToNode->removeLink(this);
}

NodeItem *Link::fromNode() const
{
    return myFromNode;
}

NodeItem *Link::toNode() const
{
    return myToNode;
}

void Link::setColor(const QColor &color)
{
    setPen( QPen( color, m_PenSize == 0? 1.0 : m_PenSize ) );
}

QColor Link::color() const
{
    return pen().color();
}

void Link::setPenSize(double size)
{
    m_PenSize = size;
    setPen( QPen( pen().color(), size ) );
}

double Link::penSize() const
{
    return pen().widthF();
}

void Link::trackNodes()
{
    QPointF fromNode = myFromNode->pos();
    fromNode.setY(fromNode.y() + myFromNode->boundingRect().height()/2);
    setLine(QLineF( fromNode, myToNode->pos()));
}

void Link::SetLink( NodeItem *fromNode, NodeItem *toNode )
{
    myFromNode = fromNode;
    myToNode = toNode;

    myFromNode->addLink(this);
    myToNode->addLink(this);

    setFlags(QGraphicsItem::ItemIsSelectable);
    setZValue(-1);

    setColor( QColor(238,121,66,100) );
    trackNodes();
}

void Link::Replot()
{
    trackNodes();
}


QVariant Link::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    return QGraphicsItem::itemChange(change, value);
}
