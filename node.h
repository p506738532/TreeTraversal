#ifndef NODE_H
#define NODE_H

#include <QApplication>
#include <QColor>
#include <QGraphicsItem>
#include <QSet>
#include <QStack>
class Link;

class Node
{
public:
    Node();
    ~Node();
    QString text() const;
    void setText(const QString &text);

    QPointF position() const;
    void setPosition(const QPointF &position);

    Node *GetParent() const;


    QList<Node *> GetSubNode() const;
    void AddSubNode(Node *node);

    void RemoveOneChild( Node * child );
    void RemoveAllChildren( );

private:
    void RemoveParent( );
    void setParentNode(Node *parentNode);


private:
    QString m_text;
    QPointF m_position;
    Node * m_parentNode;
    QList<Node *> m_nodeList;
};

class NodeItem : public QGraphicsItem
{
    Q_DECLARE_TR_FUNCTIONS(Node)

public:
    NodeItem();
    NodeItem( Node * node );
    ~NodeItem();

    void setText(const QString &text);
    QString text() const;
    void setTextColor(const QColor &color);
    QColor textColor() const;
    void setOutlineColor(const QColor &color);
    QColor outlineColor() const;
    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const;

    void addLink(Link *link);
    void removeLink(Link *link);

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);


    void SetSize(float size);
    void AddChildNode(NodeItem * child);



    void PushLinkText( QString text){ m_LinkText.push( text ); }
    QString PopLinkText( ) { return m_LinkText.pop(); }
    bool LinkStackIsEmpty() { return m_LinkText.isEmpty();}

    void SetCaseCount( int count ) { m_CaseCount = count; }
    int GetCaseCount() { return m_CaseCount; }

    void setNode(Node *node);
    Node *GetNode( );
protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant &value);

private:
    QRectF outlineRect() const;
    int roundness(double size) const;

    Node * m_node;

    QSet<Link *> myLinks;
    QColor myTextColor;
    QColor myBackgroundColor;
    QColor myOutlineColor;
    QPainterPath *m_path;
    QStack<QString> m_LinkText;
    int m_CaseCount;//计数
};


#endif
