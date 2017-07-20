#ifndef LINK_H
#define LINK_H

#include <QGraphicsLineItem>

class NodeItem;
class Link;
static const int ZValueEdgeLabel = 80;
class EdgeLabel: public QGraphicsTextItem
{
public:
    EdgeLabel(Link * , int, QString);
    void removeRefs();

    enum { Type = UserType + 6 };
    int type() const { return Type; }

    ~EdgeLabel();

    void Replot();
private:
    Link * m_Link;
    int m_size;
};
class Link : public QGraphicsLineItem
{
public:
    Link();
    Link(NodeItem *fromNode, NodeItem *toNode);
    ~Link();

    enum { Type = UserType + 5 };
    int type() const { return Type; }

    NodeItem *fromNode() const;
    NodeItem *toNode() const;

    void setColor( const QColor &color );
    QColor color() const;

    void setPenSize( double size );
    double penSize() const;

    void trackNodes();

    void SetLink(NodeItem * fromNode, NodeItem * toNode );
    void SetLable( QString text );

    void Replot();

protected:
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant &value);
private:
    NodeItem *myFromNode;
    NodeItem *myToNode;
    double m_PenSize;
};

#endif
