#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsScene>
#include "node.h"
#include "link.h"
#include <QQueue>
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle( QString("多叉树的遍历") );
    m_root = NULL;
    //新建场景
    m_scene = new QGraphicsScene;
    ui->graphicsView->setScene( m_scene );
    m_nodeNum = 1;

    //隐藏中序遍历
     ui->pushButtonIn->setVisible( false );

    ConnectInit();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ShowTree()
{
    //清理场景
    m_scene->clear();
    //计算节点位置
    HoriLayout();
    VerLayout();
    //在场景中画点，连线
    DrwaPointsInScene();

}

void MainWindow::HoriLayout()
{
    if( m_root == NULL )
    {
        return;
    }
    m_xposition = 0;
    CalXPositon( m_root );
}

void MainWindow::CalXPositon(Node *parent)
{
    if( parent == NULL )
    {
        return;
    }

    QPointF point = parent->position();

    QList<Node *> faultChildren = parent->GetSubNode();


    for( int i=0; i<faultChildren.count(); ++i )
    {
        Node * faultChild = faultChildren.at(i);
        CalXPositon( faultChild );
    }

    if( parent->GetSubNode().count() == 0 )
    {
        //叶子节点
        m_xposition += 120;
        point.setX( m_xposition );
        parent->setPosition( point );
        return;
    }
    else
    {
        //非叶子节点x方向坐标为子节点x坐标的中间
        qreal tXPosition = 0;
        for( int i=0; i<faultChildren.count(); ++i )
        {
            tXPosition += faultChildren.at(i)->position().x();
        }
        point.setX( tXPosition/faultChildren.count() );
        parent->setPosition( point );
        return;
    }
}

void MainWindow::VerLayout()
{
    if( m_root == NULL )
    {
        return;
    }

    //设置root点的Y轴坐标
    QPointF pointCurrent = m_root->position();
    pointCurrent.setY( 0 );
    m_root->setPosition( pointCurrent );

    QQueue<Node *> unitQueue;
    unitQueue.enqueue( m_root );

    while( !unitQueue.isEmpty() )
    {
        Node * currentNode= unitQueue.dequeue();
        //如果该节点是转移门，
        //设置该节点的Y轴坐标（父节点纵坐标加VERTICALSPACING）
        if( currentNode->GetParent() != NULL )
        {
            QPointF pointParent = currentNode->GetParent()->position();
            pointCurrent = currentNode->position();
            pointCurrent.setY( pointParent.y() + 100 );
            currentNode->setPosition( pointCurrent );
        }
        QList<Node *> faultChildren= currentNode->GetSubNode();
        for( int i=0; i<faultChildren.count(); ++i )
        {
            unitQueue.enqueue( faultChildren.at(i) );
        }
    }

}

void MainWindow::DrwaPointsInScene()
{

    QMap<Node*,NodeItem*> unitItemMap;

    if( m_root == NULL )
    {
        return;
    }

    //构造节点
    QQueue<Node *> unitQueue;
    unitQueue.enqueue( m_root );

    while( !unitQueue.isEmpty() )
    {
        Node * node= unitQueue.dequeue();
        //新建Item
        QPointF pointF = node->position();//位置

        NodeItem *item = new NodeItem();

        m_scene->addItem(item);
        item->setPos(pointF);
        item->setNode(node);
        unitItemMap[node]=item;
        QList<Node *> ChildrenNode= node->GetSubNode();
        for( int i=0; i<ChildrenNode.count(); ++i )
        {
            unitQueue.enqueue( ChildrenNode.at(i) );
        }
    }

    //构造链接
    QMapIterator<Node*,NodeItem*> nodeItemIter(unitItemMap);
    while( nodeItemIter.hasNext() )
    {
        nodeItemIter.next();
        Node *unit = nodeItemIter.key();

        QList<Node*> tempList = unit->GetSubNode();
        for (int i=0;i<tempList.count();i++)
        {
            Node* subUnit = tempList.at(i);

            NodeItem *startItem = unitItemMap.value(unit);
            NodeItem *endItem = unitItemMap.value(subUnit);
            if( endItem == NULL || startItem == NULL )
                continue;
            Link * link = new Link;
            link->SetLink( startItem,endItem );
            m_scene->addItem( link );
        }
    }
}

void MainWindow::PreRecursion( Node * node )
{
    if( node == NULL )
    {
        return;
    }

    node->setText( QString("Pre-No.%1").arg( m_traversalNo++ ) );

    QList<Node *> faultChildren = node->GetSubNode();
    for( int i=0; i<faultChildren.count(); ++i )
    {
        Node * faultChild = faultChildren.at(i);
        PreRecursion( faultChild );
    }
}

void MainWindow::PostRecursion(Node *node)
{
    if( node == NULL )
    {
        return;
    }


    QList<Node *> faultChildren = node->GetSubNode();
    for( int i=0; i<faultChildren.count(); ++i )
    {
        Node * faultChild = faultChildren.at(i);
        PostRecursion( faultChild );
    }
    node->setText( QString("Post-No.%1").arg( m_traversalNo++ ) );
}

void MainWindow::ConnectInit()
{
    connect( ui->pushButtonAdd,SIGNAL( clicked(bool)), this, SLOT( sl_AddNode() ) );
    connect( ui->pushButtonDelete, SIGNAL( clicked(bool) ), this, SLOT( sl_DeleteNode()) );

    connect( ui->pushButtonPre, SIGNAL( clicked(bool)), this, SLOT(sl_pre()) );
    connect( ui->pushButtonPost, SIGNAL(clicked(bool) ) , this, SLOT( sl_post() ) );
    connect( ui->pushButtonIn, SIGNAL( clicked(bool)), this, SLOT( sl_in( ) ) );
    connect( ui->pushButtonBreadth, SIGNAL( clicked(bool)), this, SLOT( sl_breadth( ) ) );
}

void MainWindow::sl_AddNode()
{
    //判断场景中是否存在节点
    QList<QGraphicsItem *>  allItemsInScene = m_scene->items( );
    if( allItemsInScene.count( ) == 0 )
    {
        m_root = new Node;
        m_root->setText( QString("No.%1").arg( m_nodeNum++ ) );
    }
    else
    {
        QList<QGraphicsItem *> items = m_scene->selectedItems();
        if( items.count() == 0 )
        {
            return;
        }
        NodeItem * nodeItem =dynamic_cast<NodeItem *>( items.at( 0 ) );
        if( nodeItem == NULL )
        {
            return;
        }
        Node * node = nodeItem->GetNode();
        if( node == NULL )
        {
            return;
        }
        Node * subNode = new Node;
        subNode->setText( QString("No.%1").arg( m_nodeNum++ ) );
        node->AddSubNode(subNode );
    }

    ShowTree( );
}

void MainWindow::sl_DeleteNode()
{
    QList<QGraphicsItem *> items = m_scene->selectedItems();
    if( items.count() == 0 )
    {
        return;
    }
    NodeItem * nodeItem =dynamic_cast<NodeItem *>( items.at( 0 ) );
    if( nodeItem == NULL )
    {
        return;
    }
    Node * node = nodeItem->GetNode();
    if( node == NULL )
    {
        return;
    }
    if(  QMessageBox::Ok !=
            QMessageBox::question( this, QString("询问"), QString("是否删除该节点？"),
                                   QMessageBox::Ok|QMessageBox::Cancel ) )
    {
        return;
    }
    Node * nodeParen = node->GetParent();
    if( nodeParen == NULL )
    {
        //node为顶节点
        node->RemoveAllChildren();
        m_root = NULL;
    }
    else
    {
        node->RemoveAllChildren();
        nodeParen->RemoveOneChild( node );
    }
    ShowTree( );

}

void MainWindow::sl_pre()
{
#ifdef RECURSION
    m_traversalNo = 1;
    PreRecursion( m_root );
#else
    if( m_root == NULL )
    {
        return;
    }

    QStack<Node *> stackNode;
    stackNode.push( m_root );
    m_traversalNo = 1;
    QList<Node *> visitedNodeList;//用来存储被访问过的节点
    while( !stackNode.isEmpty() )
    {
        Node * peekNode = stackNode.top();
        //visit parent node
        if( !visitedNodeList.contains(peekNode) )
        {
            visitedNodeList.append( peekNode );//visit it.
            peekNode->setText( QString("Pre-No.%1").arg( m_traversalNo++ )  );
        }
        QList<Node *> subNodeList =  peekNode->GetSubNode();
        bool subVisitedBool = true;
        for( int s = 0; s< subNodeList.count(); ++s )
        {
            Node * node = subNodeList.at( s );
            if( !visitedNodeList.contains(node) )
            {
                stackNode.push( node );
                subVisitedBool = false;
                break;
            }
            else {
                if(s == subNodeList.count()-1)
                {
                    subVisitedBool = true;
                }
                continue;
            }
        }
        if( subVisitedBool )
        {
            stackNode.pop();
        }
    }
#endif
    m_scene->update();
}

void MainWindow::sl_post()
{
#ifdef RECURSION
    m_traversalNo = 1;
    PostRecursion( m_root );
#else
    /***********************************************
     *  （1）栈S初始化；visitedNodeList[n]=0；
     *  （2）访问顶点v；visitedNodeList[v]=1；顶点v入栈S
     *  （3）while(栈S非空)
     *             x=栈S的顶元素(不出栈)；
     *             if(存在并找到未被访问的x的子节点点w)
     *                     w进栈;
     *             else
     *                      x出栈；
     *                      访问w；visitedNodeList[w]=1；
     * ************************************************/
    if( m_root == NULL )
    {
        return;
    }

    QStack<Node *> stackNode;
    stackNode.push( m_root );
    m_traversalNo = 1;
    QList<Node *> visitedNodeList;//用来存储被访问过的节点
    while( !stackNode.isEmpty() )
    {
            Node * peekNode = stackNode.top();
            QList<Node *> subNodeList =  peekNode->GetSubNode();
            bool subVisitedBool = true;
            for( int s = 0; s< subNodeList.count(); ++s )
            {
                Node * node = subNodeList.at( s );
                if( !visitedNodeList.contains(node) )
                {
                    stackNode.push( node );
                    subVisitedBool = false;
                    break;
                }
                else {
                    if(s == subNodeList.count()-1)
                    {
                        subVisitedBool = true;
                    }
                    continue;
                }
            }
            if( subVisitedBool )
            {
                Node * visitNode = stackNode.pop();
                visitedNodeList.append( visitNode );//visit it.
                visitNode->setText( QString("Post-No.%1").arg( m_traversalNo++ )  );
            }
    }
#endif
    m_scene->update();
}

void MainWindow::sl_in()
{

}

void MainWindow::sl_breadth()
{
#ifdef RECURSION

#else
    m_traversalNo = 1;
    QQueue<Node* > queueNode;
    queueNode.enqueue( m_root );
    while( !queueNode.isEmpty() )
    {
        Node* cNode = queueNode.dequeue();
        //DO sth.
        cNode->setText( QString("breath-No.%1").arg( m_traversalNo++ ) );

        QList<Node *> subNodeList = cNode->GetSubNode();
        for( int s = 0; s< subNodeList.count(); ++s )
        {
            Node * subNode = subNodeList.at( s );
            queueNode.enqueue( subNode );
        }
    }

#endif
    m_scene->update();
}
