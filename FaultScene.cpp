#include <QtGui>
#include "FaultScene.h"
#include <QDebug>
#include <QApplication>

namespace RORM
{
	FaultScene::FaultScene(QObject *parent)
	{
		line = NULL;
		itemIndexN=0;
        m_startItem = NULL;
        m_nodeOrEventID = 1;

		this->setBackgroundBrush(QPixmap(":/vlTarget/images/background.png"));
		//按 yInteral 将scene平均分成若干个层级
		for ( int i=0; i<FaultScene::sceneHeight; i++ )
		{
			if ( i%FaultScene::yInteral == 0 )
			{
				allYValues.append( i );
			}
		}
        m_project = NULL;
//        setAcceptDrops(true);//接受拖拽事件
	}


	void FaultScene::mousePressEvent( QGraphicsSceneMouseEvent *mouseEvent )
	{
		if ( mouseEvent->button() != Qt::LeftButton ){
			return;
        }
        m_dragStartPosition = mouseEvent->scenePos();
//        m_startItem = mouseGrabberItem();
        QTransform trans;
        m_startItem = itemAt( m_dragStartPosition,trans );

//        QList<QGraphicsItem *> itemList = this->selectedItems();
//        if( itemList.count() == 0 )
//            break;
//        FaultItem * itemSelected = dynamic_cast<FaultItem *>( itemList.at( 0 ) );
//        if( itemSelected == NULL )
//            break;
//        FTNode * ftNode = itemSelected->GetFaultUnit();
//        si_selectFaultNode( ftNode );

		QGraphicsScene::mousePressEvent(mouseEvent);
	}
	
	void FaultScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
	{
        if (!(mouseEvent->buttons() & Qt::LeftButton))
            return;
//        if ((mouseEvent->pos() - m_dragStartPosition).manhattanLength()
//             < QApplication::startDragDistance() )
//            return;
//        QDrag *drag = new QDrag(this);
//        QMimeData *mimeData = new QMimeData;
        //用文本来描述树的结构
//        QByteArray data;
//        mimeData->setData(mimeType, data);
//        drag->setMimeData(mimeData);
//        Qt::DropAction dropAction = drag->exec(Qt::CopyAction | Qt::MoveAction);
        QGraphicsScene::mouseMoveEvent( mouseEvent );
	}
    void FaultScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
    {
        QPointF pos = mouseEvent->scenePos();
//        QTransform trans;
//        QGraphicsItem * itemTem = itemAt( pos,trans );
//        FaultItem * tempItem = dynamic_cast<FaultItem *>( itemTem );

        QList<QGraphicsItem *>itemList = items( pos );
        for( int i=0; i<itemList.count(); ++i )
        {
            QGraphicsItem * item = itemList.at(i);
            if( item != NULL && m_startItem != NULL )//
            {
                if( item == m_startItem )
                {
                    continue;
                }
                //释放地点存在Item
                FaultItem * endItem = dynamic_cast<FaultItem *>( item );
                FaultItem * startItem = dynamic_cast<FaultItem *>( m_startItem );
                if( endItem != NULL && startItem!= NULL )
                {
                    FTNode* endNode = endItem->GetFaultUnit();
                    FTNode* startNode = startItem->GetFaultUnit();

                    if( startNode->GetParent() != NULL )
                    {
                        startNode->GetParent()->RemoveSubNode( startNode );
                        endNode->AddSubNode( startNode );
                        ShowCurrentSystem();
                    }
                }
            }
        }

        QList<QGraphicsItem *> itemLSelectedist = this->selectedItems();
        if( itemLSelectedist.count() == 0 )
        {
            si_selectItemEmpty();
            return;
        }
        FaultItem * itemSelected = dynamic_cast<FaultItem *>( itemLSelectedist.at( 0 ) );
        if( itemSelected == NULL )
            return;
        FTNode * ftNode = itemSelected->GetFaultUnit();
        pos = ftNode->GetPosition();
        if( ftNode != NULL )
        {
            si_selectFaultNode( ftNode );
        }

        QGraphicsScene::mouseReleaseEvent(mouseEvent);
    }

    void FaultScene::dragEnerEvent(QGraphicsSceneDragDropEvent *event)
    {
        event->acceptProposedAction();
    }

    void FaultScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
    {
        event->acceptProposedAction();
    }

    void FaultScene::dropEvent(QGraphicsSceneDragDropEvent *event)
    {
        qDebug() << "hasFun";
        if( event->mimeData()->hasFormat( RormListWidget::MIME_TYPE ) )
        {
            //获取子树
            QByteArray treeidByte = event->mimeData()->data( RormListWidget::MIME_TYPE );
            QString treeIdString( treeidByte );
            FaultTree * faultTree = NULL;
            for( int i=0; i<m_faultTreeList.count(); ++i )
            {
                if( m_faultTreeList.at(i)->GetRoot()->GetCode() == treeIdString )
                {
                    faultTree = m_faultTreeList.at(i);
                    break;
                }
            }
            if( faultTree == NULL )
            {
                return;
            }

            //获得父Item;
            QPointF  pos = event->scenePos();
            QTransform transform;
            QGraphicsItem * item = itemAt( pos,transform );
            if( item == NULL )
            {
                return;
            }
            FaultItem * ftItem = dynamic_cast<FaultItem *>(item);
            if( ftItem ==NULL )
            {
                return;
            }
            FTNode * ftNode = ftItem->GetFaultUnit();
            //新建转移门，转入Item
            FaultItem * transFTItem = new FaultItem;
            TransferItem *  transferItem = transFTItem->GetTransferItem();
            if( transferItem == NULL )
            {
                return;
            }
            transferItem->SetShowBeOr( true );
            transferItem->SetTransType( TransferItem::TRANSFERIN );
            transferItem->SetSubTree( faultTree );
            ftNode->SetCode( faultTree->GetRoot()->GetCode() );
            ftItem->SetFaultUnit( ftNode );
            //给子树顶点添加父
        }
        else if( event->mimeData()->hasFormat( RormTreeWidget::MIME_TYPE ) )
        {
            qDebug() << "hasFormat";
            //获取故障模式
            QByteArray mimeByte = event->mimeData()->data( RormTreeWidget::MIME_TYPE );
            QString mimeString( mimeByte );
            QStringList mimeList = mimeString.split( " ", QString::SkipEmptyParts );
            if( mimeList.count() !=2 )
            {
                qDebug() << "1";
                return;
            }
            QString failureIdString = mimeList.at(0);
            QString deviceIdString = mimeList.at(1);
            qDebug() << failureIdString << deviceIdString;

            FailureMode * failureModel = NULL;
            for( int i=0; i<m_failureModelList.count(); ++i )
            {
                if( m_failureModelList.at(i)->GetCode() == failureIdString )//error
                {
                    failureModel = m_failureModelList.at(i);
                    break;
                }
            }
            if( failureModel == NULL )
            {
                qDebug() << "2";
                return;
            }
            //获取设备
            Device * device = NULL;
            for( int i=0; i<m_deviceList.count(); ++i )
            {
                if( m_deviceList.at(i)->GetCode() == deviceIdString )
                {
                    device = m_deviceList.at(i);
                    break;
                }
            }
            if( device == NULL )
            {
                qDebug() << "3";
                return;
            }
            qDebug() << device->GetName();
            //获得门Item;
            QPointF  pos = event->scenePos();
            QTransform transform;
            QGraphicsItem * item = itemAt( pos,transform );
            if( item == NULL )
            {
                qDebug() << "4";
                return;
            }
            FaultItem * ftItem = dynamic_cast<FaultItem *>(item);
            if( ftItem ==NULL )
            {
                qDebug() << "5";
                return;
            }
            FTNode * ftNode = ftItem->GetFaultUnit();
            if( ftNode == NULL )
            {
                qDebug() << "6";
                return;
            }
            Event * selectEvent = ftNode->GetEvent();
            if( selectEvent == NULL )
            {
                qDebug() << "NO EVENT";
                return;
            }
            FTNode * newFTNode;
            if( selectEvent->GetType() == Event::BASICEVENT )
            {

            }
            else if( selectEvent->GetType() == Event::GATES )
            {
                QMap<FailureMode *, BasicEvent *> failBEMap =  m_project->GetDeFailBEMap( ).value( device );
                //获得基本事件
                Event * newEvent = failBEMap.value( failureModel );
                if( newEvent == NULL )
                {
                    qDebug() << "NO BE";
                    return;
                }
                //新建故障树节点，并绑定基本事件
                newFTNode = new FTNode;
                newFTNode->SetShortId( GenerateNodeID() );
                newFTNode->SetEvent( newEvent );
                ftNode->AddSubNode( newFTNode );

            }

            QMap<Device *,FailureMode *> deviceFailure;
            deviceFailure.insert( device,failureModel );
            m_nodeDevieceFailureMap.insert( newFTNode, deviceFailure );
        }
        ShowCurrentSystem();
    }

	int FaultScene::computeLayer( qreal _y )
	{
		if( _y < 0)
		{
			return 0;
		}else if (_y>=allYValues.at(allYValues.count()-1))
		{
			return allYValues.count()-1;
		}

		int layerUp = 0;
		int layerBottom = 0;
		for(int i=0; i<allYValues.count(); i++)
		{
			layerUp = allYValues.at(i);
			if(_y>=layerBottom && _y<layerUp)
			{
				return i;
			}
			layerBottom = allYValues.at(i);
		}
		return 0;
    }

    void FaultScene::setProject(LPSAProject *project)
    {
        m_project = project;
    }

    QList<Device *> FaultScene::deviceList() const
    {
        return m_deviceList;
    }

    void FaultScene::setDeviceList(const QList<Device *> &deviceList)
    {
        m_deviceList = deviceList;
    }

    int FaultScene::GenerateNodeID()
    {
        for( int i=0; i<m_failureModelList.count(); ++i )
        {
            if( m_failureModelList.at(i)->GetShortId() == m_nodeOrEventID )
            {
                m_nodeOrEventID++;
                return GenerateNodeID();
            }
        }
        return m_nodeOrEventID++;
    }

    QList<FailureMode *> FaultScene::failureModelList() const
    {
        return m_failureModelList;
    }

    void FaultScene::setFailureModelList(const QList<FailureMode *> &failureModelList)
    {
        m_failureModelList = failureModelList;
    }

    QMap<FTNode *, QMap<Device *, FailureMode *> > FaultScene::GetNodeDevieceFailureMap() const
    {
        return m_nodeDevieceFailureMap;
    }

    void FaultScene::AddFTTree( FaultTree* _system )
    {
        m_ftTreeList.append( _system );
    }

    void FaultScene::ShowCurrentSystem()
	{
        if( m_currentTree == NULL )
        {
            return;
        }
        //清空画布上的所有Item
        this->clear();
        //排布树的结构
        m_currentTree->FaultTreeLayout();
		//构造节点
        QMap<FTNode*,FaultItem*> unitItemMap;
        QList<FTNode*> list = m_currentTree->GetSubNodes2Show();
		for (int i=0;i<list.count();i++)
		{
			FTNode *unit = list.at(i);
            Event * event = unit->GetEvent();
            //QString name = unit->GetCode();//名称
            //QString code = unit->GetCode();
			QPointF pointF = unit->GetPosition();//位置

            FaultItem *item = new FaultItem();
			
			addItem(item);
			itemIndexN++;
			item->setPos(pointF);
			item->SetFaultUnit(unit);
            if( event == NULL )
            {
                continue;
            }
            if( event->GetType() == Event::GATES )
            {
                //判断是否显示逻辑门
                if( unit->GetIsTransferNode()
                        && unit != m_currentTree->GetRoot() )
                    item->GetGateItem()->SetShowBeOr( false );
                else
                    item->GetGateItem()->SetShowBeOr( true );
            }
            else if( event->GetType() == Event::BASICEVENT
                     || event->GetType() == Event::HOUSE
                     || event->GetType() == Event::CCFEVENT )
            {
                //判断是否底层事件
                item->GetBasicEventItem( )->setShowOrNot( true );
            }

            //判断是否显示转移门
            if( unit->GetIsTransferNode() == true )
            {
                if( event->GetType() == Event::BASICEVENT )
                {
                    //如果这个节点是基本事件
                    TransferItem * transfer = item->GetTransferItem();
                    transfer->SetShowBeOr( false );
                }
                else if( unit == m_currentTree->GetRoot() )
                {
                    //顶节点显示转出门
                    TransferItem * transfer = item->GetTransferItem();
                    transfer->SetShowBeOr( true );
                    transfer->SetCodeText( unit->GetRootInSubTree()->GetCode() );
                    transfer->SetTransType( TransferItem::TRANSFEROUT );
                    //设置转移符号的位置
                    transfer->setPos( item->boundingRectP().width(),
                                         item->boundingRectP().height()/2 );
                }
                else
                {
                    //底节点显示转入门
                    item;
                    TransferItem * transfer = item->GetTransferItem();
                    transfer->SetShowBeOr( true );
                    transfer->SetTransType( TransferItem::TRANSFERIN );
                    //设置转移符号的位置
                    transfer->setPos( item->boundingRectP().width()/2.0
                                      - transfer->boundingRect().width()/2.0,
                                      item->boundingRectP().height() );
                }
            }
			unitItemMap[unit]=item;
		}


		//构造链接
		for (int j=0;j<list.count();j++)
		{
			FTNode *unit = list.at(j);
            if( unit->GetIsTransferNode()
                    && unit != m_currentTree->GetRoot() )
            {
                continue;
            }
            QList<FTNode*> tempList = unit->GetSubNode();
            for (int i=0;i<tempList.count();i++)
			{
                FTNode* subUnit = tempList.at(i);
                LineType lineType = BROKEN;
				
                FaultItem *startItem = unitItemMap.value(unit);
                FaultItem *endItem = unitItemMap.value(subUnit);
                if( endItem == NULL || startItem == NULL )
                    continue;
				FaultLine *arrow = new FaultLine(startItem,endItem,lineType);
				startItem->addLine(arrow);
				endItem->addLine(arrow);
				this->addItem(arrow);
			}
        }
    }

    FaultTree *FaultScene::GetCurrentTree()
    {
        return m_currentTree;
    }
}
