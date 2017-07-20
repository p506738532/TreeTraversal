#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
/**********************************************************
 * 不足之处：节点Item之间的间距为固定值，而且分辨率改变时，绘图效果大打折扣。
 * 删除的节点没有析构。
 *
 *
 * ********************************************************/
//#define RECURSION//递归宏开关
#pragma execution_character_set("utf-8")

namespace Ui {
class MainWindow;
}

class QGraphicsScene;
class Node;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    void AddNode( Node * parenNode );
    void DeleteNode( Node * deleNode );
    void ShowTree();
    void HoriLayout( );
    void CalXPositon( Node *parent );
    void VerLayout( );

    void DrwaPointsInScene();

    void PreRecursion(Node *node);//前序遍历，递归算法
    void PostRecursion(Node *node );//后序遍历，递归算法

    void ConnectInit( );
    private slots:
    void sl_AddNode( );
    void sl_DeleteNode( );
    void sl_pre( );
    void sl_post( );
    void sl_in( );
    void sl_breadth( );


private:
    Ui::MainWindow *ui;

    QGraphicsScene * m_scene;

    Node * m_root;

    int m_nodeNum;

    int m_traversalNo;

    double m_xposition;


};

#endif // MAINWINDOW_H
